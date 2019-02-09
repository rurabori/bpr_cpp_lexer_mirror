#include "type_filter.h"
#include "rule_filters.h"
#include "lexer_rule.h"
#include "states.h"

#include <variant>

int main() {
    // test filtering unique types 
    {
        using expected_result_t = std::variant<char,int,double,std::string>;
        using input_t = ctll::list<char, char, int, int, double, std::string, int, char, int, double, std::string>;
        using result_t = ctle::unique_from_list_to_type_t<std::variant, input_t>;

        static_assert(std::is_same_v<ctle::unique_from_list_to_type_t<std::variant, input_t>, result_t>);
    }

    // test filtering rules by state
    {
        enum estates {
            state1 = ctle::state_reserved, // all user defined enums must start here, lexer checks this at compile time.
            state2
        };
        constexpr std::array state_definitions{ctle::state<estates>{state1, true}, ctle::state<estates>{state2, false}};
        
        
        using rule_state_initial = ctle::lexer_rule<"si">;
        using rule_state1 = ctle::lexer_rule<"s1", std::nullopt_t, std::array{state1}>;
        using rule_state_e_ne = ctle::lexer_rule<"sene", std::nullopt_t, std::array{state1, state2}>;
        using rule_state2 = ctle::lexer_rule<"s2", std::nullopt_t, std::array{state2}>;
        using rule_state_all = ctle::lexer_rule<"sa", std::nullopt_t, std::array{ctle::all_states}>;
        

        using rules_t = ctll::list <
            rule_state_initial,
            rule_state1,
            rule_state2,
            rule_state_e_ne,
            rule_state_all
        >;
        // test initial_filter
        {
            using filter = ctle::initial_filter<state_definitions>;
            using expected_result_t = ctll::list<
                rule_state_initial,
                rule_state2,
                rule_state_e_ne,
                rule_state_all
            >;
            using result_t = filter::template filtered_t<rules_t>;
            static_assert(std::is_same_v<expected_result_t, result_t>);
        }
        // test state_filter
        {
            using filter = ctle::state_filter<estates::state1>;
            using expected_result_t = ctll::list<
                rule_state1,
                rule_state_e_ne,
                rule_state_all
            >;
            using result_t = filter::template filtered_t<rules_t>;
            static_assert(std::is_same_v<expected_result_t, result_t>);
        }

    }

}
