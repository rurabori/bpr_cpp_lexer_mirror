#ifndef CTLE_RULE_FILTERS
#define CTLE_RULE_FILTERS

#include "utils.h"
namespace ctle {
/**
 * @brief filters rules that are active in this state.
 *
 * @tparam State a ctle::state type.
 * @tparam initial_state the state that represents initial state.
 */
template<typename State, auto initial_state>
class state_filter
{
    template<typename Rule>
    static constexpr bool decide() {
        return Rule::is_valid_in_state(State::identifier())
               || (!State::exclusive() && Rule::is_valid_in_state(initial_state));
    }

    template<typename Rule>
    static constexpr auto filter_one() {
        if constexpr (decide<Rule>())
            return ctll::list<Rule>{};
        else
            return ctll::list<>{};
    }

    template<typename... Rule>
    static constexpr auto filter_impl(ctll::list<Rule...>) {
        return (ctll::list<>() + ... + filter_one<Rule>());
    }

public:
    /**
     * @brief constructs a filtered list.
     *
     * @tparam Rules The list of rules to filter.
     */
    template<typename Rules>
    using filtered_t = decltype(filter_impl(Rules{}));
};
} // namespace ctle
#endif // CTLE_RULE_FILTERS