#include <urcu-bp.h>
#include "rcu_domain.hpp"
#define _LGPL_SOURCE
namespace std {

	class rcu_bp: public rcu_domain {
#include "rcu_domain_derived.hpp"
		static inline bool register_thread_needed() { return false; }
	};
}
