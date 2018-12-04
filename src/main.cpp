#include "lexer.h"
#include <bits/stdc++.h>
#include "ctll/list.hpp"
#include "file.h"

enum tokens {
	name,
	age,
	unknown
};

std::string_view fun1(std::string_view a) { std::cout << "Name : " << a << std::endl; return a; }

std::string_view fun2(std::string_view a) {
	 std::cout << "Years : " << a << std::endl;
	 return a;
}



int main(int argc, char const *argv[])
{
	 lexer<
	 	tokens,
	 	ctll::list<
	 			lexer_rule<"eva|anicka|janicko|marienka|borisko|sasenka", tokens::name, fun1>,
	 			lexer_rule<"[0-9]+yrs", tokens::age, fun2>,
	 			lexer_rule<"asd">,
	 			lexer_rule<"def">,
	 			lexer_rule<"[ \t\r\n]+">
	 		>
	 > a;

	ctle::basic_file<char> myfile(argv[1]);
	auto [res, token] = a.match(myfile.begin(), myfile.end());

	return 0;
}
