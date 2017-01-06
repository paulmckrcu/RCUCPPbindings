#include "rcu_ptr.hpp"

#include <iostream> // std::cout

struct Foo {
  explicit Foo(int i) : a(i) { }
  int a;
};

int main() {
  Foo f(42);
  int j = 5;
  std::experimental::rcu_ptr<Foo> fp(&f);
  fp.retire([=](Foo* fval) {
      std::cout << "Callback: " << fval->a << std::endl;
      std::cout << "Captured j as " << j << std::endl;
  });
  rcu_barrier();
}
