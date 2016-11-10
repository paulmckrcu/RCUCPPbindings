#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"

// Derived-type approach.

struct foo: public rcu_head {
	int a;
};

void my_cb(rcu_head *rhp)
{
	struct foo *fp = static_cast<struct foo *>(rhp);

	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1;

int main(int argc, char **argv)
{
	// First with a normal function.
	foo1.a = 42;
	call_rcu(&foo1, my_cb);
	rcu_barrier(); // Drain all callbacks before reusing them!

	// Next with a lambda, but no capture.
	foo1.a = 43;
	call_rcu(&foo1,
		      [] (rcu_head *rhp) {
			struct foo *fp = static_cast<struct foo *>(rhp);

			std::cout << "Lambda callback fp->a: " << fp->a << "\n";
		      });
	rcu_barrier();

	return 0;
}
