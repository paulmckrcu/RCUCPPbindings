#include "rcu_domain.hpp"
#define _LGPL_SOURCE
#define RCU_SIGNAL
#include <urcu.h>

class rcu_signal: public rcu_domain {
#include "rcu_domain_derived.hpp"
};
