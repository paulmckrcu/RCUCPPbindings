#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"

struct foo {
	int a;
	struct rcu_head rh;
};

struct foo my_foo;

void my_func(struct rcu_head *rhp)
{
	std::cout << "Hello World from a callback!\n";
}

void synchronize_rcu_abstract(class rcu_domain &p, std::string s)
{
	std::cout << s << "\n";
	p.register_thread();
	p.read_lock();
	p.read_unlock();
	p.quiescent_state();
	p.synchronize();
	p.call(&my_foo.rh, my_func);
	p.barrier();
	p.unregister_thread();
}

extern class rcu_domain &rb;
extern class rcu_domain &rm;
extern class rcu_domain &rq;

int main()
{
	class rcu_signal rs;

	rcu_register_thread();
	rcu_read_lock();
	rcu_read_unlock();
	synchronize_rcu();
	std::cout << "Hello World!\n";
	call_rcu(&my_foo.rh, my_func);
	rcu_barrier();
	rcu_unregister_thread();

	synchronize_rcu_abstract(rs, "Derived class rcu_signal");
	synchronize_rcu_abstract(rb, "Derived class rcu_bp");
	synchronize_rcu_abstract(rm, "Derived class rcu_mb");
	synchronize_rcu_abstract(rq, "Derived class rcu_qsbr");
}
