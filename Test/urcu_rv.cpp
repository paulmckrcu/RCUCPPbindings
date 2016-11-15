#include "urcu_rv.hpp"

static class std::rcu_rv _rv;
class std::rcu_domain &rv = _rv;
