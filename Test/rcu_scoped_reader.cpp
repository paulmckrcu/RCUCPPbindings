#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_scoped_reader.hpp"

int main()
{
	rcu_register_thread();
	{
		class rcu_scoped_reader rr;
	}
	{
		class rcu_scoped_reader rrs(rcu_signal);
	}
	rcu_unregister_thread();

	return 0;
}
