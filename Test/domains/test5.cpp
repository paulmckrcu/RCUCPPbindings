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

void synchronize_rcu_abstract(std::rcu_domain &p, std::string s)
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

extern std::rcu_domain &rb;
extern std::rcu_domain &rm;
extern std::rcu_domain &rq;
extern std::rcu_domain &rs;
extern std::rcu_domain &rv;

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
