#include <iostream>
#include <unistd.h>
#define RCU_SIGNAL
#include <urcu.h>
#include <rcu.hpp>

// Derived-type approach, and derived from ajodwyer/test2.cpp.
// All bugs property of subsequent submitter.

struct foo: public std::rcu_obj_base<foo> {
    int a;
};

std::rcu_reader start_rcu_read()
{
	std::cout << "In start_rcu_read()\n";
	return {};
}

void end_rcu_read(std::rcu_reader rdr)
{
	std::cout << "In end_rcu_read()\n";
}

void my_cb(foo *fp)
{
	std::cout << "In my_cb()\n";
	delete(fp);
}

int main(int argc, char **argv)
{
    struct foo *fp = new struct foo;

    printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(std::rcu_obj_base<foo>), sizeof(foo));

    fp->a = 42;
    rcu_register_thread();

    {
	std::rcu_reader rdr1;
	std::rcu_reader rdr2(std::defer_lock);
	std::rcu_reader rdr4(std::defer_lock);

	std::cout << "Attempting RAII on fp->a " << fp->a << "\n";
	rdr2 = std::move(rdr1);
	rdr4 = start_rcu_read();
	end_rcu_read(std::move(rdr4));
	std::cout << "Back from end_rcu_read()\n";
    }

    // First with a normal function.
    fp->retire();
    std::rcu_barrier(); // Drain all callbacks on general principles

    // Next with a rcu_domain
    fp = new struct foo;
    fp->a = 43;
    fp->retire();
    std::rcu_barrier();

    // Next with bare retire().
    fp = new struct foo;
    fp->a = 44;
    std::rcu_retire(fp, my_cb);
    std::rcu_barrier();

    std::synchronize_rcu();

    rcu_unregister_thread();

    return 0;
}
