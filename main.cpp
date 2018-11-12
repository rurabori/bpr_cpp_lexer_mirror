#include "input_ideas.h"
#include <bits/stdc++.h>
#include "ctll/list.hpp"
#include "ctll/fixed_string.hpp"

enum tokens {
	name,
	age,
	unknown
};

constexpr bool fun1() { return false; }

constexpr bool fun2() { return true; }


int main(int argc, char const *argv[])
{

	 // the tokens might not be needed as param, will consider tho.
	 lexer<
	 	tokens,
	 	ctll::list<
	 			lexer_rule<"anicka|janicko|marienka|borisko|sasenka", tokens::name, fun1>,
	 			lexer_rule<"[0-9]+yrs", tokens::age, fun2>,
	 			lexer_rule<".*", tokens::unknown>,
	 			lexer_rule<"petko", tokens::name>
	 		>
	 > a;

	return 0;
}