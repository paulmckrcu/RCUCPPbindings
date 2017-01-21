#include "rcu_ptr.hpp"
#include <assert.h>

int Foo_destructions;

struct Foo {
  int a;

  explicit Foo(int i) : a(i) { }
  ~Foo() { ++Foo_destructions; }
};

void test_default_retire()
{
    Foo_destructions = 0;
    Foo *pf = new Foo(42);
    {
        std::experimental::rcu_ptr<Foo> fp(pf);
        fp.retire();  // defers a call to "delete pf"
    }
    assert(Foo_destructions == 0);
    rcu_barrier();
    assert(Foo_destructions == 1);
}

void test_lambda_retire()
{
    Foo_destructions = 0;
    Foo f(42);
    int j = 5;
    int calls = 0;
    {
        std::experimental::rcu_ptr<Foo> fp(&f);
        fp.retire([j, &calls](Foo* fval) {
            assert(fval->a == 42);
            assert(j == 5);
            assert(calls == 0);
            ++calls;
        });  // defers a call to this lambda
    }
    assert(Foo_destructions == 0 && calls == 0);
    rcu_barrier();
    assert(Foo_destructions == 0 && calls == 1);
}

void test_default_destructor()
{
    Foo_destructions = 0;
    Foo f(42);
    {
        std::experimental::rcu_ptr<Foo> fp(&f);
        // destroying the rcu_ptr does not retire or destroy f itself
    }
    assert(Foo_destructions == 0);
    rcu_barrier();
    assert(Foo_destructions == 0);
}

void test_lambda_destructor()
{
    Foo_destructions = 0;
    Foo f(42);
    int j = 5;
    int calls = 0;
    {
        std::experimental::rcu_ptr<Foo> fp(&f, [j, &calls](Foo* fval) {
            ++calls;
            assert(false);
        });
        // destroying the rcu_ptr does not call the lambda
    }
    assert(Foo_destructions == 0 && calls == 0);
    rcu_barrier();
    assert(Foo_destructions == 0 && calls == 0);
}

void test_lambda_destructor_plus_retire()
{
    Foo_destructions = 0;
    Foo f(42);
    int j = 5;
    int calls = 0;
    {
        std::experimental::rcu_ptr<Foo> fp(&f, [j, &calls](Foo* fval) {
            assert(fval->a == 42);
            assert(j == 5);
            assert(calls == 0);
            ++calls;
        });
        fp.retire();  // defers a call to the lambda above
    }
    assert(Foo_destructions == 0 && calls == 0);
    rcu_barrier();
    assert(Foo_destructions == 0 && calls == 1);
}

int main()
{
    test_default_retire();
    test_lambda_retire();
    test_default_destructor();
    test_lambda_destructor();
    test_lambda_destructor_plus_retire();
}
