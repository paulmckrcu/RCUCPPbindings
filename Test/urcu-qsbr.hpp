#include "rcu_domain.hpp"
#define _LGPL_SOURCE
namespace std {
#include <urcu-qsbr.h>

	class rcu_qsbr: public rcu_domain {
#include "rcu_domain_derived.hpp"
	};
}
