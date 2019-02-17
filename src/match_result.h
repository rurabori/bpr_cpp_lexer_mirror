#ifndef CTLE_LEXER_RESULT
#define CTLE_LEXER_RESULT

#include "action.h"

#include <variant>
#include <optional>

namespace ctle {

    
    /**
     * @brief holds a result of match operation
     * 
     * @tparam ResultType the type of return for a pattern creating this (fx. rules with captures have different returns).
     * @tparam Action, a callable object which is called if this is the best match.
     */
    template <typename ResultType, typename Action = std::nullptr_t>
    struct match_result {
        ResultType      match;

        template <typename ReturnType>
        using action_t = action<Action, ReturnType>;
        /**
         * @brief get a view of the match.
         * 
         * @return a std::(w)string_view
         */
        auto to_view() const noexcept {
            return match.to_view();
        }
        /**
         * @brief get length of current match.
         * 
         * @return length of the current match.
         */
        auto length() const noexcept {
            return to_view().length();
        }
        /**
         * @brief   check whether rule creating this provided an action to execute
         *          if this is the longest match.
         * 
         * @return bool
         */
        static constexpr bool has_action() { return !std::is_same_v<Action, std::nullptr_t>; }
        /**
         * @brief executes an action if this is the result picked as the best.
         * 
         * @param lexer reference to lexer to pass to the action being executed.
         * @return a std::optional of return_t obtained from lexer. If empty, the action is non-returning.
         */
        template<typename = std::enable_if_t<has_action()>>
        auto do_action(LexerInterface& lexer) {
            using return_t = typename std::remove_reference_t<decltype(lexer)>::return_t;
            return ctle::apply_tuple<action_t<return_t>>(match, lexer);
        }
    };


    template <typename CharType>
    struct empty_match_result {
        auto to_view() const noexcept {
            return std::basic_string_view<CharType>{};
        }
        /**
         * @brief get length of current match.
         * 
         * @return length of the current match.
         */
        auto length() const noexcept {
            return to_view().length();
        }
        /**
         * @brief   check whether rule creating this provided an action to execute
         *          if this is the longest match.
         * 
         * @return bool
         */
        static constexpr bool has_action() { return false; }
    };

    /**
     * @brief a small wrapper around std::variant.
     * 
     * @tparam Args variadic pack passed to std::variant.
     */
    template<typename EmptyMatch, typename... Args>
    class variant_match_wrapper {
        using variant_t = std::variant<EmptyMatch, Args...>;
        variant_t m_match; //< holds all possible match results.
    public:
        variant_match_wrapper() : m_match{EmptyMatch{}} {}
        variant_match_wrapper(variant_match_wrapper&& other) : m_match{std::move(other.m_match)} {}
        variant_match_wrapper(variant_t&& match) : m_match{std::move(match)} {}

        /**
         * @see match_result::length()
         */
        auto length() const noexcept { return std::visit([](auto&& value){ return value.length(); }, m_match); }
        /**
         * @see match_result::has_action()
         */
        constexpr auto has_action() const noexcept { return std::visit([](auto&& value){ return value.has_action(); }, m_match); }


        auto to_view() const noexcept { return std::visit([](auto&& value){ return value.to_view(); }, m_match); }
        /**
         * @see match_result::do_action()
         */

        auto do_action(LexerInterface& lexer) noexcept { 
            using return_t = std::optional<typename std::remove_reference_t<decltype(lexer)>::return_t>;
            const auto visitor = [&lexer](auto&& value){
                return_t retval;

                if constexpr (value.has_action())
                   retval = value.do_action(lexer);

                return retval;
            };

            return std::visit(visitor, m_match);    
        }    
        /**
         * @brief used in fold expression by lexer.
         * 
         * @param other other match result.
         * @return the result which matched more text.
         */
        variant_match_wrapper operator|(variant_match_wrapper other) noexcept {           
            if (length() < other.length())
                m_match.swap(other.m_match);

            return std::move(*this);
        }
    };
} // namespace ctle

#endif // CTLE_LEXER_RESULT