#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_delete.hpp"

// Derived-type approach.

struct foo: public std::rcu_head_delete<foo> {
    int a;
};

struct my_deleter {
    void operator () (void*) const { std::cout << "In my_deleter\n"; }
};

template <class T>
using rcu_head_t = std::rcu_head_delete<T, my_deleter>;

struct bar: public rcu_head_t<bar> {
    int a;
};

int main(int argc, char **argv)
{
    struct bar my_bar;
    struct foo *fp = new struct foo;
    rcu_domain_signal rs;

    // First with a normal function.
    fp->a = 42;
    fp->retire();
    rcu_barrier(); // Drain all callbacks on general principles

    // Next with a rcu_domain
    fp = new struct foo;
    fp->a = 43;
    fp->retire(rs);
    rcu_barrier();

    // Next with my_deleter
    my_bar.a = 44;
    my_bar.retire();
    rcu_barrier();

    return 0;
}
