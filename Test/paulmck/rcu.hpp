#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <utility>

// Derived-type approach.  All RCU-protected data structures using this
// approach must derive from std::rcu_obj_base, which in turn derives
// from std::rcu_head.  No idea what happens in case of multiple inheritance.

namespace std {
    template<typename T, typename D = default_delete<T>, bool E = is_empty<D>::value>
    class rcu_obj_base: private rcu_head {
        D deleter;
    public:
        void retire(D d = {}) noexcept
        {
            deleter = std::move(d);
            ::call_rcu(
                static_cast<rcu_head *>(this),
                [](rcu_head *rhp) {
                    auto rhdp = static_cast<rcu_obj_base *>(rhp);
                    auto obj = static_cast<T *>(rhdp);
                    rhdp->deleter(obj);
                }
            );
        }
    };


    // Specialization for when D is an empty type.

    template<typename T, typename D>
    class rcu_obj_base<T,D,true>: private rcu_head {
    public:
        void retire(D = {}) noexcept
        {
            ::call_rcu(
                static_cast<rcu_head *>(this),
                [](rcu_head *rhp) {
                    auto rhdp = static_cast<rcu_obj_base *>(rhp);
                    auto obj = static_cast<T *>(rhdp);
                    D()(obj);
                }
            );
        }
    };

    // RAII for RCU readers
    class rcu_domain {
    public:
        rcu_domain() {}
        rcu_domain(const rcu_domain&) = delete;
        rcu_domain& operator=(const rcu_domain&) = delete;

        void lock() noexcept
        {
            rcu_read_lock();
        }

        void unlock() noexcept
        {
            rcu_read_unlock();
        }
    };

    class rcu_domain the_rcu_default_domain;

    rcu_domain& rcu_default_domain() noexcept
    {
        return the_rcu_default_domain;
    }

    // Free functions for RCU updaters
    void rcu_synchronize() noexcept
    {
	::synchronize_rcu();
    }

    void rcu_barrier() noexcept
    {
	::rcu_barrier();
    }

    namespace details {
	template<typename T, typename D = default_delete<T>>
	class rcu_obj_base_ni: public rcu_head {
	public:
	    rcu_obj_base_ni(T *pi, D di = {}) {
		p = pi;
		d = di;
	    }
	    T *p;
	    D d;
	};
    }

    template<typename T, typename D = default_delete<T>>
    void rcu_retire(T *p, D d = {})
    {
	auto robnp = new details::rcu_obj_base_ni<T, D>(p, d);

	::call_rcu(
	    static_cast<rcu_head *>(robnp),
	    [](rcu_head *rhp) {
		auto robnp2 = static_cast<details::rcu_obj_base_ni<T, D> *>(rhp);

		robnp2->d(robnp2->p);
		delete robnp2;
	    });
    }

} // namespace std
