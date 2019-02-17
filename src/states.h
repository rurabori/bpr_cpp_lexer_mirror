#ifndef CTLE_STATES
#define CTLE_STATES

namespace ctle {
    /**
     * @brief a small struct representing a definition of state.
     * 
     * @tparam Identifier the identifier of a state (most often an enum member).
     * @tparam Exclusive specifies whether the rule is exclusive or not (if it's actions are available to other states).
     * @tparam EofAction an action to be encountered if EOF reached in this state (if none specified, the default of lexer is taken).
     */
    template <auto Identifier, bool Exclusive = false, typename EofAction = std::nullptr_t>
    struct state {
        // accessors
        static constexpr auto identifier() noexcept { return Identifier; }
        static constexpr bool exclusive() noexcept { return Exclusive; }
        using eof_action_t = EofAction;
    };
       
    // always defined states.
    constexpr int state_initial = 0;
    constexpr int all_states = 1;

    // just so that we have some breathing room
    constexpr int state_reserved = 256;
}
#endif // CTLE_STATES
