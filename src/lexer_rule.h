#ifndef CTLE_LEXER_RULE
#define CTLE_LEXER_RULE

#include "ctll/fixed_string.hpp"

#include "utils.h"
#include "states.h"
#include "pattern_info.h"
#include "ctle_concepts.h"
#include "lexer_result.h"

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
		 * @brief return type of rule.
		 * 
		 * @tparam Ty type of input iterator.
		 */
		template<typename Ty>
		using return_t = decltype(pattern_t::match_relaxed(std::declval<Ty>(), std::declval<Ty>()));
		/**
		 * @brief prints the regex pattern.
		 */
		static void print() noexcept { printf("%s\n", Pattern.begin()); }
		/**
		 * @brief checks if rule has provided an action.
		 */
		static constexpr bool has_action() noexcept { return !std::is_same_v<Action, std::nullptr_t>; }
		/**
		 * @brief checks whether rule is valid in said state.
		 */
		static constexpr bool is_valid_in_state(auto state) { return contains<States>(all_states) || contains<States>(state); }
		/**
		 * @brief executes an action provided by rule.
		 * 
		 * @param lexer the lexer which matched the rule.
		 * @param result the result which was matched (std::variant<all_result_types>)
		 * @return std::optional<Lexer::return_t> either empty optional if action's return is void or value returned by action.
		 */
		template <typename Lexer, typename = std::enable_if_t<has_action()>>
		static CTRE_FORCE_INLINE auto do_action(Lexer& lexer, typename Lexer::variant_return_t&& result) {		
			// get return type for this Lexer.
			using f_return_t = decltype(get_return_t_by_lexer<Pattern, Lexer>());
			using optional_t = std::optional<typename Lexer::return_t>;
			// can call directly as this rule is the longest, hence the one assigning the value to variant.
			auto&& res = std::get<f_return_t>(result);
			optional_t retval;
			// disambiguate between void/non void return types.
			using action_return_t = decltype(do_action_impl(lexer, res));
			if constexpr (std::is_same_v<action_return_t, void>)
				do_action_impl(lexer, res);
			else
				retval.emplace(do_action_impl(lexer, res));

			return retval;
		}
		/**
		 * @brief tries to match text by rule.
		 * 
		 * @param begin begin iterator.
		 * @param end end iterator.
		 * 
		 * @return lexer_rule matched type and action pointer (if any).
		 */
		template <typename Lexer, typename Ibegin, typename Iend>
		static constexpr CTRE_FORCE_INLINE auto match(Ibegin begin, Iend end) {
			using signature_t = typename Lexer::action_signature_t;
			using var_t = typename Lexer::variant_return_t;
			
			signature_t sig = nullptr;

			// this has to be in constexpr if because if !has_action() do_action is not compiled.
			if constexpr (has_action()) sig = &do_action<Lexer>;

			return lexer_result<var_t, signature_t>{pattern_t::match_relaxed(begin, end), sig};
		}

	private:
		// glorified std::apply
		template<typename Result, size_t... idx>
		static CTRE_FORCE_INLINE auto do_action_impl(LexerInterface& lexer, Result&& result, std::index_sequence<idx...>) {
			return Action()(lexer, result.template get<idx>()...);	
		}
		// glorified std::apply
		template<typename Result>
		static CTRE_FORCE_INLINE auto do_action_impl(LexerInterface& lexer, Result&& result) {
			return do_action_impl(lexer, result, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Result>>>());	
		}
	};
} // namespace ctle
#endif //CTLE_LEXER_RULE