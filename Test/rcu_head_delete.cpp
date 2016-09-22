#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_delete.hpp"

// Derived-type approach.

struct foo: public std::rcu_head_delete<foo> {
	int a;
};

int main(int argc, char **argv)
{
	struct foo *fp = new struct foo;
	class std::rcu_signal rs;

	printf("%zu %zu %zu\n", sizeof(std::rcu_head), sizeof(std::rcu_head_delete<foo>), sizeof(foo));

	// First with a normal function.
	fp->a = 42;
	fp->call();
	std::rcu_barrier(); // Drain all callbacks on general principles

	// Next with a rcu_domain
	fp = new struct foo;
	fp->a = 43;
	fp->call(rs);
	std::rcu_barrier();

	return 0;
}
