#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_container_of.hpp"

// container_of() approach, encapsulated into rcu_head_container_of class.

struct foo {
	int a;
	struct std::rcu_head rh;
};

void my_cb(struct std::rcu_head *rhp)
{
	struct foo *fp;

	fp = std::rcu_head_container_of<struct foo>::enclosing_class(rhp);
	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1 = { 42 };

int main(int argc, char **argv)
{
	std::rcu_head_container_of<struct foo>::set_field(&foo::rh);

	// First with a normal function.
	std::call_rcu(&foo1.rh, my_cb);
	std::rcu_barrier(); // Drain all callbacks before reusing them!

	// Next with a lambda, but no capture.
	foo1.a = 43;
	std::call_rcu(&foo1.rh,
		      [] (std::rcu_head *rhp) {
			struct foo *fp;

			fp = std::rcu_head_container_of<struct foo>::enclosing_class(rhp);
			std::cout << "Lambda callback fp->a: " << fp->a << "\n";
		      });
	std::rcu_barrier();

	return 0;
}
