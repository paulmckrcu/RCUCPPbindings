#include "urcu-mb.hpp"

static rcu_domain_mb _rm;
static std::rcu::rcu_domain_wrapper<decltype(_rm)> _rmw(_rm);
std::rcu::rcu_domain_base& rm = _rmw;
