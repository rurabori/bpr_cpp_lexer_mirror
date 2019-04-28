#ifndef CTLE_LEXER_RESULT
#define CTLE_LEXER_RESULT

#include "action.h"
#include "callable.h"

#include <variant>
#include <optional>

namespace ctle {

/**
 * @brief A class representing an empty result.
 *
 * @tparam CharT Used to generate a view of right characters.
 * @tparam ReturnT Used to generate the right optional retval of do_action.
 */
template<typename CharT, typename ReturnT>
struct empty_match_result
{
    /**
     * @brief get a view of the match.
     *
     * @return a std::(w)string_view
     */
    auto to_view() const noexcept { return std::basic_string_view<CharT>{}; }
    /**
     * @brief get length of current match.
     *
     * @return length of the current match.
     */
    auto length() const noexcept { return to_view().length(); }

    std::optional<ReturnT> do_action(auto&) { return std::nullopt; }
};

/**
 * @brief A class encapsulating a result of match operation. Containing the match itself as well as
 * the action.
 *
 * @tparam ResultT the type of result from regex match.
 * @tparam FunctionT the type of function provided.
 */
template<typename ResultT, typename FunctionT>
class match_result
{
    /** @brief the result of a match. */
    ResultT m_result;
    /** @brief a callable. */
    FunctionT m_action;

public:
    /**
     * @brief Construct a new match_result.
     *
     * @param result the result of a regex match.
     * @param action a callable to execute on do_action.
     */
    match_result(ResultT&& result, FunctionT action)
      : m_result{std::move(result)}, m_action{action} {}
    /**
     * @brief get a view of the match.
     *
     * @return a std::(w)string_view
     */
    auto to_view() const { return m_result.to_view(); }
    /**
     * @brief get length of current match.
     *
     * @return length of the current match.
     */
    auto length() const { return to_view().length(); }
    /**
     * @brief executes an action for this rule. While passing a reference to its lexer and the
     * matched text (and captures if any provided).
     *
     * @param lexer a reference to the lexer.
     * @return whatever the action returns.
     */
    auto do_action(auto& lexer) { return m_action(lexer, std::move(m_result)); }
};
// CTAD
template<typename ResultT, typename FunctionT>
match_result(ResultT&&, FunctionT)->match_result<ResultT, FunctionT>;

/**
 * @brief a small wrapper around std::variant.
 *
 * @tparam Args variadic pack passed to std::variant.
 */
template<typename EmptyMatch, typename... Args>
class variant_match_wrapper
{
    using variant_t = std::variant<EmptyMatch, Args...>;
    variant_t m_match; //< holds all possible match results.
public:
    variant_match_wrapper() : m_match{EmptyMatch{}} {}
    variant_match_wrapper(variant_match_wrapper&& other) : m_match{std::move(other.m_match)} {}
    variant_match_wrapper(variant_t&& match) : m_match{std::move(match)} {}

    /**
     * @see match_result::length()
     */
    auto length() const noexcept {
        return std::visit([](auto&& value) { return value.length(); }, m_match);
    }
    auto to_view() const noexcept {
        return std::visit([](auto&& value) { return value.to_view(); }, m_match);
    }
    /**
     * @see match_result::do_action()
     */
    auto do_action(LexerInterface& lexer) noexcept {
        return std::visit([&lexer](auto&& value) { return value.do_action(lexer); }, m_match);
    }
    /**
     * @brief used in fold expression by lexer.
     *
     * @param other other match result.
     * @return the result which matched more text.
     */
    variant_match_wrapper operator|(variant_match_wrapper other) noexcept {
        if (length() < other.length()) m_match.swap(other.m_match);

        return std::move(*this);
    }
};
} // namespace ctle

#endif // CTLE_LEXER_RESULT