#include "urcu-qsbr.hpp"

static std::rcu_qsbr _rq;
std::rcu_domain &rq = _rq;
