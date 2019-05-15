#include "rule.h"
#include "default_actions.h"
#include "states.h"

#include <catch2.h>

TEST_CASE("Test relaxed matching.", "[ctle::rule]") {
    using rule = ctle::rule<"a">;

    using return_t = decltype(rule::match(std::declval<char*>(), std::declval<char*>()));
    STATIC_REQUIRE(std::tuple_size_v<return_t> == 1);

    SECTION("Correct input.") {
        std::string_view input = "a_string";

        auto result = rule::match(input.begin(), input.end());
        REQUIRE(result.to_view() == "a");
    }

    SECTION("Correct input.") {
        std::string_view input = "b_string";
        REQUIRE(!rule::match(input.begin(), input.end()));
    }
}

TEST_CASE("Test state recognition.", "[ctle::rule]") {
    constexpr auto state = 42;

    SECTION("Test no states.") {
        using rule = ctle::rule<"">;
        STATIC_REQUIRE(rule::is_valid_in_state(ctle::state_initial));
        STATIC_REQUIRE_FALSE(rule::is_valid_in_state(state));
    }

    SECTION("Test list of states.") {
        using rule = ctle::rule<"", ctle::empty_callable, std::array{state, 123, 12}>;
        STATIC_REQUIRE(rule::is_valid_in_state(state));
        STATIC_REQUIRE_FALSE(rule::is_valid_in_state(ctle::state_initial));
    }

    SECTION("Test wildcard.") {
        using rule = ctle::rule<"", ctle::empty_callable, std::array{ctle::all_states}>;
        STATIC_REQUIRE(rule::is_valid_in_state(42424242));
    }
}
