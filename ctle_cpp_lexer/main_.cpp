#include <bits/stdc++.h>
#include "lexer.h"
#include "ctll/list.hpp"
#include "file.h"
#include <charconv>
#include "file_stack.h"
#include "ctle_concepts.h"
#include "wise_enum.h"
#include "states.h"
#include "utils.h"
using namespace ctle;

WISE_ENUM(tokens, age, name, unknown, eof, no_match)
WISE_ENUM(states, (string, state_reserved), CLASS)


class plus_operation {
public:
	tokens operator()(LexerInterface& b, std::string_view all, std::string_view first_num, std::string_view second_num) const {
		b.smd();
		std::cout << all;
		std::cout << " || = " << std::atoi(std::string(first_num.begin(), first_num.end()).c_str()) + std::atoi(std::string(second_num.begin(), second_num.end()).c_str()) << std::endl;
		return tokens::age; 
	}
};
/*
class include {
public:
	void operator()(LexerInterface& b, std::string_view all, std::string_view file_name) {
		b.line_counter();
		std::cout << file_name << std::endl;
		b.add_file(file_name);
	}
};*/


class lexer_extension
{
public:
	void smd() { std::cout << "yay" << std::endl; }
};

class lineno_c
{
	size_t lineno{0};
public:
	void line_counter() { std::cout << ++lineno << std::endl; }
};

using x = class { int counter; };





constexpr auto include = [](LexerInterface& b, std::string_view all, std::string_view file_name) -> void {
	b.line_counter();
	std::cout << file_name << std::endl;
	b.add_file(file_name);
};

template<callable x>
void test() {

}

int main(int argc, char const *argv[])
{

	constexpr char include_[] = "#include";
	constexpr char capture[] 	= "(.*)";

	using lexer_def = lexer< tokens, 
				ctll::list<
					lexer_rule<"int|double|rosticek|marienka|borisko|sasenka">,
					lexer_rule<"[0-9]+(yrs)", default_actions::echo>,
					lexer_rule<"asd", default_actions::echo>,
					lexer_rule<"#include \"(.*?)\"", include, std::array{ctle::all_states}>,
					lexer_rule<"def">,
					lexer_rule<"([0-9]+)\\s*\\+\\s*([0-9]+)", plus_operation{}>,
					lexer_rule<"[ \t\r\n]+",empty_callable, std::array{all_states}>,
					lexer_rule<"test", default_actions::echo, std::array{states::string}>,
					lexer_rule<"asdasfasf", default_actions::simple_return(tokens::age)>
				>,
				states,
					ctll::list<state<states::string, true>, state<states::CLASS>>,
				derives_from<lexer_extension, lineno_c>
				>;

	lexer_def v22;

	v22.add_file(argv[1]);

	while (true) if (auto token = v22.lex(); token == tokens::no_match || token == tokens::eof) break;

	auto x = &test<[](){}>;

	x();

	std::cout << concat<"test", include_, "  ">().begin() << std::endl;

	return 0;
}
