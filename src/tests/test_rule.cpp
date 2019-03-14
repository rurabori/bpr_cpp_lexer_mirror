#include "catch2.h"

#include "lexer_rule.h"
#include "default_actions.h"


TEST_CASE("Tests for rules without an action.", "[lexer_rule]") {
    
    using rule = ctle::lexer_rule<"a">;
    // test if return type generation works (important for lexer variant).
    using return_type = typename rule::template return_t<typename std::string_view::iterator, typename std::string_view::iterator>;
    // check if rule detection works
    static_assert(!return_type::has_action());

    std::string_view test_string{"a"};
    auto retval = rule::match(test_string.begin(), test_string.end());
    // check if return type is same as the one we generated.
    static_assert(std::is_same_v<decltype(retval), return_type>);
    // check if returns are the same as what got passed, or the input is completely different.
    REQUIRE(retval.to_view() == test_string);
    // try sending wrong
    std::string_view test_string_wrong{"b"};
    retval = rule::match(test_string_wrong.begin(), test_string_wrong.end());
    // match length must be 0
    REQUIRE(retval.length() == 0);
}

TEST_CASE("Tests for rules with an action.", "[lexer_rule]") {
    
    create_action(test, bool, (std::string_view whole_text, std::string_view capture_1) {
        return whole_text == capture_1;
    });

    using rule = ctle::lexer_rule<"(a)", test>;
    // test if return type generation works (important for lexer variant).
    using return_type = typename rule::template return_t<typename std::string_view::iterator, typename std::string_view::iterator>;
    // check if rule detection works
    static_assert(return_type::has_action());

    std::string_view test_string{"a"};
    auto retval = rule::match(test_string.begin(), test_string.end());
    // check if return type is same as the one we generated.
    static_assert(std::is_same_v<decltype(retval), return_type>);
    // check if returns are the same as what got passed, or the input is completely different.
    REQUIRE(retval.to_view() == test_string);
    REQUIRE(retval.template do_action<bool>());
    
    // try sending wrong
    std::string_view test_string_wrong{"b"};
    retval = rule::match(test_string_wrong.begin(), test_string_wrong.end());
    // match length must be 0
    REQUIRE(retval.length() == 0);
}

