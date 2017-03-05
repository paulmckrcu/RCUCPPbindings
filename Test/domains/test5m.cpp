#include "urcu-mb.hpp"

static std::rcu_mb _rm;
std::rcu_flavor_base &rm = _rm;
