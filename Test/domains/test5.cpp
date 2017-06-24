#include <iostream>
#include <string>
#include <unistd.h>
#include <urcu.h>
#include "rcu_domain.hpp"

struct foo {
	int a;
	rcu_head rh;
};

struct foo my_foo;

void my_func(rcu_head *rhp)
{
	std::cout << "Hello World from a callback!\n";
}

void synchronize_rcu_abstract(std::rcu::rcu_domain_base& p, std::string s)
{
	std::cout << s << "\n";
	p.register_thread();
	p.read_lock();
	p.read_unlock();
	p.quiescent_state();
	p.synchronize();
	p.retire(&my_foo.rh, my_func);
	p.barrier();
	p.unregister_thread();
}

extern std::rcu::rcu_domain_base& rb;
extern std::rcu::rcu_domain_base& rm;
extern std::rcu::rcu_domain_base& rq;
extern std::rcu::rcu_domain_base& rs;
extern std::rcu::rcu_domain_base& rv;

int main()
{
	rcu_register_thread();
	rcu_read_lock();
	rcu_read_unlock();
	synchronize_rcu();
    std::cout << "Hello World!\n";
	call_rcu(&my_foo.rh, my_func);
	rcu_barrier();
	rcu_unregister_thread();

	synchronize_rcu_abstract(rb, "Derived class rcu_bp");
	synchronize_rcu_abstract(rm, "Derived class rcu_mb");
	synchronize_rcu_abstract(rq, "Derived class rcu_qsbr");
	synchronize_rcu_abstract(rs, "Derived class rcu_signal");
	synchronize_rcu_abstract(rv, "Derived class rcu_rv");
}
