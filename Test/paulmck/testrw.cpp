#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <thread>

std::shared_mutex myrwlock;
int a, b; // Current configuration values

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
	int my_a, my_b;

	my_set(3, 4);
	my_get(&my_a, &my_b);
	printf("a = %d, b = %d\n", my_a, my_b);
	return 0;
}
