#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "match_result.h"
#include "rule_filters.h"
#include "utils.h"
#include "action.h"
#include "extensions.h"
#include "states.h"

#include <optional>

namespace ctle {
/**
 * @brief The main lexer class.
 *
 * @tparam Rules an instance of ctle::rules.
 * @tparam States an instance of ctle::states.
 * @tparam Extensions an instance of ctle::extensions.
 * @tparam Actions an instance of ctle::actions, these are the actions used in the initial state,
 * which is not user defined by itself but its actions can be.
 * @tparam IteratorT a ForwardIterator.
 */
template<typename ReturnT, typename Rules, typename States = states<>,
         typename Extensions = extensions<>, typename Actions = defaults<ReturnT>,
         typename IteratorT = const char*>
class lexer
  : public Extensions::template inner<lexer<Rules, States, Extensions, Actions, IteratorT>>
{
    /**@brief The return type of rules (and of lex() function implicitly). */
    using rule_return_t = ReturnT;
    /** @brief A list of rules. To avoid the typename hell. */
    using rule_list = Rules;
    /** @brief The type representing states. */
    using state_t = typename States::type;
    /** @brief The list of rules.*/
    using state_list = typename States::list;
    /** @brief The type of characters in the input. */
    using char_t = std::remove_cvref_t<decltype(*std::declval<IteratorT>())>; // dereference
    /** @brief A pair of iterators representings the input range. */
    using input_range_t = input_range<IteratorT>;
    /** @brief The type used to represent lexemes. */
    using string_view_t = std::basic_string_view<char_t>;
    /** @brief The return type of lex() function, a token and a lexeme. */
    using return_t = std::tuple<rule_return_t, string_view_t>;
    /**
     * @brief The internal return type used in match function, the optional signals whether to
     * return or not and the string view is the lexeme.
     */
    using match_return_t = std::tuple<std::optional<rule_return_t>, string_view_t>;
    /** @brief prototype of a match function as one is generated for each state. */
    using match_signature_t = match_return_t (lexer::*)();
    /**
     * @brief Represents a state-function pair, the state is the identifier and the function is its
     * match function.
     */
    using state_function_pair_t = std::pair<int, match_signature_t>;
    /** @brief current matching function (switched when state is switched). */
    match_signature_t current_match_function{nullptr};
    /** @brief The current range we're lexing. */
    input_range_t m_input{};
    /**
     * @brief a function representing no action, just returns an empty optional.
     *
     * @tparam ResultT a result of match provided by rule::match.
     * @return std::optional<rule_return_t> an empty one.
     */
    template<typename ResultT>
    static std::optional<rule_return_t> no_action(lexer&, ResultT&&) {
        return std::nullopt;
    }
    /** @brief The type which can store the match result lexemes (even with captures). */
    using storage_t
      = std::array<std::basic_string_view<char_t>, capture::max<IteratorT, IteratorT>(rule_list{})>;
    /**
     * @brief The required signature of an action. Must take the storage_t. Uses just first N
     * elements internally.
     */
    using action_signature_t = std::optional<rule_return_t> (*)(lexer&, storage_t&&);
    /** @brief The type wihich encapsulates the storage and an action attached to it. */
    using match_result_t = match_result<storage_t, action_signature_t>;
    /**
     * @brief an implementation of a rule within the lexer.
     *
     * @tparam Rule a ctle::rule in which the provided data is stored.
     */
    template<typename Rule>
    class rule
    {
        /** @brief calls the match function of the ctle::rule. */
        static CTLL_FORCE_INLINE auto match_pattern(input_range_t range) {
            return Rule::match(range.begin, range.end);
        }
        /** @brief the type of result returned by match in the rule. */
        using result_t = decltype(match_pattern(std::declval<input_range_t>()));
        /**
         * @brief a wrapper around an actual call to the action. This way we can get function
         * pointers to lambdas with captures etc. This call knows how many elements from storage it
         * should forward to the action itself as it knows how many captures this rule has.
         *
         * @param l a reference to a lexer.
         * @param p a result of lexer_rule::match.
         * @return std::optional<rule_return_t> behaves as every action.
         */
        static std::optional<rule_return_t> action_impl(lexer& l, storage_t&& p) {
            static constexpr auto tmp = Rule::action;
            return apply_tuple<action<tmp, rule_return_t>{}, std::tuple_size_v<result_t>>(
              std::move(p), l);
        }
        /**
         * @brief gets the correct pointer to a function. If rule has action, returns a pointer
         * to function executing said action. If not returns a potiner to a function that just
         * returns an empty optional (just like empty_action would).
         *
         * @return pointer to the correct action.
         */
        static constexpr action_signature_t get_action() {
            if constexpr (Rule::action.empty())
                return nullptr;
            else
                return &action_impl;
        }

    public:
        /**
         * @brief tries to match the input to the provided rule.
         *
         * @param input a input_range_t that contains the begin and end iterators to specify the
         * scanned range.
         * @return auto a match_result, containing a match and an action.
         */
        static CTLL_FORCE_INLINE auto match(input_range_t input) {
            return match_result_t{match_pattern(input), get_action()};
        }
        /**@brief the return type of calling match. */
        using return_t = decltype(match(std::declval<input_range_t>()));
    };

public:
    /** @brief Ctor. */
    lexer() noexcept { set_state(state_initial); }
    /**
     * @brief Set the state of lexer.
     *
     * @param state state to be set
     * @return true on success.
     * @return false if no such state exists.
     */
    bool set_state(int state) noexcept {
        auto it = std::lower_bound(m_state_functions.cbegin(), m_state_functions.cend(),
                                   state_function_pair_t{state, nullptr},
                                   [](const auto& a, const auto& b) { return a.first < b.first; });

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
    bool set_state(state_t state) noexcept { return set_state(static_cast<int>(state)); }
    /**
     * @brief tells lexer to match another rule
     *
     * @return return_t return of the first rule which has action that returns.
     */
    return_t lex() {
        while (true) {
            if (auto [retval, lexeme] = (this->*current_match_function)(); retval)
                return {std::move(retval.value()), lexeme};
        }
    }
    /**
     * @brief Set the input.
     *
     * @param args Constor arguments for the input range.
     */
    void set_input(const auto& input) { m_input = input_range_t{input.begin(), input.end()}; }
    /**
     * @brief Get the input range in its current state.
     *
     * @return input_range_t
     */
    input_range_t get_input() { return m_input; }

private:
    /**
     * @brief create one pair of identifier/function
     *
     * @tparam State the ctle::state object.
     * @return assert_start wheter or not to assert the start of user defined states.
     */
    template<typename State, bool assert_start = true>
    static constexpr auto make_state_function_pair() {
        if constexpr (assert_start)
            static_assert(State::identifier() >= state_reserved,
                          "All states must begin at state_reserved.");
        // the next four statements are there to not ICE gcc.
        // get the eof action for this state (if none specified uses the one for initial state).
        constexpr auto chosen_eof
          = callable_utils::get_default<State::actions::eof, Actions::eof>();
        constexpr auto eof_action = action<chosen_eof, rule_return_t>{};
        // get the no match action for this state (if none specified uses the one for initial
        // state).
        constexpr auto chosen_no_match
          = callable_utils::get_default<State::actions::no_match, Actions::no_match>();
        constexpr auto no_match_action = action<chosen_no_match, rule_return_t>{};
        // take pointer to a method with a filtered set of rules.
        constexpr match_signature_t function_ptr = &lexer::match_impl<
          actions<eof_action, no_match_action>,
          typename state_filter<State, state_initial>::template filtered_t<rule_list>>;
        // add this matching function to array of all matching functions.
        return state_function_pair_t{State::identifier(), function_ptr};
    }
    /** @brief creates matching functions for all states and sorts them by identifiers. */
    template<typename... StateDefinition>
    static constexpr auto make_state_functions(ctll::list<StateDefinition...>) {
        std::array retval{make_state_function_pair<state<state_initial, true, Actions>, false>(),
                          make_state_function_pair<StateDefinition>()...};

        sort(retval, [](const auto& a, const auto& b) { return a.first < b.first; });

        return retval;
    }
    /**
     * @brief the function from which the matching functions are generated.
     *
     * @see match.
     */
    template<typename LocalActions, typename filtered_rules>
    match_return_t match_impl() {
        return match<LocalActions>(filtered_rules());
    }
    /**
     * @brief real match function, uses fold statement to match all rules. Also moves input
     * iterator by length of matched text.
     *
     * @tparam eof_handler a handler if an EOF is encountered in this state.
     * @tparam rule_list rules to be used.
     * @return return_t value returned by action of a longest matched rule if it is returning,
     * otherwise matches till such rule is encountered or EOF.
     */

    /**
     * @brief real match function, uses fold statement to match all rules. Also moves input
     * iterator by length of matched text.
     *
     * @tparam LocalActions specifying the eof and no_match actions in this state.
     * @tparam Rule A pack of rules.
     * @return A tuple of std::optional<rule_return_t> and the lexeme as a string_view.
     */
    template<typename LocalActions, typename... Rule>
    CTLL_FORCE_INLINE match_return_t match(ctll::list<Rule...>) noexcept {
        // handle eof
        if (m_input.begin == m_input.end)
            [[unlikely]] return match_return_t{LocalActions::eof(*this), string_view_t{}};
        // try to match
        auto result = (match_result_t{nullptr} | ... | rule<Rule>::match(m_input));
        // hadnle no_match
        if (!result.length())
            [[unlikely]] return match_return_t{LocalActions::no_match(*this), string_view_t{}};
        // advance iterator in read stream (file)
        std::advance(m_input.begin, result.length());
        // handle matched rule w/o action.
        return match_return_t{result.do_action(*this), result.to_view()};
    }
    /** @brief an array holding a function pointer for each state (something like a vtable). */
    static constexpr auto m_state_functions{make_state_functions(state_list())};
};

} // namespace ctle
#endif // CTLE_LEXER