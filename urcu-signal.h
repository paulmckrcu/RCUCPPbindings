#include "rcu.hpp"
#define _LGPL_SOURCE
#define RCU_SIGNAL
namespace std {
#include <urcu.h>

	class rcu_signal: public rcu_domain {
	public:
		void read_lock() noexcept
		{
			rcu_read_lock();
		}

		void read_unlock() noexcept
		{
			rcu_read_unlock();
		}

		void synchronize() noexcept
		{
			synchronize_rcu();
		}

		void call(class rcu_head *rhp,
			  void cbf(class rcu_head *rhp))
		{
			call_rcu(rhp, cbf);
		}
	};
}
