#include <atomic>
#include <memory>
#include <thread>
#include <vector>
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
        t[i] = std::thread([i, &c]{
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

void test_non_race_free_type()
{
    static auto get_next_value = []{
        static std::atomic<int> x(0);
        return ++x;
    };
    static auto the_zero_vector = []{
        return std::make_unique<std::vector<A>>(10, A(0));
    };
    static auto print_vector = [](const auto& c){
        static std::mutex m;
        std::lock_guard<std::mutex> lock(m);  // Just to avoid interleaving output.
        auto sp = c.get_snapshot();
        for (int i=0; i < sp->size(); ++i) {
            printf("%d ", (*sp)[i].value);
        }
        printf("\n");
    };
    std::rcu::cell<std::vector<A>> c(the_zero_vector());
    std::thread t[10];
    for (int i=0; i < 10; ++i) {
        t[i] = std::thread([i, &c]{
            int result = get_next_value();
            if (result == 3) {
                // Zero the whole vector, thread-safely.
                c.update(the_zero_vector());
            } else {
                // Update only my own element of the vector.
                auto sp = c.get_snapshot();
                (*sp)[i] = A(result);
            }
            print_vector(c);
        });
    }
    for (int i=0; i < 10; ++i) {
        t[i].join();
    }
    print_vector(c);
}

int main(int argc, char **argv)
{
    test_simple();
    rcu_barrier(); assert(A::live_objects == 0);
    test_outliving();
    rcu_barrier(); assert(A::live_objects == 0);
    test_shared_ptr();
    rcu_barrier(); assert(A::live_objects == 0);
    test_thread_safety();
    rcu_barrier(); assert(A::live_objects == 0);
    test_non_race_free_type();
    rcu_barrier(); assert(A::live_objects == 0);
    return 0;
}
