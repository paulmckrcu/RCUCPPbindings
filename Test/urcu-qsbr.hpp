#include "rcu_domain.hpp"
#define _LGPL_SOURCE
#include <urcu-qsbr.h>

class rcu_qsbr: public rcu_domain {
#include "rcu_domain_derived.hpp"
};
