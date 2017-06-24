#include "urcu-qsbr.hpp"

static rcu_domain_qsbr _rq;
static std::rcu::rcu_domain_wrapper<decltype(_rq)> _rqw(_rq);
std::rcu::rcu_domain_base& rq = _rqw;
