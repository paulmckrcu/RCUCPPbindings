#include "urcu-signal.hpp"

static std::rcu_signal _rs;
std::rcu_flavor_base &rs = _rs;
