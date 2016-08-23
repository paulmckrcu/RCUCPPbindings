#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "from_rcu_head.hpp"

// container_of() approach, encapsulated into from_rcu_head class.

struct foo {
	int a;
	struct std::rcu_head rh;
};

std::from_rcu_head<struct foo> frh_foo(&foo::rh);

void my_cb(struct std::rcu_head *rhp)
{
	struct foo *fp = frh_foo.enclosing_class(rhp);

	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1 = { 42 };

int main(int argc, char **argv)
{
	std::call_rcu(&foo1.rh, my_cb);
	std::rcu_barrier();

	return 0;
}
