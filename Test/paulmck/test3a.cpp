#include <iostream>
#include <unistd.h>
#define RCU_SIGNAL
#include <urcu.h>
#include "rcu.hpp"

// Derived-type approach, and derived from ajodwyer/test3.cpp.
// All bugs property of subsequent submitter.

struct foo: public std::rcu_obj_base<foo, void(*)(foo*)> {
    int a;
};

void my_cb(struct foo *fp)
{
    std::cout << "Callback fp->a: " << fp->a << "\n";
}

struct foo foo1;

int main(int argc, char **argv)
{

    printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(std::rcu_obj_base<foo, void(*)(foo*)>), sizeof(foo));

    // First with a normal function.
    foo1.a = 42;
    foo1.retire(my_cb);
    std::rcu_barrier(); // Drain all callbacks before reusing them!

    // Next with a lambda, but no capture.
    foo1.a = 43;
    foo1.retire([] (struct foo *fp) {
            std::cout << "Lambda callback fp->a: " << fp->a << "\n";
          });
    std::rcu_barrier();

    std::cout << "Deletion with no rcu_domain\n";
    foo1.a = 44;
    foo1.retire(my_cb);
    std::rcu_barrier();

    std::cout << "Deletion with rcu_signal rcu_domain\n";
    foo1.a = 45;
    foo1.retire(my_cb);
    std::rcu_barrier();

    return 0;
}
