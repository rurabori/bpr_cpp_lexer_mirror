#ifndef CTLE_LEXER_RULE
#define CTLE_LEXER_RULE

#include "utils.h"
#include "ctll/fixed_string.hpp"
#include "ctll_concepts.h"

template <typename>
void placeholder() {}

template <ctll::basic_fixed_string Pattern,auto Token = nullptr, class Action = std::nullptr_t>
struct lexer_rule
{
public:
	using token_type = decltype(Token);
	using char_type = typename decltype(Pattern)::char_type;
	using action = Action;

	static constexpr auto pattern = make_pattern<Pattern>();
	static constexpr auto token = Token;
	
	constexpr CTRE_FORCE_INLINE auto get_token(typename std::enable_if_t<std::is_same<decltype(Token), nullptr_t>::value>* = 0) const noexcept { return 0; }
	constexpr CTRE_FORCE_INLINE auto get_token(typename std::enable_if_t<!std::is_same<decltype(Token), nullptr_t>::value>* = 0) const noexcept { return token; }

	template <typename Ibegin, typename Iend>
	constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) {
		auto retval = pattern.match_relaxed(begin, end);
		return retval.to_view();
	}
};
#endif //CTLE_LEXER_RULE