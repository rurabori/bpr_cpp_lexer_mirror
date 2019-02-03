#ifndef CTLE_STATES
#define CTLE_STATES

namespace ctle {
    /**
     * @brief class to hold state information.
     * 
     * @tparam T type of state (an enum or an enum class).
     */
    template <typename T>
    struct state {
        T       value;
        bool    exclusive{false};
    };

    // always defined states.
    constexpr int state_initial = 0;
    constexpr int all_states = 1;

    // just so that we have some breathing room
    constexpr int state_reserved = 256;
}
#endif // CTLE_STATES
