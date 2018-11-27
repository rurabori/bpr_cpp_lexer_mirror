#include <tuple>
#include "ctll/list.hpp"
#include "ctll/fixed_string.hpp"
#include <functional>

constexpr auto identity() noexcept { return true; }

template <ctll::basic_fixed_string Pattern,auto Token, auto Action = nullptr>
struct lexer_rule
{
public:
	static constexpr auto pattern = Pattern;
	static constexpr auto token = Token;
	static constexpr auto action = Action;
};

template <typename Tokens, typename rules = ctll::list<>>
struct lexer
{};