#include "urcu-bp.hpp"

static rcu_domain_bp _rb;
static std::rcu::rcu_domain_wrapper<decltype(_rb)> _rbw(_rb);
std::rcu::rcu_domain_base& rb = _rbw;
