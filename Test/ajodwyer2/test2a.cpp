#include <iostream>
#include <unistd.h>
#define RCU_SIGNAL
#include <urcu.h>
#include <rcu.hpp>

// Derived-type approach.

struct foo: public std::rcu_obj_base<foo> {
    int a;
};

std::rcu_reader start_rcu_read()
{
	std::rcu_reader rdr3;

	std::cout << "In start_rcu_read()\n";
	return std::forward<std::rcu_reader>(rdr3);
}

void end_rcu_read(std::rcu_reader&& rdr)
{
	std::rcu_reader rdr5(nullptr);

	std::cout << "In end_rcu_read()\n";
	rdr5 = std::move(rdr);
}

int main(int argc, char **argv)
{
    struct foo *fp = new struct foo;

    printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(std::rcu_obj_base<foo>), sizeof(foo));

    fp->a = 42;

    {
	std::rcu_reader rdr1;
	std::rcu_reader rdr2(nullptr);
	std::rcu_reader rdr4(nullptr);

	std::cout << "Attempting RAII on fp->a " << fp->a << "\n";
	rdr2 = std::move(rdr1);
	rdr4 = std::move(start_rcu_read());
	end_rcu_read(std::forward<std::rcu_reader>(rdr4));
	std::cout << "Back from end_rcu_read()\n";
    }

    // First with a normal function.
    fp->retire();
    std::rcu_reader::barrier(); // Drain all callbacks on general principles

    // Next with a rcu_domain
    fp = new struct foo;
    fp->a = 43;
    fp->retire();
    std::rcu_reader::barrier();

    return 0;
}
