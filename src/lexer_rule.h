#ifndef CTLE_LEXER_RULE
#define CTLE_LEXER_RULE

#include "utils.h"
#include "ctll/fixed_string.hpp"

template <ctll::basic_fixed_string Pattern,auto Token = nullptr, auto Action = nullptr>
struct lexer_rule
{
public:
	using token_type = decltype(Token);
	using char_type = typename decltype(Pattern)::char_type;
	static constexpr auto pattern = make_pattern<Pattern/*add_capture<Pattern>()*/>();
	static constexpr auto token = Token;
	static constexpr auto action = Action;

	constexpr CTRE_FORCE_INLINE auto get_token(typename std::enable_if_t<std::is_same<decltype(Token), nullptr_t>::value>* = 0) const noexcept { return 0; }
	constexpr CTRE_FORCE_INLINE auto get_token(typename std::enable_if_t<!std::is_same<decltype(Token), nullptr_t>::value>* = 0) const noexcept { return token; }

	template <typename Ibegin, typename Iend>
	constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) {
		auto retval = pattern.match_relaxed(begin, end);
		if (retval) {
			if constexpr (action) {
				return action(retval);
			}
		} 

		return retval.to_view();
	}
};
#endif //CTLE_LEXER_RULE