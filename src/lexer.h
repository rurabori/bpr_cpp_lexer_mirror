#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "lexer_rule.h"
#include "type_filter.h"
#include "rule_filters.h"
#include "utils.h"
#include "ctle_concepts.h"
#include "file_stack.h"

#include "ctre.hpp"
#include "ctll/list.hpp"
#include "wise_enum.h"

#include <iostream>
#include <optional>
#include <stack>
#include <variant>

namespace ctle {

	/**
	 * @brief holds all information needed for lexing.
	 * 
	 * @tparam ReturnType return type of actions in rules (fx. enum in which tokens are specified).
	 * @tparam Rules a ctll::list of ctle::lexer_rules.
	 * @tparam States an enum in which states are defined.
	 * @tparam StateDefinitions an array of state<States> to forther clasify states as (non)exclusive.
	 */
	template<	
		typename ReturnType,
		typename Rules = ctll::list<>, 
		typename States = nullptr_t, 
		std::array StateDefinitions = {}
	>
	class lexer
	{
	public:
		using state_t = States;
		using return_t = ReturnType;
		using string_view_t = typename std::basic_string_view<char>;
		using stack_t = ctle::basic_file_stack<ctle::basic_file<char>>;
		using file_iterator_t = typename stack_t::iterator;

		/**
		 * @brief Get all possible return types of rules
		 * 
		 * @tparam rule all rules.
		 * @return ctll::list<typename rule::template return_t<file_iterator_t>...> all possible return types.
		 */
		template<typename... rule>
		static constexpr auto get_return_pack(ctll::list<rule...>) -> ctll::list<typename rule::template return_t<file_iterator_t>...>;

		using return_pack_t = decltype(get_return_pack(std::declval<Rules>()));
		/**
		 * @brief a variant which holds all possible return values of lexer_rule::match
		 */
		using variant_return_t = ctle::unique_from_list_to_type_t<std::variant, return_pack_t>;
		/**
		 * @brief required signature of lexer_rule action.
		 */
		using action_signature_t = std::optional<ReturnType>(*)(lexer&, variant_return_t&&);
	private:
		/**
		 * @brief return type of match function.
		 */
		using match_t = std::optional<ReturnType>;
		/**
		 * @brief prototype of a match function as one is generated for each state.
		 */
		using match_signature_t = match_t (lexer::*)();
		/**
		 * @brief a pair of index / signature.
		 */
		using state_function_pair_t = std::pair<int, match_signature_t>;
		/**
		 * @brief an array holding a function pointer for each state (something like virtual table).
		 */
		std::array<state_function_pair_t, StateDefinitions.size() + 1> m_state_functions{state_function_pair_t{0, nullptr}};
		/**
		 * @brief current matching function (switched when state is switched).
		 */
		match_signature_t current_match_function{nullptr};	
		// self explanatory.
		std::string_view 		m_current_match;
		stack_t 				m_stack{};
		file_iterator_t 		m_begin{};
		file_iterator_t 		m_end{};
		variant_return_t 		m_return_value;
		std::optional<return_t> m_return;

	public:
		constexpr lexer() {
			static_assert(wise_enum::is_wise_enum_v<States>, "Not a wise choice my friend, use WISE_ENUM");
			fill_state_functions(std::make_index_sequence<StateDefinitions.size()>());	
			set_state(state_initial);
		}
		/**
		 * @brief Set the state of lexer.
		 * 
		 * @param state state to be set
		 * @return true on success.
		 * @return false if no such state exists.
		 */
		constexpr bool set_state(int state) {
			auto it = std::find_if(m_state_functions.cbegin(), m_state_functions.cend(), [state](const auto& r){ return r.first == state; });
			
			if (it == m_state_functions.cend()) return false;

			current_match_function = it->second;
			return true;
		}
		/**
		 * @brief Set the state of lexer.
		 * 
		 * @param state state to be set
		 * @return true on success.
		 * @return false if no such state exists.
		 */
		constexpr bool set_state(state_t state) {
			return set_state(static_cast<int>(state));
		}
		/**
		 * @brief tells lexer to match another rule
		 * 
		 * @return auto return of the first rule which has action that returns.
		 */
		auto lex() {
			return std::pair{"", (this->*current_match_function)()};		
		}
		/**
		 * @brief adds a file to lexer stack.
		 * 
		 * @tparam Args forwarded to constructor of file.
		 * @param c_args forwarded to constructor of file.
		 * @return true file creation succeeded.
		 * @return false file creation failed.
		 */
		template <typename... Args>
		bool add_file(Args&&... c_args) {
			auto retval = m_stack.push(std::forward<Args>(c_args)...);
			if (!retval)
				return false;
			m_begin = m_stack.begin();
			m_end = m_stack.end();
			return true;
		}

	private:
		/**
		 * @brief creates matching function for one state.
		 * 
		 * @tparam index of this state function.
		 */
		template<auto index = 0> 
		constexpr void fsf_impl() {
			constexpr auto new_index = index + 1;
			static_assert(static_cast<int>(StateDefinitions[index].value) >= state_reserved, "All states must begin at state_reserved.");
			// create a filter for this state
			using filter = state_filter<StateDefinitions[index].value>;
			// take pointer to a method with a filtered set of rules.
			auto function_ptr = &lexer::match_impl<typename filter::template filtered_t<Rules>>;
			// add this matching function to array of all matching functions.
			m_state_functions[new_index] = state_function_pair_t{static_cast<int>(StateDefinitions[index].value), function_ptr};
		}
		/**
		 * @brief creates matching functions for all states.
		 * 
		 * @tparam idx index sequence used in fold.
		 */
		template <size_t... idx>
		constexpr void fill_state_functions(std::index_sequence<idx...>) {
			// this one is a bit special, as we do not check the value of state (starts at ...) also using different filter.
			constexpr auto tmp = StateDefinitions; // gcc just does not like sending cnttp parameters around.
			// create a filter for this state
			using filter = initial_filter<tmp>;
			// take pointer to a method with a filtered set of rules.
			auto function_ptr = &lexer::match_impl<typename filter::template filtered_t<Rules>>;
			// add this matching function to array of all matching functions.
			m_state_functions[0] = {state_initial, function_ptr};
			// fill other state functions
			(fsf_impl<idx>() , ...);
		}
		/**
		 * @brief the function from which the matching functions are generated.
		 * 
		 * @tparam filtered_rules set of rules to use in this function.
		 * @return match_t return of match.
		 */
		template <typename filtered_rules>
		match_t CTRE_FORCE_INLINE match_impl() {
			return match(filtered_rules());
		}
		/**
		 * @brief real match function, uses fold statement to match all rules. Also moves input iterator by length of matched text.
		 * 
		 * @tparam rule_list rules to be used.
		 * @return match_t value returned by action of a longest matched rule if it is returning, otherwise matches till such rule is encountered or EOF.
		 */
		template <typename... rule_list>
		match_t CTRE_FORCE_INLINE match(ctll::list<rule_list...>) noexcept {			
			while (m_begin != m_end) {
				auto result = (lexer_result<variant_return_t,action_signature_t>{variant_return_t{}, nullptr} | ... | rule_list::template match<lexer>(m_begin, m_end));
				if (!result.length())
					break;
				
				std::advance(m_begin, result.length());

				if (result.action) {
					if (auto retval = result.action(*this, std::move(result.match)); retval)
						return retval;
				}
			}

			return ReturnType::unknown;
		}	
	};

}
#endif // CTLE_LEXER