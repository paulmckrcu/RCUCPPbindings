#include "urcu-bp.hpp"

static std::rcu_bp _rb;
std::rcu_domain &rb = _rb;
