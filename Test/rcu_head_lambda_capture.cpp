#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_lambda_capture.hpp"

// Lambda-capture approach.

struct foo: public rcu_head_funcptr<foo, [foo*] () {}> {
	int a;
};

struct foo foo1;

int main(int argc, char **argv)
{
	struct foo *fp = &foo;

	fp->a = 42;
	call_rcu(fp,
		      [=fp] (void) {
				std::cout << "Callback fp->a: " << fp->a << "\n";
		      });
	rcu_barrier();

	return 0;
}
