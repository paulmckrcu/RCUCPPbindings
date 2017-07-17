#pragma once

#include <memory>
#include <experimental/optional>
#include "rcu.hpp"

// Class template std::rcu::indirect<T,D> is provided in the std namespace, but it
// can be implemented "user-side" if need be; it does not have to be part of an
// initial proposal. It allows associating an existing pointer with a deleter
// instance, so that you can retire objects that don't themselves inherit from
// std::rcu_obj_base<T,D>.

// For compatibility with dshollman/rcu_ptr.hpp, we allow the user to pass in
// a deleter instance either at retire-time *or* at construction-time. Since
// the underlying std::rcu_obj_base does not allow setting the
// deleter until retire-time, our implementation of std::rcu::indirect must
// reserve enough space to keep the construction-time-specified deleter until
// it's ready to pass in at retire-time.

namespace std {
namespace rcu {

namespace detail {
    template<class D>
    class indirect_deleter {
        D d;
      public:
        indirect_deleter() = default;
        explicit indirect_deleter(D d) : d(std::move(d)) {}
        template<typename MD> void operator()(MD *md) const { d(md->t); }
    };
} // namespace detail

template<class T, class D = std::default_delete<T>>
class indirect {
    struct metadata : std::rcu_obj_base<metadata, detail::indirect_deleter<D>> {
        T *t;
    };
    metadata md;
    std::experimental::optional<D> d_from_ctor;  // D might not be default-constructible
  public:
    indirect() { md.t = nullptr; }
    explicit indirect(T *p) { md.t = p; }
    explicit indirect(T *p, D d) { md.t = p; d_from_ctor.emplace(std::move(d)); }
    T* get() const { return md.t; }
    void retire(D d) {
        md.retire(detail::indirect_deleter<D>(std::move(d)));
    }
    void retire() {
        if (d_from_ctor) {
            md.retire(detail::indirect_deleter<D>(*std::move(d_from_ctor)));
        } else {
            md.retire();
        }
    }
};

}} // namespace std::rcu
