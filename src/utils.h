#ifndef CTLE_UTILS
#define CTLE_UTILS
#include "ctre.hpp"
#include "ctll/fixed_string.hpp"
#include "lexer_re.h"
#include "states.h"
#include <array>
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
}
#endif //CTLE_UTILS