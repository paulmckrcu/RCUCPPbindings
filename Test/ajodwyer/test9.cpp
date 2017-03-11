#include <atomic>
#include <memory>
#include <thread>
#include "urcu-signal.hpp"
#include "rcu_cell.hpp"

struct A {
    static std::atomic<int> live_objects;
    int value;
    explicit A(int v): value(v) { ++live_objects; }
    A(const A& o): value(o.value) { ++live_objects; }
    A(A&& o): value(o.value) { ++live_objects; }
    A& operator= (const A&) = default;
    A& operator= (A&&) = default;
    ~A() { value = 999; --live_objects; }
};

std::atomic<int> A::live_objects{};

void test_simple()
{
    std::rcu::cell<A> c;
    c.update(std::make_unique<A>(42));
    auto sp1 = c.get_snapshot();
    c.update(std::make_unique<A>(43));
    auto sp2 = c.get_snapshot();

    assert(A::live_objects == 2);
    assert(sp1->value == 42);
    assert(sp2->value == 43);

    sp1 = std::move(sp2);
    assert(sp1->value == 43);
    assert(sp2 == nullptr);
}

void test_outliving()
{
    std::rcu::snapshot_ptr<A> sp = nullptr;
    if (true) {
        std::rcu::cell<A> c(std::make_unique<A>(314));
        sp = c.get_snapshot();
    }
    assert(sp != nullptr);
    assert(sp->value == 314);
}

void test_shared_ptr()
{
    std::shared_ptr<A> shptr;
    if (true) {
        std::rcu::cell<A> c(std::make_unique<A>(314));
        auto sp = c.get_snapshot();
        shptr = std::move(sp);
        assert(shptr);
        assert(shptr->value == 314);
    }
    assert(shptr);
    assert(shptr->value == 314);
    shptr = nullptr;
}

void test_thread_safety()
{
    std::rcu::cell<A> c(std::make_unique<A>(0));
    std::thread t[100];
    for (int i=0; i < 100; ++i) {
        t[i] = std::thread([&]{
            c.update(std::make_unique<A>(i));
            auto sp1 = c.get_snapshot();
            c.update(std::make_unique<A>(100+i));
            auto sp2 = c.get_snapshot();
            sp1 = nullptr;
            sp2 = nullptr;
            c.update(std::make_unique<A>(200+i));
            auto sp3 = c.get_snapshot();
        });
    }
    for (int i=0; i < 100; ++i) {
        t[i].join();
    }
    auto sp = c.get_snapshot();
    assert(sp != nullptr);
    assert(sp->value >= 200);
}

int main(int argc, char **argv)
{
    test_simple();
    rcu_barrier(); printf("%d\n", (int)A::live_objects);
    test_outliving();
    rcu_barrier(); printf("%d\n", (int)A::live_objects);
    test_shared_ptr();
    rcu_barrier(); printf("%d\n", (int)A::live_objects);
    test_thread_safety();
    rcu_barrier(); printf("%d\n", (int)A::live_objects);
    return 0;
}
