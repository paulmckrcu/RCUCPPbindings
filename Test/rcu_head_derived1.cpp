#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"

// Derived-type approach.

struct foo: public std::rcu_head {
	int a;
};

void my_cb(struct std::rcu_head *rhp)
{
	struct foo *fp = static_cast<struct foo *>(rhp);

	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1;

int main(int argc, char **argv)
{
	// First with a normal function.
	foo1.a = 42;
	std::call_rcu(&foo1, my_cb);
	std::rcu_barrier(); // Drain all callbacks before reusing them!

	// Next with a lambda, but no capture.
	foo1.a = 43;
	std::call_rcu(&foo1,
		      [] (struct std::rcu_head *rhp) {
			struct foo *fp = static_cast<struct foo *>(rhp);

			std::cout << "Lambda callback fp->a: " << fp->a << "\n";
		      });
	std::rcu_barrier();

	return 0;
}
