#include "utils.h"
#include "rule.h"

#include <catch2.h>
#include <algorithm>

TEST_CASE("Test finding maximal number of captures.", "[utils::capture::max]") {
    constexpr auto wrap = [](auto rules) { return ctle::capture::max<char*, char*>(rules); };

    using single_capture = ctll::list<ctle::rule<"a">>;
    STATIC_REQUIRE(wrap(single_capture{}) == 1);

    using multiple_captures = ctll::list<ctle::rule<"a(b)">, ctle::rule<"(a),(b)">>;
    STATIC_REQUIRE(wrap(ctll::concat(single_capture{}, multiple_captures{})) == 3);
}

TEST_CASE("Test concatenation of fixed strings.", "[utils::concat]") {
    constexpr ctll::fixed_string expected{"first_second_third"};
    constexpr auto               result = ctle::concat<"first", "_second", "_third">();

    STATIC_REQUIRE(std::is_same_v<decltype(expected), decltype(result)>);
    using namespace ctle; // for equality operator
    STATIC_REQUIRE(expected == result);
}
