#include "action.h"

#include <catch2.h>

TEST_CASE("Callable creation.", "[ctle::callable]") {
    class foo
    {
    public:
        constexpr foo() {}
        void operator()() const noexcept {}
    };

    constexpr ctle::callable from_lambda{[] {}};
    constexpr ctle::callable from_struct{foo{}};

    from_lambda();
    from_struct();
}

TEST_CASE("Check if return detection works.", "[action]") {
    SECTION("Returning action.") {
        constexpr ctle::action<[] { return 1; }, int> action{};
        REQUIRE(action().has_value());
        REQUIRE(action().value() == 1);
    }
    SECTION("Non returning action.") {
        REQUIRE_FALSE(ctle::action<[] {}, int>{}().has_value());
    }

    SECTION("Optionally returning action.") {
        using optionally_returning_t = ctle::action<[](bool returns) -> std::optional<int> {
            if (returns) return 12;
            return std::nullopt;
        },
                                                    int>;
        constexpr optionally_returning_t action;

        REQUIRE(action(true).has_value());
        REQUIRE(action(true).value() == 12);
        REQUIRE_FALSE(action(false).has_value());
    }
}
