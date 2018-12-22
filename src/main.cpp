#include "lexer.h"
#include <bits/stdc++.h>
#include "ctll/list.hpp"
#include "file.h"
#include <charconv>
#include "file_stack.h"
#include "ctll_concepts.h"
#include "wise_enum.h"
#include "states.h"

WISE_ENUM(tokens, age, name, unknown)
WISE_ENUM(states, (string, state_reserved), CLASS)


class plus_operation {
public:
	tokens operator()(LexerInterface& b, std::string_view all, std::string_view first_num, std::string_view second_num) {
		std::cout << all;
		std::cout << " || = " << std::atoi(std::string(first_num.begin(), first_num.end()).c_str()) + std::atoi(std::string(second_num.begin(), second_num.end()).c_str()) << std::endl;
		b.set_state(states::CLASS);
		return tokens::age; 
	}
};

class echo {
public:
	template <typename... pack>
	void operator()(LexerInterface& b, pack... all) {
		(printf("ECHO : %s", all.to_view().data()), ...);
		//return tokens::age;
	}
};

int main(int argc, char const *argv[])
{


	lexer< tokens, 
				ctll::list<
					lexer_rule<"int|double|rosticek|marienka|borisko|sasenka">,
					lexer_rule<"[0-9]+(yrs)", echo, std::array{states::CLASS}>,
					lexer_rule<"asd", echo, std::array{states::CLASS}>,
					lexer_rule<"def">,
					lexer_rule<"([0-9]+)\\s*\\+\\s*([0-9]+)", plus_operation>,
					lexer_rule<"[ \t\r\n]+",nullptr_t, std::array{all_states}>
				>,
				states,
					std::array{state<states>{states::string, true}, state<states>{states::CLASS}}
				> v22;

	v22.add_file(argv[1]);
	
	while (true) if (auto [matched, token] = v22.lex(); token.has_value() && token.value() == tokens::unknown) break;

	return 0;
}
