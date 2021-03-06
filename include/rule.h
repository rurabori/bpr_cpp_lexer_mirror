#ifndef CTLE_RULE
#define CTLE_RULE

#include "utils.h"
#include "callable.h"
#include "regex.h"
#include "container.h"

#include <ctll/fixed_string.hpp>
#include <array>

namespace ctle {
/**
 * @brief holds one rule.
 *
 * @tparam Pattern a string representing the rule.
 * @tparam Action a callable. The action will be executed after this rule is matched.
 * @tparam States an std::array of all states this rule is valid in.
 */
template<ctll::fixed_string Pattern, callable Action = empty_callable,
         std::array States = std::array{state_initial}>
class rule
{
public:
    static constexpr auto action = Action;
    using pattern_t = decltype(make_re<Pattern>());
    /**
     * @brief checks whether rule is valid in said state.
     */
    static constexpr bool is_valid_in_state(auto state) {
        return contains<States>(all_states) || contains<States>(state);
    }
    /**
     * @brief tries to match text by rule.
     *
     * @param begin begin iterator.
     * @param end end iterator.
     *
     * @return a match_result, containing the matched text (and captures), and an action (if any).
     */
    template<typename Ibegin, typename Iend>
    static constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) noexcept {
        return pattern_t::match_relaxed(begin, end);
    }
};

/**
 * @brief An using to better specify the container in the code.
 *
 * @tparam ReturnT the return value of the rules.
 * @tparam Rules a ctll::list of ctle::rule-s.
 */
template<typename ReturnT, typename Rules>
using rules = container<ReturnT, Rules>;

} // namespace ctle
#endif // CTLE_RULE