#ifndef CTLE_UTILS
#define CTLE_UTILS
#include "ctre.hpp"
#include "ctll/fixed_string.hpp"
#include "lexer_re.h"
#include "states.h"
#include <array>
namespace ctle{
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

	template<typename first, typename second>
	constexpr auto filter_elem() {
		if constexpr (!std::is_same_v<first, second>) {
			return ctll::list<second>{};
		} else {
			return ctll::list<>{};
		}
	}

	template<typename type, typename... others>
	constexpr auto remove_type() {
		return (ctll::list<>{} + ... + filter_elem<type, others>());
	}

	template <typename... Results, typename... Types, typename Type>
	constexpr auto operator+(std::pair<ctll::list<Results...>, ctll::list<Types...>> input, ctll::list<Type>) {
		if constexpr (!sizeof...(Types))
			return input;

		using front = decltype(ctll::front(input.second));
		if constexpr (std::is_same_v<front, Type>) {
			return std::pair<ctll::list<Type, Results...>, decltype(remove_type<Type, Types...>())>{};
		} else {
			return input;		
		}
	}

	template<typename... types>
	constexpr auto remove_duplicates(ctll::list<types...> a) {
		return (std::pair<ctll::list<>, decltype(a)>{} + ... + ctll::list<types>{}).first;
	}

	template<template <typename> typename output, typename... Types>
	constexpr auto assign_unique_impl(ctll::list<Types...>) {
		return output<Types...>{};
	}


	template<template <typename> typename output, typename... Types>
	constexpr auto assign_unique(ctll::list<Types...>) {
		return assign_unique_impl<output>(remove_duplicates(ctll::list<Types...>{}));
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

	template<std::array all_states, typename rule, size_t... idx>
	constexpr bool filter_rule_helper(std::index_sequence<idx...>) {
		return ((!all_states[idx].exclusive && rule::is_valid_in_state(all_states[idx].value)) || ... || rule::is_valid_in_state(state_initial));
	}

	template <typename... Start, typename Next>
	constexpr auto operator+(ctll::list<Start...> start, Next next) {
		return ctll::concat(start, next);
	}

	template<std::array all_states, typename rule>
	constexpr auto filter_rule()  {
		if constexpr (filter_rule_helper<all_states, rule>(std::make_index_sequence<all_states.size()>())) {
			return ctll::list<rule>();
		} else {
			return ctll::list<>();	
		}
	}

	template<std::array all_states, typename... rules>
	constexpr auto filter_initial(ctll::list<rules...>)  {
		return (ctll::list<>() + ... + filter_rule<all_states, rules>());
	}
	
	template<std::array array>
	constexpr bool contains(auto is_in) {
		for (auto x : array)
			if (static_cast<int>(x) == static_cast<int>(is_in))
				return true;

		return false;
	}
}
#endif //CTLE_UTILS