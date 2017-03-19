#include "urcu-signal.hpp"

static rcu_domain_signal _rs;
static std::rcu::rcu_domain_wrapper<decltype(_rs)> _rsw(_rs);
std::rcu::rcu_domain_base& rs = _rsw;
