#ifndef CTLE_CONTAINER
#define CTLE_CONTAINER
#include <ctll/list.hpp>

namespace ctle {
/**
 * @brief A structure to hold a type and a ctll::list, used for rules and states.
 *
 * @tparam Ty the type to hold
 * @tparam List The list to hold.
 */
template<typename Ty, typename List>
struct container
{
    using type = Ty;
    using list = List;
};
} // namespace ctle
#endif