#include <unistd.h>
#include "urcu-qsbr.hpp"

static class std::rcu_qsbr _rq;
class std::rcu_domain &rq = _rq;
