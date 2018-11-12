#include "input_ideas.h"
#include <bits/stdc++.h>
#include "ctll/list.hpp"
#include "ctll/fixed_string.hpp"


constexpr bool die() { return false; }

constexpr bool die2() { return true; }



template <typename Func>
concept bool LexerFunction = requires(std::string_view a) {
	{ Func::operator()(a) };
};


template <typename... Rules> constexpr auto test() {
	return (Rules::action_t() || ... || false);
}

template <ctll::basic_fixed_string pattern,auto token, auto action = die>
struct lexer_rule
{
	static constexpr auto pattern_t = pattern;
	static constexpr auto token_t = token;
	static constexpr auto action_t = action;
};

template <typename Tokens, typename rules = ctll::list<>>
struct lexer
{


};

enum tokens {
	name,
	age,
	unknown
};

int main(int argc, char const *argv[])
{

	 // the tokens might not be needed as param, will consider tho.
	 lexer<
	 	tokens,
	 	ctll::list<
	 			lexer_rule<"anicka|janicko|marienka|borisko|sasenka", tokens::name, die>,
	 			lexer_rule<"[0-9]+yrs", tokens::age, die2>,
	 			lexer_rule<".*", tokens::unknown>,
	 			lexer_rule<"petko", tokens::name>
	 		>
	 > a;

	return 0;
}