#include <unistd.h>
#include "urcu-bp.hpp"

static class rcu_bp _rb;
class rcu_domain &rb = _rb;
