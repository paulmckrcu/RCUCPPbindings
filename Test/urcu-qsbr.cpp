#include <unistd.h>
#include "urcu-qsbr.hpp"

static class rcu_qsbr _rq;
class rcu_domain &rq = _rq;
