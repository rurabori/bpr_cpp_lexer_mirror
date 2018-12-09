#include "lexer.h"
#include <bits/stdc++.h>
#include "ctll/list.hpp"
#include "file.h"
#include <charconv>
#include "file_stack.h"
#include "ctll_concepts.h"

enum tokens {
	name,
	age,
	unknown
};

class plus_operation {
public:
	std::string_view operator()(std::string_view a, LexerInterface&b) {
		std::cout << a;
		constexpr auto pat = make_pattern<"([0-9]+)\\s*\\+\\s*([0-9]+)">();
		auto [matched, cap1, cap2] = pat.match(a);
		std::cout << " = " << std::atoi(std::string(cap1.begin(), cap1.end()).c_str()) + std::atoi(std::string(cap2.begin(), cap2.end()).c_str()) << std::endl;
		return std::string_view(); 
	}
};


int main(int argc, char const *argv[])
{
	 lexer<
	 	tokens,
	 	ctll::list<
	 			lexer_rule<"int|double|janicko|marienka|borisko|sasenka", tokens::name>,
	 			lexer_rule<"[0-9]+yrs", tokens::age>,
	 			lexer_rule<"asd">,
	 			lexer_rule<"def">,
				lexer_rule<"[0-9]+\\s*\\+\\s*[0-9]+", tokens::name, plus_operation>,
	 			lexer_rule<"[ \t\r\n]+">
	 		>
	 > a;

	a.add_file(argv[1]);
	a.add_file(argv[1]);
	a.add_file(argv[1]);
	a.add_file(argv[1]);
	a.add_file(argv[1]);
	
	while (true) if (auto [matched, token] = a.lex(); token == tokens::unknown) break;
	return 0;
}
