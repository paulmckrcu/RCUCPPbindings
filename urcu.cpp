#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"

struct foo {
	int a;
	struct std::rcu_head rh;
};

struct foo my_foo;

void my_func(struct std::rcu_head *rhp)
{
	std::cout << "Hello World from a callback!\n";
}

void synchronize_rcu_abstract(class std::rcu_domain &p)
{
	p.synchronize();
}

int main()
{
	class std::rcu_signal rs;

	std::rcu_register_thread();
	std::rcu_read_lock();
	std::rcu_read_unlock();
	std::synchronize_rcu();
	std::cout << "Hello World!\n";
	std::call_rcu(&my_foo.rh, my_func);
	std::rcu_barrier();
	std::rcu_unregister_thread();

	std::cout << "Hello World via derived class!\n";
	rs.register_thread();
	rs.read_lock();
	rs.read_unlock();
	synchronize_rcu_abstract(rs);
	rs.call(&my_foo.rh, my_func);
	rs.barrier();
	rs.unregister_thread();
}
