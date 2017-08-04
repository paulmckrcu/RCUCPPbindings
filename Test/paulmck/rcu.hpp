#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <tuple>
#include <utility>
#include <boost/compressed_pair.hpp>

// Derived-type approach.  All RCU-protected data structures using this
// approach must derive from std::rcu_obj_base, which in turn derives
// from std::rcu_head.  No idea what happens in case of multiple inheritance.

namespace std {

    namespace detail {
        struct rcu_obj_base_empty_type {};
    }

    template<typename T, typename D = default_delete<T>>
    class rcu_obj_base:
        private rcu_head,
        private boost::compressed_pair<D, detail::rcu_obj_base_empty_type> {
    public:
        void retire(D d = {})
        {
            this->first() = std::move(d);
            ::call_rcu(
                static_cast<rcu_head *>(this),
                [](rcu_head *rhp) {
                    auto rhdp = static_cast<rcu_obj_base *>(rhp);
                    auto obj = static_cast<T *>(rhdp);
                    rhdp->first()(obj);
                }
            );
        }
    };

    // RAII for RCU readers
    class rcu_reader {
    public:
        rcu_reader() noexcept
        {
            rcu_read_lock();
            active = true;
        }
        rcu_reader(std::defer_lock_t) noexcept
        {
            active = false;
        }
        rcu_reader(const rcu_reader &) = delete;
        rcu_reader(rcu_reader&& other) noexcept
        {
            active = other.active;
            other.active = false;
        }
        rcu_reader& operator=(const rcu_reader&) = delete;
        rcu_reader& operator=(rcu_reader&& other) noexcept
        {
            if (active) {
                rcu_read_unlock();
            }
            active = other.active;
            other.active = false;
            return *this;
        }

        ~rcu_reader() noexcept
        {
            if (active) {
                rcu_read_unlock();
            }
        }

        void swap(rcu_reader& other) noexcept
        {
            std::swap(active, other.active);
        }

        void lock() noexcept
        {
            rcu_read_lock();
            active = true;
        }

        void unlock() noexcept
        {
            rcu_read_unlock();
            active = false;
        }

    private:
        bool active;
    };

    void swap(rcu_reader& a, rcu_reader& b) noexcept
    {
        a.swap(b);
    }

    // Free functions for RCU updaters
    static void synchronize_rcu() noexcept
    {
	::synchronize_rcu();
    }

    static void rcu_barrier() noexcept
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
