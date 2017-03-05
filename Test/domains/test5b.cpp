#include "urcu-bp.hpp"

static std::rcu_bp _rb;
std::rcu_flavor_base &rb = _rb;
