#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "lexer_rule.h"
#include "type_filter.h"
#include "rule_filters.h"
#include "utils.h"
#include "ctle_concepts.h"
#include "default_actions.h"
#include "action.h"
#include "file_stack.h"

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
	 * @tparam EofHandler a handler which gets called on eofs.
	 * @tparam NoMatchHandler a handler if nothing matches.
	 * @tparam FileType a class representing input file (needs to implement initialize() -> bool).
	 */
	template<	
		typename ReturnType,
		typename Rules = ctll::list<>,
		typename States = nullptr_t,
		typename StateDefinitions = ctll::list<>,
		typename DerivesFrom = derives_from<>,
		callable EofHandler = default_actions::eof(ReturnType::eof),
		callable NoMatchHandler = default_actions::simple_return(ReturnType::no_match),
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
		using match_signature_t = std::optional<ReturnType> (lexer::*)();
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
		 * @return return_t return of the first rule which has action that returns.
		 */
		return_t lex() {
			std::optional<return_t> retval{std::nullopt};
			
			do {
				retval = (this->*current_match_function)();
			} while (!retval);

			return retval.value();		
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
			std::advance(m_current_file.begin, - n);
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
		 * @tparam StateEofHandler the eof handler to be used for this state. 
		 * @return assert_start wheter or not to assert the start of user defined states.
		 */
		template<int State, typename Filter, callable StateEofHandler, bool assert_start = true>
		static constexpr auto make_state_function_pair() {
			if constexpr (assert_start)
				static_assert(State >= state_reserved, "All states must begin at state_reserved.");
			// the next four lines are there to not ICE gcc.
			constexpr auto def_eof = EofHandler;
			constexpr auto state_eof = StateEofHandler;
			constexpr auto used_eof = callable_utils::get_default<state_eof, def_eof>();
			constexpr auto eof_action = action<used_eof, return_t>{};
			// take pointer to a method with a filtered set of rules.
			constexpr match_signature_t function_ptr = &lexer::match_impl<eof_action, typename Filter::template filtered_t<Rules>>;
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
					make_state_function_pair<state_initial, initial_filter<StateDefinitions>, EofHandler, false>(),
					make_state_function_pair<StateDefinition::identifier(), state_filter<StateDefinition::identifier()>, StateDefinition::eof_action()>()...
				};

			sort(retval, [](const auto& a, const auto& b){ return a.first < b.first; });

			return retval;
		}
		/**
		 * @brief the function from which the matching functions are generated.
		 * 
		 * @tparam eof_handler a handler if an EOF is encountered in this state.
		 * @tparam filtered_rules set of rules to use in this function.
		 * @return std::optional<return_t> return of match (if returning).
		 */
		template<callable eof_handler, typename filtered_rules>
		std::optional<return_t> match_impl() {
			static constexpr auto eof_handler_copy = eof_handler; // gcc shenanigans.
			return match<eof_handler_copy>(filtered_rules());
		}
		/**
		 * @brief a handler for a case of nothing being matched.
		 * 
		 * @return auto behaves like any other action.
		 */
		auto no_match_handler() {
			static constexpr auto no_match_copy = NoMatchHandler;
			static constexpr auto no_match_action = action<no_match_copy, return_t>{};
			return no_match_action(*this);
		}
		/**
		 * @brief real match function, uses fold statement to match all rules. Also moves input iterator by length of matched text.
		 * 
		 * @tparam eof_handler a handler if an EOF is encountered in this state.
		 * @tparam rule_list rules to be used.
		 * @return return_t value returned by action of a longest matched rule if it is returning, otherwise matches till such rule is encountered or EOF.
		 */
		template<callable eof_handler, typename... rule_list>
		std::optional<return_t> match(ctll::list<rule_list...>) noexcept {
			std::optional<return_t> retval;
			// handle eof
			if (m_current_file.begin == m_current_file.end) return eof_handler(*this);
			// try to match
			auto result = (variant_return_t{} | ... | variant_return_t{rule_list::match(m_current_file.begin, m_current_file.end)});
			// hadnle no_match
			if (!result.length()) return no_match_handler();
			// advance iterator in read stream (file)
			std::advance(m_current_file.begin, result.length());
			// update the stored text
			update_text(result.to_view());
			// handle matched rule w/o action.
			if (!result.has_action()) return std::nullopt;
			// execute an action if rule specifies one.
			return (result.has_action() ? result.do_action(*this) : std::optional<return_t>{});
		}
		/**
		 * @brief an array holding a function pointer for each state (something like a vtable).
		 */
		static constexpr auto m_state_functions{make_state_functions(StateDefinitions())};
	};

}
#endif // CTLE_LEXER