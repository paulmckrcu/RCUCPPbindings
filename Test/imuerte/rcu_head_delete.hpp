#pragma once

#include <cstddef>
#include <memory>

// Derived-type approach, according to Isabella Muerte.

namespace std {
    template <class T>
    struct default_deleter {
        void operator () (T* ptr) const noexcept { delete ptr; }
    };

    template<class T, class Deleter=default_deleter<T>>
    struct rcu_head_delete: rcu_head, Deleter {

        Deleter& get_deleter () { return *this; }

        void call () {
            call_rcu(this, +[] (rcu_head * rhp) {
                auto self = static_cast<T*>(rhp);
                self->get_deleter()(self);
            });
        }

        void call (rcu_domain& rd) {
            rd.call(this, +[] (rcu_head * rhp) {
                auto self = static_cast<T*>(rhp);
                self->get_deleter()(self);
            });
        }
    };
}
