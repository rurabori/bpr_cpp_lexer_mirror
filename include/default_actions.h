#ifndef CTLE_DEFAULT_ACTIONS
#define CTLE_DEFAULT_ACTIONS
#include "ctle_concepts.h"

#include <optional>
#include <iostream>

namespace ctle::default_actions {

constexpr auto echo
  = [](LexerInterface& b, auto&&... all) -> void { (std::cout << ... << all) << '\n'; };

constexpr auto simple_return = [](auto retval) { return [retval](auto&&...) { return retval; }; };

} // namespace ctle::default_actions
#endif // CTLE_DEFAULT_ACTIONS
