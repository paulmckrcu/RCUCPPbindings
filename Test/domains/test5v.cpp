#include "urcu-rv.hpp"

static std::rcu_rv _rv;
std::rcu_domain &rv = _rv;
