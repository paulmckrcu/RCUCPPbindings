#define _LGPL_SOURCE
namespace std {
#include <urcu-bp.h>

	class rcu_bp: public rcu_domain {
#include "urcu.hpp"
	};
}
