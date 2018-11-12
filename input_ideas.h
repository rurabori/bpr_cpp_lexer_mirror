#include <tuple>
#include "ctll/list.hpp"

enum basic_states { a };

template <typename pattern, auto action = nullptr>
struct rule_ {
	using pattern_t = pattern;
	static constexpr auto action_t = action;
};

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