#include "rcu_domain.hpp"
#define _LGPL_SOURCE
#define RCU_MB
namespace std {
#include <urcu.h>

	class rcu_mb: public rcu_domain {
#include "rcu_domain_derived.hpp"
	};
}
