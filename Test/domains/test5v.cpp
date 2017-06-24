#include "urcu-rv.hpp"

static rcu_domain_rv _rv;
static std::rcu::rcu_domain_wrapper<decltype(_rv)> _rvw(_rv);
std::rcu::rcu_domain_base& rv = _rvw;
