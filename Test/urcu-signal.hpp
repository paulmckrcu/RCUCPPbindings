
#include <urcu.h>
#include "rcu_domain.hpp"

#define _LGPL_SOURCE
#define RCU_SIGNAL


namespace std {

	class rcu_signal: public rcu_domain {
#include "rcu_domain_derived.hpp"
	};
}
