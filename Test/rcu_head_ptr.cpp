#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_ptr.hpp"

// Added-pointer approach

struct foo {
	int a;
	class std::rcu_head_ptr<struct foo> rh;
};

void my_cb(struct std::rcu_head *rhp)
{
	// class std::rcu_head_ptr<struct foo> *rhp1 = rcu_head_to_T(rhp, rhp1);
	class std::rcu_head_ptr<struct foo> *rhp1 =
		rhp1->rcu_head_to_rcu_head_ptr(rhp);
	struct foo *fp = rhp1->enclosing_class();

	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1;

int main(int argc, char **argv)
{
	class std::rcu_head_ptr<struct foo> *rhp = &foo1.rh;

	foo1.a = 42;
	foo1.rh = &foo1;
	std::call_rcu(rhp, my_cb);
	std::rcu_barrier();

	return 0;
}
