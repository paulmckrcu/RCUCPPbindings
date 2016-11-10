#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_ptr.hpp"

// Added-pointer approach

struct foo {
	foo(int i)
	{
		this->a = i;
		this->rh = this;
	}
	int a;
	class std::rcu_head_ptr<struct foo> rh;
};

void my_cb(struct foo *fp)
{
	std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1(42);

int main(int argc, char **argv)
{
	struct foo *fp;
	class std::rcu_signal rs;

	foo1.rh = &foo1;
	foo1.rh.call(my_cb);
	rcu_barrier();

	foo1 = 43;
	foo1.rh = &foo1;
	foo1.rh.call([] (struct foo *fp) {
			std::cout << "Callback fp->a: " << fp->a << "\n";
		      });
	rcu_barrier();

	std::cout << "Deletion with no rcu_domain\n";
	fp = new foo(44);
	fp->rh.call();
	rcu_barrier();

	std::cout << "Deletion with rcu_signal rcu_domain\n";
	fp = new foo(45);
	fp->rh.call(rs);
	rs.barrier();

	return 0;
}
