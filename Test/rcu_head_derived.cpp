#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_derived.hpp"

// Derived-type approach.

struct foo: public std::rcu_head_derived<foo> {
	int a;
};

void my_cb(struct foo *fp)
{
	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1;

int main(int argc, char **argv)
{
	struct foo *fp;
	class std::rcu_signal rs;

	// First with a normal function.
	foo1.a = 42;
	foo1.call(my_cb);
	std::rcu_barrier(); // Drain all callbacks before reusing them!

	// Next with a lambda, but no capture.
	foo1.a = 43;
	foo1.call([] (struct foo *fp) {
			std::cout << "Lambda callback fp->a: " << fp->a << "\n";
		  });
	std::rcu_barrier();

	std::cout << "Deletion with no rcu_domain\n";
	fp = new foo;
	fp->a = 44;
	fp->call();

	std::cout << "Deletion with rcu_signal rcu_domain\n";
	fp = new foo;
	fp->a = 44;
	fp->call(rs);

	return 0;
}
