#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
// #include "rcu_head_ptr.hpp"

// Lambda-capture approach -- just ignore RCU's rcu_head pointer!
// But of limited practical use.
//	http://stackoverflow.com/questions/7852101/c-lambda-with-captures-as-a-function-pointer

struct foo {
	int a;
	std::rcu_head rh;
};

struct foo foo1;

int main(int argc, char **argv)
{
	foo1.a = 42;
	std::call_rcu(&foo1.rh,
		      [] (std::rcu_head *rhp) {
				struct foo *fp = &foo1;

				std::cout << "Callback fp->a: " << fp->a << "\n";
		      });

	std::rcu_barrier();

	return 0;
}
