#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_delete.hpp"

// Derived-type approach.

struct foo: public rcu_head_delete<foo> {
	int a;
};

int main(int argc, char **argv)
{
	struct foo *fp = new struct foo;
	class rcu_signal rs;

	printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(rcu_head_delete<foo>), sizeof(foo));

	// First with a normal function.
	fp->a = 42;
	fp->call();
	rcu_barrier(); // Drain all callbacks on general principles

	// Next with a rcu_domain
	fp = new struct foo;
	fp->a = 43;
	fp->call(rs);
	rcu_barrier();

	return 0;
}
