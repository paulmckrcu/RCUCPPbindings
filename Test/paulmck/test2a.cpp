#include <iostream>
#include <unistd.h>
#include <mutex>
#define RCU_SIGNAL
#include <urcu.h>
#include <rcu.hpp>

// std::unique_lock<std::rcu_reader> blork;

// Derived-type approach, and derived from ajodwyer/test2.cpp.
// All bugs property of subsequent submitter.

struct foo: public std::rcu_obj_base<foo> {
    int a;
};

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
	std::scoped_lock<std::rcu_domain> rdr1(std::rcu_default_domain());

	std::cout << "Attempting RAII on fp->a " << fp->a << "\n";
	std::cout << "End of attempted RAII\n";
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

    std::rcu_synchronize();

    rcu_unregister_thread();

    return 0;
}
