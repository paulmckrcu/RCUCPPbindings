#include <iostream>
#include <unistd.h>
#include "urcu-signal.h"

struct foo {
	int a;
	struct rcu_head rh;
};

struct foo my_foo;

void my_func(struct rcu_head *rhp)
{
	std::cout << "Hello World from a callback!\n";
}

int main()
{
	rcu_read_lock();
	rcu_read_unlock();
	synchronize_rcu();
	std::cout << "Hello World!\n";
	call_rcu(&my_foo.rh, my_func);
	sleep(1);
}
