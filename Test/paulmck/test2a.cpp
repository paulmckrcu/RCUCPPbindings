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

std::scoped_lock<std::rcu_domain> *start_reader()
{
	auto new_rdr = new std::scoped_lock<std::rcu_domain>(std::rcu_default_domain());

	return new_rdr;
}

void end_reader(std::scoped_lock<std::rcu_domain> *old_rdr)
{
	delete old_rdr;
}

void dynamic_reader()
{
	std::cout << "Attempting abstracted RCU reader\n";
	auto my_rdr = start_reader();
	end_reader(my_rdr);
}

std::unique_lock<std::rcu_domain> start_deferred_reader()
{
    	std::unique_lock<std::rcu_domain> newu_rdr(std::rcu_default_domain());

	return std::move(newu_rdr);
}

void end_deferred_reader(std::unique_lock<std::rcu_domain> oldu_rdr)
{
}

void dynamic_deferred_reader()
{
	std::cout << "Attempting abstracted RCU reader via deferral\n";
	auto rdrud = std::move(start_deferred_reader());
	end_deferred_reader(std::move(rdrud));
}

int main(int argc, char **argv)
{
    struct foo *fp = new struct foo;

    printf("%zu %zu %zu\n", sizeof(rcu_head), sizeof(std::rcu_obj_base<foo>), sizeof(foo));

    fp->a = 42;
    rcu_register_thread();

    {
	std::scoped_lock<std::rcu_domain> rdr1();

	std::cout << "Attempting RAII on fp->a " << fp->a << "\n";
	std::cout << "End of attempted RAII\n";

	// Does std::scoped_lock<> need to know about rcu_domain
	// in order to allow defer_lock tricks?  But unique_lock
	// works just fine, see below.
    }

    {
    	std::unique_lock<std::rcu_domain> rdru(std::rcu_default_domain());
	std::cout << "RAII unique_lock\n";
    	std::unique_lock<std::rcu_domain> rdrud(std::rcu_default_domain(), std::defer_lock);
	rdrud = std::move(rdru);
    }

    dynamic_reader();
    dynamic_deferred_reader();

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
