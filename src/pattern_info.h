#ifndef CTLE_PATTERN_INFO
#define CTLE_PATTERN_INFO

#include "ctll/fixed_string.hpp"

namespace ctle {
	/**
	 * @brief holds info about pattern. This whole class is a workaround for GCC 9 bug 88092.
	 * 
	 * @tparam Pattern pattern from which re is created.
	 */
	template <ctll::basic_fixed_string Pattern>
	struct pattern_info {
		using char_t = decltype(Pattern)::char_type;
		using pattern_t = decltype(make_re<Pattern>());
		/**
		 * @brief return type for re compiled from this pattern. 
		 * 
		 * @tparam Ty iterator type.
		 */
		template<typename Ty>
		using return_t = decltype(pattern_t::match_relaxed(std::declval<Ty>(), std::declval<Ty>()));
	};
	/**
	 * @brief creates pattern_info from pattern -> workaround for GCC 9 bug 88092
	 * 
	 * @tparam Pattern pattern from which info is created.
	 * @return pattern_info
	 */
	template<ctll::basic_fixed_string Pattern>
	constexpr auto make_pattern_info() {
		constexpr auto pattern = Pattern;
		return pattern_info<pattern>{};
	}
	/**
	 * @brief Get the return_t by Lexer
	 * 
	 * @tparam Pattern pattern from which info is created.
	 * @tparam Lexer lexer (only the iterator is of interest).
	 * @return return type of rule wich this lexer's iterator. 
	 */
	template<ctll::basic_fixed_string Pattern, typename Lexer>
	constexpr auto get_return_t_by_lexer() 
		-> typename decltype(make_pattern_info<Pattern>())::template return_t<typename Lexer::file_iterator_t>;
} // namespace ctle

#endif // CTLE_PATTERN_INFO