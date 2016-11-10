#include <urcu.h>
#include "rcu_domain.hpp"
#define _LGPL_SOURCE
#define RCU_MB

namespace std {

	class rcu_mb: public rcu_domain {
#include "rcu_domain_derived.hpp"
	};
}
