#include <iostream>
#include <unistd.h>
#include "urcu-signal.h"

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

	std::rcu_read_lock();
	std::rcu_read_unlock();
	std::synchronize_rcu();
	std::cout << "Hello World!\n";
	std::call_rcu(&my_foo.rh, my_func);
	synchronize_rcu_abstract(rs);
	sleep(1);
}
