#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu.hpp"

// Derived-type approach.

struct foo: public std::rcu_obj_base<foo> {
    int a;
};

int main(int argc, char **argv)
{
    struct foo *fp = new struct foo;
    std::rcu_signal rs;

    printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(std::rcu_obj_base<foo>), sizeof(foo));

    // First with a normal function.
    fp->a = 42;
    fp->retire();
    rcu_barrier(); // Drain all callbacks on general principles

    // Next with a rcu_domain
    fp = new struct foo;
    fp->a = 43;
    fp->retire(rs);
    rcu_barrier();

    return 0;
}
