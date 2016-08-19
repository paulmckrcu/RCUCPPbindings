#include "rcu.hpp"
#define _LGPL_SOURCE
#define RCU_SIGNAL
namespace std {
#include <urcu.h>

	class rcu_signal: public rcu_domain {
#include "urcu.hpp"
	};
}
