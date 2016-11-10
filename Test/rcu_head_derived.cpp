#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_delete.hpp"

// Derived-type approach.

struct foo: public rcu_head_delete<foo, void(*)(foo*)> {
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
	class rcu_signal rs;

	printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(rcu_head_delete<foo, void(*)(foo*)>), sizeof(foo));

	// First with a normal function.
	foo1.a = 42;
	foo1.call(my_cb);
	rcu_barrier(); // Drain all callbacks before reusing them!

	// Next with a lambda, but no capture.
	foo1.a = 43;
	foo1.call([] (struct foo *fp) {
			std::cout << "Lambda callback fp->a: " << fp->a << "\n";
		  });
	rcu_barrier();

	std::cout << "Deletion with no rcu_domain\n";
	fp = new foo;
	fp->a = 44;
	fp->call();
	rcu_barrier();

	std::cout << "Deletion with rcu_signal rcu_domain\n";
	fp = new foo;
	fp->a = 45;
	fp->call(rs);
	rs.barrier();

	return 0;
}
