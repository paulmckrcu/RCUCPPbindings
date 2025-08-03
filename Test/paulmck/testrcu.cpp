#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <thread>
#include <atomic>
#include <cassert>
#define RCU_SIGNAL
#include <urcu.h>
#include <rcu.hpp>

struct myconfig {
	int a;
	int b;
};
std::atomic<struct myconfig *> curconfig;

void get(int *cur_a, int *cur_b)
{
	struct myconfig *mcp;

	std::scoped_lock<std::rcu_domain> rdr1(std::rcu_default_domain());
	mcp = curconfig.load(std::memory_order::consume);
	*cur_a = mcp->a;
	*cur_b = mcp->b;
}

std::mutex mylock;

void set(int cur_a, int cur_b)
{
	struct myconfig *mcp = new myconfig();
	struct myconfig *oldmcp;

	assert(mcp);
	mcp->a = cur_a;
	mcp->b = cur_b;
	{
		std::unique_lock lock(mylock);
		oldmcp = curconfig.exchange(mcp);
	}
	std::rcu_synchronize();
	delete oldmcp;
}

int main(int argc, char *argv[])
{
	struct myconfig *mcp = new myconfig(1, 7);
	int my_a, my_b;

	curconfig.store(mcp, std::memory_order::release);
	get(&my_a, &my_b);
	printf("a = %d, b = %d\n", my_a, my_b);
	assert(my_a == 1 && my_b == 7);
	set(3, 4);
	get(&my_a, &my_b);
	printf("a = %d, b = %d\n", my_a, my_b);
	assert(my_a == 3 && my_b == 4);
	return 0;
}
