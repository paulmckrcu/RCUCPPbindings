#include <iostream>
#include <unistd.h>
#define RCU_SIGNAL
#include <urcu.h>
#include <rcu.hpp>

// Derived-type approach.

struct foo: public std::rcu_obj_base<foo> {
    int a;
};

int main(int argc, char **argv)
{
    struct foo *fp = new struct foo;

    printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(std::rcu_obj_base<foo>), sizeof(foo));

    {
    	std::rcu_reader rdr1();
    }

    // First with a normal function.
    fp->a = 42;
    fp->retire();
    std::rcu_reader::barrier(); // Drain all callbacks on general principles

    // Next with a rcu_domain
    fp = new struct foo;
    fp->a = 43;
    fp->retire();
    std::rcu_reader::barrier();

    return 0;
}
