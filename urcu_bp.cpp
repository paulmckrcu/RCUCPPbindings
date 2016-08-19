#include <unistd.h>
#include "urcu-bp.hpp"

static class std::rcu_bp _rb;
class std::rcu_domain &rb = _rb;
