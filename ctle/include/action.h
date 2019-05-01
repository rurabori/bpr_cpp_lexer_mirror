#ifndef CTLE_ACTION
#define CTLE_ACTION
#include "callable.h"

#include <optional>

namespace ctle {
/**
 * @brief 	a wrapper for action objects which distinguishes those which return and those which
 * don't. if an action wishes to return conditionaly return std::optional<ReturnType> from that
 * action and if it's empty, the caller won't return, if always returns, return ReturnType, if
 * doesn't return, specify return type as void.
 *
 * @tparam callable a constexpr constructible callable object.
 * @tparam ReturnType desired return_type if any.
 */
template<callable Action, typename ReturnType>
class action
{
    using optional_return_t = std::optional<ReturnType>;

public:
    /**
     * @brief 	wraps an operator of contained callable and provides some compile time errorchecking
     * 			as well as support for "void" in return type of said callable.
     *
     * @tparam Args argpack to forward.
     * @param args 	argpack to forward.
     * @return optional_return_t a std::optional<ReturnType> empty only if wrapped has void return
     * type.
     */
    template<typename... Args>
    optional_return_t operator()(Args&&... args) const {
        using action_return_t = decltype(Action(std::forward<Args>(args)...));
        // distinguish between returning and non_returning ones.
        if constexpr (!std::is_same_v<action_return_t, void>) {
            // check if return type is correct.
            static_assert(
              std::is_same_v<action_return_t,
                             ReturnType> || std::is_same_v<action_return_t, optional_return_t>,
              "Action has a wrong return type.");
            return optional_return_t{Action(std::forward<Args>(args)...)};
        } else {
            Action(std::forward<Args>(args)...);
            return optional_return_t{};
        }
    }
};
} // namespace ctle
#endif // CTLE_ACTION