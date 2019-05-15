#ifndef CTLE_LEXER_RESULT
#define CTLE_LEXER_RESULT

#include "action.h"
#include "callable.h"

#include <tuple>
#include <array>
#include <optional>
#include <ctll/utilities.hpp>

namespace ctle {
/**
 * @brief A class representing any kind of match result that can be obtained by calling rule::match
 * in a lexer.
 *
 * @tparam ContainerT A container that can store all the posible retvals of rule::match. This is
 * probably an std::array, that has size equal to maximal number of captures in a rule in a given
 * lexer.
 * @tparam SigT a signature of an action that can consume the container.
 */
template<typename ContainerT, typename SigT>
class match_result
{
    /**
     * @brief the type used to represent matched text. Usually a std::basic_string_view
     *
     */
    using value_type = typename ContainerT::value_type;
    ContainerT m_results;
    /**
     * @brief an action that is executed if any rule has matched.
     *
     */
    SigT m_action{nullptr};
    /**
     * @brief Internal impl of ctor.
     */
    template<typename Ty, size_t... idx>
    CTLL_FORCE_INLINE match_result(Ty&& data, SigT action, std::index_sequence<idx...>)
      : m_results{data.template get<idx>()...}, m_action{action} {}

public:
    /**
     * @brief Construct a new match_result from the result of rule::match (unpacks a tuple into the
     * underlying container).
     *
     * @tparam Ty
     * @param data a tuple-like object returned from rule::match.
     * @param action an action to be executed if this result is chosen as the matched one.
     */
    template<typename Ty>
    match_result(Ty&& data, SigT action)
      : match_result(std::move(data), action, std::make_index_sequence<std::tuple_size_v<Ty>>()) {}
    /**
     * @brief Ctor for an empty result. The action shouldn't be executed if this is the case.
     *
     */
    constexpr match_result(SigT action) : m_results{value_type{}}, m_action{action} {}
    /**
     * @brief executes the stored action.
     *
     * @param lexer a reference to the lexer (forwarded to action itself).
     * @return the return value of executed action.
     */
    CTLL_FORCE_INLINE auto do_action(auto& lexer) {
        using retval_t = decltype(m_action(lexer, std::move(m_results)));
        if (m_action == nullptr) return retval_t{};

        return m_action(lexer, std::move(m_results));
    }
    /**
     * @brief Converts the result into a std::basic_string_view.
     *
     * @return auto A view of this result.
     */
    auto to_view() const noexcept { return m_results[0]; }
    /**
     * @brief returns the length of this result.
     *
     * @return length of this result.
     */
    auto length() const noexcept { return to_view().length(); }
    /**
     * @brief used in a fold expresion.
     *
     * @param other the result to compare to.
     * @return the one that is longer.
     */
    match_result operator|(match_result other) {
        return (length() < other.length()) ? other : *this;
    }
};
} // namespace ctle

#endif // CTLE_LEXER_RESULT