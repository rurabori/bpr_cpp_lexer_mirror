#ifndef CTLE_UTILS
#define CTLE_UTILS
#include "ctre.hpp"
#include "ctll/fixed_string.hpp"
#include "lexer_re.h"
#include "states.h"
#include "callable.h"

#include <array>
#include <optional>

namespace ctle{
	/**
	 * @brief impl of list_to_type_t
	 */
	template <template <typename> typename OutputType, typename...Contents>
	constexpr auto list_to_type(ctll::list<Contents...>) {
		return OutputType<Contents...>{};
	}
	/**
	 * @brief assings types from list to some other container which can hold variadic parameters.
	 * 
	 * @tparam OutputType the template template parameter (std::variant, std::tuple etc.).
	 * @tparam List the types to be assigned in a list.
	 */
	template<template <typename> typename OutputType, typename List>
	using list_to_type_t = decltype(list_to_type<OutputType>(std::declval<List>()));
	/**
	 * @brief the operator used in fold expression for filters.
	 * 
	 * @tparam Start the rules that have been matched until this one.
	 * @tparam Next a list with the next rule or an empty list if this rule doesn't belong
	 * @param start ... same as TP 
	 * @param next ... same as TP
	 * @return the list of rules filtered so far.
	 */
	template <typename... Start, typename Next>
	constexpr auto operator+(ctll::list<Start...> start, Next next) {
		return ctll::concat(start, next);
	}
	/**
	 * @brief std::find marked as non-constexpr by my compiler, workaround.
	 * 
	 * @tparam array of elements to be searched.
	 * @param is_in the element to search for
	 * @return true if contains else false.
	 */
	template<std::array array>
	constexpr bool contains(auto is_in) {
		for (auto x : array)
			if (static_cast<int>(x) == static_cast<int>(is_in))
				return true;

		return false;
	}


	namespace detail {	
		// glorified std::apply which allows for varargs before|| THE VARARGS ARE APPLIED BEFORE TUPLE
		template<callable Callable, typename... Args, typename Tuple, size_t... idx>
		static constexpr CTRE_FORCE_INLINE auto apply_tuple(Tuple&& tuple, std::index_sequence<idx...>, Args&&... args) {
			return Callable(std::forward<Args>(args)..., tuple.template get<idx>()...);	
		}
		// yes this only swaps pair right now, not needed for anything else atm.
		template<typename Ty>
		static constexpr void swap(Ty&& a, Ty&& b) {
			auto tmp = std::move(a);
			a.first = b.first;
			a.second = b.second;
			b.first = tmp.first;
			b.second = tmp.second;
		}
	}

	// glorified std::apply which allows for varargs before || THE VARARGS ARE APPLIED BEFORE TUPLE
	template<callable Callable, typename... Args, typename Tuple>
	static constexpr CTRE_FORCE_INLINE auto apply_tuple(Tuple&& tuple, Args&&... args) {
		constexpr auto tuple_size = std::tuple_size_v<std::remove_reference_t<Tuple>>;
		return detail::apply_tuple<Callable>(std::move(tuple), std::make_index_sequence<tuple_size>(), std::forward<Args>(args)...);	
	}
	
	// just a bubble sort, not expecting long sequences of states.
	template<typename Indexable, typename Comparator>
	static constexpr auto sort(Indexable& to_sort, Comparator&& compare) {
		for (auto i = 0; i < to_sort.size(); ++i) {
			for (auto j = 0; j < to_sort.size() - i - 1; ++j) {
				if (!compare(to_sort[j], to_sort[j+1]))
					detail::swap(to_sort[j], to_sort[j+1]);
			}
		}
	}

	template<ctll::basic_fixed_string... input>
	static constexpr auto concat() {
		static_assert(sizeof...(input), "Concatenating no strings makes no sense.");
		using char_t = decltype((input[0], ...));
		
		char_t buffer[(0 + ... + input.size())] = {};
		char_t* dest = &buffer[0];

		const auto loop_body = [&dest](const auto& str) { for (auto c : str) *(dest++) = c; };
		
		(loop_body(input), ...);

		return ctll::basic_fixed_string(buffer);
	}

	template<typename... Ty>
	class derives_from : public Ty...
	{};
}
#endif //CTLE_UTILS