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

template <typename Lexer, typename FunctionSignature>
struct lexer_result {
	using parent_type = Lexer;
	std::string_view match{""};
	FunctionSignature action{nullptr};

	lexer_result operator|(lexer_result other) {
		return (match.length() < other.match.length()) ? other : *this;
	}
};

template <ctll::basic_fixed_string Pattern, class Action = std::nullptr_t, std::array States = std::array{state_initial}>
struct lexer_rule
{
public:
	using char_type = typename decltype(Pattern)::char_type;
	static constexpr auto pattern = make_pattern<Pattern>();
	
	using pattern_return = decltype(pattern.match_relaxed(std::declval<ctle::stack_iterator<ctle::basic_file_stack<ctle::basic_file<char>>>>(), std::declval<ctle::stack_iterator<ctle::basic_file_stack<ctle::basic_file<char>>>>()));
	static constexpr const auto& states = States; 
	static inline pattern_return matched;

private:
	template <size_t... idx>
	static CTRE_FORCE_INLINE auto do_action_impl(LexerInterface& lexer, std::index_sequence<idx...>) {
		return Action()(lexer, matched.template get<idx>()...);	
	}
public:

	static void print() noexcept { printf("%s\n", Pattern.begin()); }

	static constexpr bool has_action() noexcept { return !std::is_same_v<Action, std::nullptr_t>; }

	static constexpr bool is_valid_in_state(auto state, typename std::enable_if_t<contains<States>(all_states)>* = 0) {
		return true;
	}

	static constexpr bool is_valid_in_state(auto state, typename std::enable_if_t<!contains<States>(all_states)>* = 0) {
		return contains<States>(state);
	}

	template <typename Lexer, typename = std::enable_if_t<!std::is_same_v<Action, std::nullptr_t>>>
	static CTRE_FORCE_INLINE auto do_action(Lexer& lexer) {		
		using return_t = decltype(do_action_impl(lexer, std::make_index_sequence<std::tuple_size_v<pattern_return>>()));
 		if constexpr (std::is_same_v<return_t, void>) {
			do_action_impl(lexer, std::make_index_sequence<std::tuple_size_v<pattern_return>>());
			return std::optional<typename Lexer::return_t>{};
		} else {
			return std::optional{do_action_impl(lexer, std::make_index_sequence<std::tuple_size_v<pattern_return>>())};
		}
	}
	

	

	template <typename Lexer, typename Ibegin, typename Iend>
	static constexpr CTRE_FORCE_INLINE auto match2(Ibegin begin, Iend end) {
		matched = pattern.match_relaxed(begin, end);
		using sig_t = typename Lexer::action_signature;
		sig_t sig = nullptr;

		if constexpr (has_action()) {
			sig = &do_action<Lexer>;
		}
		
		return lexer_result<Lexer, sig_t>{matched.to_view(), sig};
	}

	template <typename Ibegin, typename Iend>
	static constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) {
		matched = pattern.match_relaxed(begin, end);
		return matched.to_view();
	}
};



#endif //CTLE_LEXER_RULE