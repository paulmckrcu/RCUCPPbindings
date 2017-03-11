#include <iostream>
#include <string>
#include <unistd.h>
/* #include <urcu.h> */
#include "urcu-bp.hpp"
#include "urcu-rv.hpp"

struct foo {
	int a;
	rcu_head rh;
};

struct foo my_foo;

void my_func(rcu_head *rhp)
{
	std::cout << "Hello World from a callback!\n";
}

template <typename _T>
void synchronize_rcu_abstract(_T &p, std::string s)
{
	std::rcu_flavor_base::cookie_t c;

	std::cout << s << "\n";
	p.register_thread();
	c = p.read_lock();
	p.read_unlock(c);
	p.quiescent_state();
	p.synchronize();
	p.retire(&my_foo.rh, my_func);
	p.barrier();
	p.unregister_thread();
}

std::rcu_bp rb;
std::rcu_rv rv;

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
	synchronize_rcu_abstract(rv, "Derived class rcu_rv");
}
