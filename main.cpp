#include "input_ideas.h"
#include <bits/stdc++.h>
#include "ctre.hpp"
#include "ctll/list.hpp"

enum tokens {
	name,
	age,
	unknown
};

constexpr bool fun1(int) { return false; }

constexpr bool fun2() { return true; }


int main(int argc, char const *argv[])
{
	 // the tokens might not be needed as param, will consider tho.
	 lexer<
	 	tokens,
	 	ctll::list<
	 			lexer_rule<"anicka|janicko|marienka|borisko|sasenka", tokens::name>,
	 			lexer_rule<"[0-9]+yrs", tokens::age, fun2>,
	 			lexer_rule<".*", tokens::unknown>
	 		>
	 > a;

	std::cout << lexer_rule<"Hello, world.", tokens::name, fun2>().pattern.begin() << std::endl;


	return 0;
}
