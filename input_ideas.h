#include <tuple>
#include "ctll/list.hpp"
#include "ctll/fixed_string.hpp"

constexpr auto identity() noexcept { return true; }

template <ctll::basic_fixed_string Pattern,auto Token, auto Action = identity>
struct lexer_rule
{
	static constexpr auto pattern = Pattern;
	static constexpr auto token = Token;
	static constexpr auto action = Action;
};

template <typename Func>
concept bool LexerFunction = requires(std::string_view a) {
	{ Func::operator()(a) };
};

template <typename... Rules> constexpr auto test() {
	return (Rules::action_t() || ... || false);
}


template <typename Tokens, typename rules = ctll::list<>>
struct lexer
{};



/*
template <typename TokenEnum, typename rules = ctll::list<>, typename StateEnum = basic_states>
class lexer {
	using token_t = TokenEnum;
	using state_t = StateEnum;

	template <size_t Pos> struct placeholder{};

	template <size_t... indices>
	static constexpr auto test(std::index_sequence<indices...>) noexcept {

	}


};
*/