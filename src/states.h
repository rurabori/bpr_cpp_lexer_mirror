#ifndef CTLE_STATES
#define CTLE_STATES

template <typename T>
struct state {
    T value;
    bool exclusive{false};
};

const int state_initial = 0;
const int all_states = 1;

// just so that we have some breathing room
const int state_reserved = 256;

#endif // CTLE_STATES
