#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_ptr.hpp"

// Added-pointer approach

struct foo {
	foo(int i)
	{
		this->a = i;
	}
	int a;
	class std::rcu_head_ptr<struct foo> rh;
};

void my_cb(struct std::rcu_head *rhp)
{
	struct foo *fp = fp->rh.enclosing_class(rhp);

	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1(42);

int main(int argc, char **argv)
{
	foo1.rh = &foo1;
	std::call_rcu(&foo1.rh, my_cb);
	std::rcu_barrier();

	foo1 = 43;
	foo1.rh = &foo1;
	std::call_rcu(&foo1.rh,
		      [] (struct std::rcu_head *rhp) {
		      	struct foo *fp = fp->rh.enclosing_class(rhp);

			std::cout << "Callback fp->a: " << fp->a << "\n";
		      });
	std::rcu_barrier();

	return 0;
}
