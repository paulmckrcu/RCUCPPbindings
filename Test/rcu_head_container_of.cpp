#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_container_of.hpp"

// container_of() approach, encapsulated into rcu_head_container_of class.

struct foo {
	int a;
	struct std::rcu_head rh;
};

std::rcu_head_container_of<struct foo> frh_foo(&foo::rh);

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
