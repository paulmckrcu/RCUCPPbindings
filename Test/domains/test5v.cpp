#include "urcu-rv.hpp"

static std::rcu_rv _rv;
std::rcu_flavor_base &rv = _rv;
