#ifndef CTLE_ACTION
#define CTLE_ACTION
#include "ctre.hpp"
#include "ctll/fixed_string.hpp"
#include "lexer_re.h"
#include "states.h"


#include <array>
#include <optional>

namespace ctle{
	/**
	 * @brief 	a wrapper for action objects which distinguishes those which return and those which don't.
	 * 			if an action wishes to return conditionaly return std::optional<ReturnType> from that action
	 * 			and if it's empty, the caller won't return, if always returns, return ReturnType, if doesn't return,
	 * 			specify return type as void.
	 * 
	 * @tparam Action an action class (whathever defines static method execute() and returns accordingly).
	 * @tparam ReturnType desired return_type if any.
	 */
	template<typename Action, typename ReturnType>
    class action {
        using optional_return_t = std::optional<ReturnType>;
    public:
        template<typename... Args>
        static optional_return_t execute(Args&&... args) {
            using action_return_t = decltype(Action::execute(std::forward<Args>(args)...));
			// distinguish between returning and non_returning ones.
			if constexpr (!std::is_same_v<action_return_t, void>) {
				// check if return type is correct.
                static_assert(  std::is_same_v<action_return_t, ReturnType> || 
                                std::is_same_v<action_return_t, optional_return_t>, 
                                "Action has a wrong return type."
                );
                return optional_return_t{Action::execute(std::forward<Args>(args)...)};
            } else {
                Action::execute(std::forward<Args>(args)...);
                return optional_return_t{};
            }
        }
    };
	/**
	 * @brief a helper class, which is used to get a default action when the specified one is not null.
	 * 
	 */
	class get_default {
		// implementation
		template<typename Ty, typename Default>
		static constexpr auto get_default_impl() {
			if constexpr (std::is_same_v<Ty, std::nullptr_t>)
				return Default();
			else
				return Ty();
		}
	public:
		/**
		 * @brief returns Ty if Ty is not std::nullptr_t else returns Default
		 * 
		 * @tparam Ty 
		 * @tparam Default 
		 */
		template<typename Ty, typename Default>
		using type_t = decltype(get_default_impl<Ty, Default>());
	};
}
#endif //CTLE_ACTION