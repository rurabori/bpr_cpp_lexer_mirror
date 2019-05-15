#include "rule_filters.h"
#include "rule.h"
#include "states.h"

#include <catch2.h>

template<typename State>
using filter_t = ctle::state_filter<State, ctle::state_initial>;

TEST_CASE("Test filtering of rules.", "[ctle::state_filter]") {
    enum state_id
    {
        inclusive = ctle::state_reserved,
        exclusive
    };

    using state_inclusive = ctle::state<state_id::inclusive>;
    using state_exclusive = ctle::state<state_id::exclusive, true>;

    using initial_rule = ctle::rule<"">;
    using inclusive_rule = ctle::rule<"", ctle::empty_callable, std::array{state_id::inclusive}>;
    using exclusive_rule = ctle::rule<"", ctle::empty_callable, std::array{state_id::exclusive}>;
    using all_rule = ctle::rule<"", ctle::empty_callable, std::array{ctle::all_states}>;

    using rule_list = ctll::list<initial_rule, inclusive_rule, exclusive_rule, all_rule>;

    STATIC_REQUIRE(std::is_same_v<ctll::list<initial_rule, inclusive_rule, all_rule>,
                                  filter_t<state_inclusive>::template filtered_t<rule_list>>);

    STATIC_REQUIRE(std::is_same_v<ctll::list<exclusive_rule, all_rule>,
                                  filter_t<state_exclusive>::template filtered_t<rule_list>>);
}
