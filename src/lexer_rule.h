#ifndef CTLE_LEXER_RULE
#define CTLE_LEXER_RULE

#include "utils.h"
#include "ctll/fixed_string.hpp"
#include "ctll_concepts.h"
#include "states.h"
#include <array>
#include <algorithm>
#include "stack_iterator.h"
#include "file_stack.h"
#include <optional>

template<std::array array>
constexpr bool contains(auto is_in) {
	for (auto x : array)
		if (static_cast<int>(x) == static_cast<int>(is_in))
			return true;

	return false;
}



template <ctll::basic_fixed_string Pattern, class Action = std::nullptr_t, std::array States = std::array{state_initial}>
struct lexer_rule
{
public:
	using char_type = typename decltype(Pattern)::char_type;
	//using action = Action;
	static constexpr auto pattern = make_pattern<Pattern>();
	
	using pattern_return = decltype(pattern.match_relaxed(std::declval<ctle::stack_iterator<ctle::basic_file_stack<ctle::basic_file<char>>>>(), std::declval<ctle::stack_iterator<ctle::basic_file_stack<ctle::basic_file<char>>>>()));
	static constexpr const auto& states = States; 
	static inline pattern_return matched;

private:
	template <size_t... idx>
	static CTRE_FORCE_INLINE auto do_action_impl(LexerInterface& lexer, std::index_sequence<idx...>) {
		return Action()(matched.template get<idx>()..., lexer);	
	}
public:

	static constexpr bool has_action() noexcept { return !std::is_same_v<Action, std::nullptr_t>; }

	static constexpr bool is_valid_in_state(auto state, typename std::enable_if_t<contains<States>(all_states)>* = 0) {
		return true;
	}

	template <typename = std::enable_if_t<!std::is_same_v<Action, std::nullptr_t>>>
	static CTRE_FORCE_INLINE auto do_action(LexerInterface& lexer) {
		return do_action_impl(lexer, std::make_index_sequence<std::tuple_size_v<pattern_return>>());
	}
	
	static constexpr bool is_valid_in_state(auto state, typename std::enable_if_t<!contains<States>(all_states)>* = 0) {
		return contains<States>(state);
	}

	template <typename Ibegin, typename Iend>
	static constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) {
		matched = pattern.match_relaxed(begin, end);
		return matched.to_view();
	}
};

#endif //CTLE_LEXER_RULE