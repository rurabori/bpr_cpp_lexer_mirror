#include "action.h"
#include "catch2.h"

enum ret {
    o1,
    o2
};

struct act_void {
    static void execute() { }
};

struct act_returning {
    static ret execute() { return ret::o1;}
};

struct act_opt_returning {
    static std::optional<ret> execute(bool should_return) { return (should_return) ? ret::o1 : std::optional<ret>{};}
};

template<typename Action>
using action_prototype = ctle::action<Action, ret>;

TEST_CASE("Check if return detection works (also check if defaulting works, checked during compilation).", "[action]") {
    
    REQUIRE(!action_prototype<act_void>::execute().has_value());
    REQUIRE(action_prototype<act_returning>::execute().has_value());
    REQUIRE(!action_prototype<act_opt_returning>::execute(false).has_value());
    REQUIRE(action_prototype<act_opt_returning>::execute(true).has_value());

    // check the defaulting
    static_assert(std::is_same_v<act_void, typename ctle::get_default::type_t<std::nullptr_t, act_void>>);
    static_assert(std::is_same_v<act_returning, typename ctle::get_default::type_t<act_returning, act_void>>);
}
