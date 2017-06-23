#include "urcu-signal.hpp"

static std::rcu_signal _rs;
std::rcu_domain &rs = _rs;
