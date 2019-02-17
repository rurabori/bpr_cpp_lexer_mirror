#ifndef CTLE_LEXER_RULE
#define CTLE_LEXER_RULE

#include "ctll/fixed_string.hpp"

#include "utils.h"
#include "states.h"
#include "ctle_concepts.h"
#include "match_result.h"

#include <array>
#include <algorithm>
#include <optional>
#include <variant>

namespace ctle {
	/**
	 * @brief holds one rule.
	 * 
	 * @tparam Pattern a string representing the rule.
	 * @tparam Action a class with overloaded operator() or std::nullptr_t if no action is needed. The action will be executed after this rule is matched.
	 * @tparam States an std::array of all states this rule is valid in.
	 */
	template <ctll::basic_fixed_string Pattern, class Action = std::nullptr_t, std::array States = std::array{state_initial}>
	class lexer_rule
	{
	public:
		using char_t = typename decltype(Pattern)::char_type;
		using pattern_t = decltype(make_re<Pattern>());
		/**
		 * @brief prints the regex pattern.
		 */
		static void print() noexcept { printf("%s\n", Pattern.begin()); }
		/**
		 * @brief checks whether rule is valid in said state.
		 */
		static constexpr bool is_valid_in_state(auto state) { return contains<States>(all_states) || contains<States>(state); }
		/**
		 * @brief tries to match text by rule.
		 * 
		 * @param begin begin iterator.
		 * @param end end iterator.
		 * 
		 * @return lexer_rule matched type and action pointer (if any).
		 */
		template <typename Ibegin, typename Iend>
		static constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) {
			using match_t = decltype(pattern_t::match_relaxed(begin, end));
			return match_result<match_t, Action>{pattern_t::match_relaxed(begin, end)};
		}
		/**
		 * @brief return type of rule.
		 * 
		 * @tparam Ty type of input iterator.
		 */
		template<typename IBegin, typename IEnd>
		using return_t = decltype(match(std::declval<IBegin>(), std::declval<IEnd>()));
		
	};
} // namespace ctle
#endif //CTLE_LEXER_RULE