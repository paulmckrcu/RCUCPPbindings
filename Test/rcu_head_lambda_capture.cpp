#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_lambda_capture.hpp"

// Lambda-capture approach.

struct foo: public std::rcu_head_funcptr<foo, [foo*] () {}> {
	int a;
};

struct foo foo1;

int main(int argc, char **argv)
{
	struct foo *fp = &foo;

	fp->a = 42;
	std::call_rcu(fp,
		      [=fp] (void) {
				std::cout << "Callback fp->a: " << fp->a << "\n";
		      });
	std::rcu_barrier();

	return 0;
}
