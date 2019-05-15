#ifndef CTLE_STATES
#define CTLE_STATES

#include "callable.h"
#include "container.h"
#include "default_actions.h"

namespace ctle {

/**
 * @brief A class to specify an action to execute when a condition occurs within lexer in a
 * specified state.
 *
 * @tparam Eof an action to execute when EOF encountered.
 * @tparam NoMatch an action to execute when lexer fails to match any text.
 */
template<callable Eof = empty_callable, callable NoMatch = empty_callable>
struct actions
{
    // bring to scope so that it can be used.
    static constexpr auto eof = Eof;
    static constexpr auto no_match = NoMatch;
};
/**
 * @brief A workaround for GCC bug 90335, behaves like actions.
 *
 * @tparam Ty the type to take the defaults from (should be an enumeration).
 */
template<typename Ty>
struct defaults
{
    static constexpr auto eof = default_actions::simple_return(Ty::eof);
    static constexpr auto no_match = default_actions::simple_return(Ty::no_match);
};

// just so that we have some breathing room
constexpr int state_reserved = 256;

/**
 * @brief a small struct representing a definition of state.
 *
 * @tparam Identifier the identifier of a state (most often an enum member).
 * @tparam Exclusive specifies whether the rule is exclusive or not (if it's actions are available
 * to other states).
 * @tparam Actions @see ctle::action. If any of these actions is an empty_callable, the defaults for
 * the lexer are used.
 */
template<auto Identifier, bool Exclusive = false, typename Actions = actions<>>
struct state
{
    // accessors
    static constexpr auto identifier() noexcept { return Identifier; }
    static constexpr bool exclusive() noexcept { return Exclusive; }
    using actions = Actions;
};

// always defined states.
constexpr int state_initial = 0;
constexpr int all_states = 1;

/**
 * @brief an using to help better distinguish this container.
 *
 * @tparam StateT the state used as identifier for rules.
 * @tparam States a ctll::list of ctle::state-s.
 */
template<typename StateT = std::nullptr_t, typename States = ctll::list<>>
using states = container<StateT, States>;

} // namespace ctle
#endif // CTLE_STATES
