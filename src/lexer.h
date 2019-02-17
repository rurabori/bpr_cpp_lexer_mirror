#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "lexer_rule.h"
#include "type_filter.h"
#include "rule_filters.h"
#include "utils.h"
#include "ctle_concepts.h"
#include "file_stack.h"
#include "actions.h"

#include "ctre.hpp"
#include "ctll/list.hpp"
#include "wise_enum.h"

#include <optional>
#include <variant>

namespace ctle {
	/**
	 * @brief holds all information needed for lexing.	
	 * 
	 * @tparam ReturnType return type of actions in rules (fx. enum in which tokens are specified).
	 * @tparam Rules a ctll::list of ctle::lexer_rules.
	 * @tparam States an enum in which states are defined.
	 * @tparam StateDefinitions the definition of a state itself ((non)exclusive | eof action etc.).
	 * @tparam DerivesFrom a derives_from specialization of all the base clases. This way you can add your own behavior to lexer (fx. count lines).
	 * @tparam EofHandler a callable which gets called on eofs.
	 * @tparam NoMatchHandler a handler if nothing matches.
	 * @tparam FileType a class representing input file (needs to implement initialize() -> bool).
	 */
	template<	
		typename ReturnType,
		typename Rules = ctll::list<>,
		typename States = nullptr_t,
		typename StateDefinitions = ctll::list<>,
		typename DerivesFrom = derives_from<>,
		typename EofHandler = default_actions::eof<ReturnType>,
		typename NoMatchHandler = default_actions::no_match<ReturnType>,
		typename FileType = basic_file<char>
	>
	class lexer : public DerivesFrom
	{
		// a stack of files (used for includes etc.).
		using stack_t = ctle::basic_file_stack<FileType>;
		using file_iterator_t = typename FileType::iterator_t;
		using file_range_t = typename stack_t::file_range;
		/**
		 * @brief Get all possible return types of rules
		 * 
		 * @tparam rule all rules.
		 * @return ctll::list<typename rule::template return_t<file_iterator_t>...> all possible return types.
		 */
		template<typename... rule>
		static constexpr auto get_return_pack(ctll::list<rule...>) -> ctll::list<typename rule::template return_t<file_iterator_t, file_iterator_t>...>;
		// a small utility for the function above.
		using return_pack_t = decltype(get_return_pack(std::declval<Rules>()));
		/**
		 * @brief a variant which holds all possible return values of lexer_rule::match
		 */
		using variant_return_t = ctle::unique_from_list_to_type_t<ctle::variant_match_wrapper, decltype(ctll::concat(ctll::list<empty_match_result<char>>(), return_pack_t()))>;
		/**
		 * @brief prototype of a match function as one is generated for each state.
		 */
		using match_signature_t = ReturnType (lexer::*)();
		/**
		 * @brief a pair of index / signature.
		 */
		using state_function_pair_t = std::pair<int, match_signature_t>;
		/**
		 * @brief current matching function (switched when state is switched).
		 */
		match_signature_t current_match_function{nullptr};	
		// self explanatory.
		stack_t 				m_stack{};
		file_range_t			m_current_file{};

		bool 					m_more{false};
	public:
		using state_t = States;
		using return_t = ReturnType;
		using string_t = typename std::basic_string<typename FileType::char_t>;
		/**
		 * @brief the text matched by lexer on the last call of lex(). Available to actions as well.
		 */
		string_t text{};


		lexer() {
			set_state(state_initial);
		}
		/**
		 * @brief Set the state of lexer.
		 * 
		 * @param state state to be set
		 * @return true on success.
		 * @return false if no such state exists.
		 */
		bool set_state(int state) {
			auto it = std::lower_bound(m_state_functions.cbegin(), m_state_functions.cend(), 
											state_function_pair_t{state, nullptr}, [](const auto& a, const auto& b){ return a.first < b.first; });

			if (it == m_state_functions.cend() || it->first != state) return false;
			
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
			// store the current location of lexed file.
			m_stack.store(m_current_file.begin);
			// create a new one.
			auto retval = m_stack.push(std::forward<Args>(c_args)...);
			if (!retval)
				return false;
			// reset the input iterators.
			reset_iterators();
			return true;
		}
		/**
		 * @brief removes a file from lexer stack.
		 * 
		 * @return true if any files are left.
		 * @return false otherwise.
		 */
		bool pop_file() noexcept {
			if (m_stack.empty())
				return false;

			m_stack.pop();
			reset_iterators();
			return !m_stack.empty();
		}
		/**
		 * @brief 	specifies to lexer, that the matched text should be kept and the next match should just append to it.
		 * 			so if first rule matches "yes" and calls this, and the second rule matches " please", the text variable
		 * 			for the next match will be "yes please".
		 * 
		 */
		void more() noexcept {
			m_more = true;
		}
		/**
		 * @brief returns first n characters to the input. It's users responsibility to not return more than matched.
		 * 
		 * @param n how many characters should be returned, if 0 all matched text is returned.
		 */
		void less(size_t n = 0) {
			n = (n) ? n : text.length();
			std::advance(m_current_file.begin(), - n);
			text.resize(text.length() - n);
		}

	private:
		/**
		 * @brief updates the text variable, if more is set, appends to current text.
		 * 
		 * @param view the view of the current match.
		 */
		void update_text(auto view) {
			if (m_more) {
				text.append(view);
				m_more = false;
			} else {
				text = view;
			}
		}
		/**
		 * @brief resets the current iterators to the new file. Needs to be called after each successful stack operation (push and pop).
		 */
		void reset_iterators() {
			m_current_file = m_stack.top();
		}
		/**
		 * @brief create one pair of identifier/function
		 * 
		 * @tparam State the identifier of this state.
		 * @tparam Filter the filter to be used on rules.
		 * @tparam StateEofHandler the eof handler to be used for this state (if std::nullptr_t default used). 
		 * @return assert_start wheter or not to assert the start of user defined states.
		 */
		template<int State, typename Filter, typename StateEofHandler, bool assert_start = true>
		static constexpr auto make_state_function_pair() {
			if constexpr (assert_start)
				static_assert(State >= state_reserved, "All states must begin at state_reserved.");
			// take pointer to a method with a filtered set of rules.
			constexpr auto function_ptr = &lexer::match_impl<get_default::type_t<StateEofHandler, EofHandler>, typename Filter::template filtered_t<Rules>>;
			// add this matching function to array of all matching functions.
			return state_function_pair_t{State, function_ptr};
		}
		/**
		 * @brief creates matching functions for all states and sorts them by identifiers.
		 * 
		 */
		template<typename... StateDefinition>
		static constexpr auto make_state_functions(ctll::list<StateDefinition...>) {
			std::array retval{
					make_state_function_pair<state_initial, initial_filter<StateDefinitions>, std::nullptr_t, false>(),
					make_state_function_pair<StateDefinition::identifier(), state_filter<StateDefinition::identifier()>, typename StateDefinition::eof_action_t>()...
				};

			sort(retval, [](const auto& a, const auto& b){ return a.first < b.first; });

			return retval;
		}
		/**
		 * @brief the function from which the matching functions are generated.
		 * 
		 * @tparam filtered_rules set of rules to use in this function.
		 * @return return_t return of match.
		 */
		template<typename eof_handler, typename filtered_rules>
		return_t match_impl() {
			return match<eof_handler>(filtered_rules());
		}
		/**
		 * @brief real match function, uses fold statement to match all rules. Also moves input iterator by length of matched text.
		 * 
		 * @tparam rule_list rules to be used.
		 * @return return_t value returned by action of a longest matched rule if it is returning, otherwise matches till such rule is encountered or EOF.
		 */
		template<typename eof_handler, typename... rule_list>
		return_t match(ctll::list<rule_list...>) noexcept {
			std::optional<return_t> retval;
			if (m_current_file.begin == m_current_file.end) {
				retval = callable<eof_handler, return_t>()(*this);
			} else {
				auto result = (variant_return_t{} | ... | variant_return_t{rule_list::match(m_current_file.begin, m_current_file.end)});
				if (!result.length()) 
					retval = callable<NoMatchHandler, return_t>()(*this);

				std::advance(m_current_file.begin, result.length());
				update_text(result.to_view());

				if (result.has_action()) 
					retval = result.do_action(*this);
			}

			return (retval) ? std::move(retval.value()) : (this->*current_match_function)();
		}

		/**
		 * @brief an array holding a function pointer for each state (something like virtual table).
		 */
		static constexpr auto m_state_functions{make_state_functions(StateDefinitions())};
	};

}
#endif // CTLE_LEXER