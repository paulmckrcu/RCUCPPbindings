#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <thread>
#include <cassert>

std::shared_mutex myrwlock;
int a = 1, b = 7; // Current configuration values

void my_get(int *cur_a, int *cur_b)
{
	std::shared_lock lock(myrwlock);
	*cur_a = a;
	*cur_b = b;
}

void my_set(int new_a, int new_b)
{
	std::unique_lock lock(myrwlock);
	a = new_a;
	b = new_b;
}

int main(int argc, char *argv[])
{
	int my_a;
	int my_b;

	my_get(&my_a, &my_b);
	printf("a = %d, b = %d\n", my_a, my_b);
	assert(my_a == 1 && my_b == 7);
	my_set(3, 4);
	my_get(&my_a, &my_b);
	printf("a = %d, b = %d\n", my_a, my_b);
	assert(my_a == 3 && my_b == 4);
	return 0;
}
