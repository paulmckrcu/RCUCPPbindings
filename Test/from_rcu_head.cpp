#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "from_rcu_head.hpp"

struct foo {
	int a;
	struct std::rcu_head rh;
};

struct foo foo1 = { 2 };

int main(int argc, char **argv)
{
	std::from_rcu_head<struct foo> frh_foo(&foo::rh);
	struct std::rcu_head *rhp = &foo1.rh;

	std::cout << frh_foo.enclosing_class(rhp)->a << "\n";

	return 0;
}
