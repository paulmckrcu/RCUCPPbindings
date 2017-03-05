#include "urcu-qsbr.hpp"

static std::rcu_qsbr _rq;
std::rcu_flavor_base &rq = _rq;
