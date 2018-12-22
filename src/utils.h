#ifndef CTLE_UTILS
#define CTLE_UTILS
#include "ctre.hpp"
#include "ctll/fixed_string.hpp"
#include "lexer_re.h"
#include "states.h"
#include <array>

template <ctll::basic_fixed_string input> CTRE_FLATTEN constexpr CTRE_FORCE_INLINE auto make_pattern() noexcept {
	constexpr auto _input = input; // workaround for GCC 9 bug 88092
	using tmp = typename ctll::parser<ctre::pcre, _input, ctre::pcre_actions>::template output<ctre::pcre_context<>>;
	static_assert(tmp(), "Regular Expression contains syntax error.");
	using re = decltype(front(typename tmp::output_type::stack_type()));
	return ctle::regular_expression(re());
}

template <ctll::basic_fixed_string... strs>
constexpr auto size_of_pack() noexcept {
	return (strs.size() + ... + 0);
}

template <ctll::basic_fixed_string added, ctll::basic_fixed_string... others> 
constexpr void append_to_array(typename decltype(added)::char_type* to_append, size_t& idx) {
	for (int i = 0; i < added.size() ; ++i) {
		to_append[idx + i] = added[i];
	}

	idx += added.size();
	if constexpr (sizeof...(others) != 0)
		append_to_array<others...>(to_append, idx);
}

template <ctll::basic_fixed_string first, ctll::basic_fixed_string... other>
constexpr auto concat_strings() {
	typename decltype(first)::char_type arr[size_of_pack<first, other...>()] = {};
	size_t idx = 0;	
	append_to_array<first,other...>(arr, idx);

	return ctll::basic_fixed_string(arr);	
}

template <ctll::basic_fixed_string pattern> constexpr auto add_capture() {
	return concat_strings<"(", pattern, ")">();
}

template<auto mask, typename rule>
constexpr auto filter_one()  {
	if constexpr (rule::is_valid_in_state(mask)) {
		return ctll::list<rule>();
	} else {
		return ctll::list<>();
	}
}

template<auto mask, typename... rules>
constexpr auto filter(ctll::list<rules...>)  {
	return (ctll::list<>() + ... + filter_one<mask, rules>());
}

template<std::array all, typename rule, size_t... idx>
constexpr bool initial_filter_helper(std::index_sequence<idx...>) {
	return ((!all[idx].exclusive && rule::is_valid_in_state(all[idx].value)) || ... || rule::is_valid_in_state(state_initial));
}

template <typename... Start, typename Next>
constexpr auto operator+(ctll::list<Start...> start, Next next) {
	return ctll::concat(start, next);
}

template<std::array all, typename rule>
constexpr auto initial_filter_one()  {
	if constexpr (initial_filter_helper<all, rule>(std::make_index_sequence<all.size()>())) {
		return ctll::list<rule>();
	} else {
		return ctll::list<>();	
	}
}

template<std::array all, typename... rules>
constexpr auto filter_initial(ctll::list<rules...>)  {
	return (ctll::list<>() + ... + initial_filter_one<all, rules>());
}

#endif //CTLE_UTILS