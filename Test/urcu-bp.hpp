#include "rcu_domain.hpp"
#define _LGPL_SOURCE
namespace std {
#include <urcu-bp.h>

	class rcu_bp: public rcu_domain {
#include "rcu_domain_derived.hpp"
	};
}
