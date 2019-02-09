#ifndef CTLE_RULE_FILTERS
#define CTLE_RULE_FILTERS

#include "utils.h"

namespace ctle {
    /**
	 * @brief filters rule that are valid in this state.
	 * 
	 * @tparam state state for which rules are being filtered
	 */
	template<auto state>
	class state_filter {
        /**
		 * @brief filters one rule
		 * 
		 * @tparam rule to be decided.
		 * @return an empty list if rule does not belong, a list with the rule otherwise.
		 */
		template<typename rule>
		static constexpr auto filter_one()  {
			if constexpr (rule::is_valid_in_state(state)) {
				return ctll::list<rule>();
			} else {
				return ctll::list<>();
			}
		}
	public:
		/**
		 * @brief filters a list of rules and returns only those valid in a said state.
		 * 
		 * @tparam rules to be filtered
		 * @return list of rules valid in this state
		 */
		template<typename... rules>
		static constexpr auto filter(ctll::list<rules...>)  {
			return (ctll::list<>() + ... + filter_one<rules>());
		}
		/**
		 * @brief an using just for convenience.
		 * 
		 * @tparam Rules The list of rules to filter.
		 */
		template<typename Rules>
		using filtered_t = decltype(filter(std::declval<Rules>()));
	};

	/**
	 * @brief filters rules for the "initial" state of lexer AKA rules valid in initial states and rules from non-exclsive states.
	 * 
	 * @tparam StateDefinitions an array of all state definitions (we need to know whether they are exclusive).
	 */
	template<std::array StateDefinitions>
	class initial_filter {
		/**
		 * @brief a function to decide whether one rule is valid in the initial state. 
		 * 
		 * @tparam rule the rule to be decided.
		 * @tparam idx index sequence to index all rules in the array.
		 * @return true if valid false else.
		 */
		template<typename Rule, size_t... idx>
		static constexpr bool decide(std::index_sequence<idx...>) {
			return ((!StateDefinitions[idx].exclusive && Rule::is_valid_in_state(StateDefinitions[idx].value)) || ... || Rule::is_valid_in_state(state_initial));
		}
		/**
		 * @brief checks if one rule is valid in an initial state.
		 * 
		 * @tparam rule the rule to be decided.
		 * @return the list with either the rule or empty if not valid.
		 */
		template<typename Rule>
		static constexpr auto filter_one()  {
			if constexpr (decide<Rule>(std::make_index_sequence<StateDefinitions.size()>())) {
				return ctll::list<Rule>();
			} else {
				return ctll::list<>();	
			}
		}
	public:
		/**
		 * @brief filters rules for the "initial" state of lexer AKA rules valid in initial states and rules from non-exclsive states.
		 * 
		 * @tparam rules a list of rules to filter
		 * @return all rules valid in the initial state.
		 */
		template<typename... Rules>
		static constexpr auto filter(ctll::list<Rules...>)  {
			return (ctll::list<>() + ... + filter_one<Rules>());
		}
		/**
		 * @brief an using just for convenience.
		 * 
		 * @tparam Rules The list of rules to filter.
		 */
		template<typename Rules>
		using filtered_t = decltype(filter(std::declval<Rules>()));
	};
} // ctle
#endif //CTLE_RULE_FILTERS