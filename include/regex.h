#ifndef CTLE_REGEX
#define CTLE_REGEX
#include <ctre.hpp>

namespace ctle {
/**
 * @brief copied from CTRE, just doesn't assert end of string as we need to match only on start on
 * string(or rather search).
 */
template<typename Iterator, typename EndIterator, typename Pattern>
constexpr inline auto match_start(const Iterator begin, const EndIterator end,
                                  Pattern pattern) noexcept {
    using namespace ctre;
    using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
    return evaluate(begin, begin, end, return_type{},
                    ctll::list<start_mark, Pattern, end_mark, accept>());
}

template<typename RE>
struct regular_expression : public ctre::regular_expression<RE>
{
    constexpr CTRE_FORCE_INLINE regular_expression() noexcept : ctre::regular_expression<RE>(){};
    constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept : ctre::regular_expression<RE>(){};

    template<typename IteratorBegin, typename IteratorEnd>
    constexpr CTRE_FORCE_INLINE static auto match_relaxed(IteratorBegin begin,
                                                          IteratorEnd   end) noexcept {
        return match_start(begin, end, RE());
    }
};

/**
 * @brief creates regular_expression.
 *
 * @tparam input pattern from which re is created.
 */
template<ctll::fixed_string input>
CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto make_re() noexcept {
    constexpr auto _input = input; // workaround for GCC 9 bug 88092
    using tmp = typename ctll::parser<ctre::pcre, _input,
                                      ctre::pcre_actions>::template output<ctre::pcre_context<>>;
    static_assert(tmp(), "Regular Expression contains syntax error.");
    using re = decltype(front(typename tmp::output_type::stack_type()));
    return ctle::regular_expression(re());
}

template<typename RE>
regular_expression(RE)->regular_expression<RE>;

} // namespace ctle

#endif // CTLE_REGEX