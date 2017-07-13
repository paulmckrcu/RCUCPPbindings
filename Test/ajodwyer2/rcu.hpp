#pragma once

#include <cstddef>
#include <memory>
#include <utility>

// Derived-type approach.  All RCU-protected data structures using this
// approach must derive from std::rcu_obj_base, which in turn derives
// from std::rcu_head.  No idea what happens in case of multiple inheritance.

namespace std {
    template<typename T, typename D = default_delete<T>, bool E = is_empty<D>::value>
    class rcu_obj_base: private rcu_head {
        D deleter;
    public:
        void retire(D d = {})
        {
            deleter = std::move(d);
            ::call_rcu(static_cast<rcu_head *>(this),
	    	       [](rcu_head *rhp)
		       {
			    auto rhdp = static_cast<rcu_obj_base *>(rhp);
			    auto obj = static_cast<T *>(rhdp);
			    rhdp->deleter(obj);
		       });
        }
    };


    // Specialization for when D is an empty type.

    template<typename T, typename D>
    class rcu_obj_base<T,D,true>: private rcu_head {
    public:
        void retire(D = {})
        {
            ::call_rcu(static_cast<rcu_head *>(this),
	    	       [](rcu_head *rhp)
		       {
			    auto rhdp = static_cast<rcu_obj_base *>(rhp);
			    auto obj = static_cast<T *>(rhdp);
			    D()(obj);
		       });
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
	rcu_reader(std::nullptr_t)
	{
	    active = false;
	}
	rcu_reader(const rcu_reader &) = delete;
	rcu_reader(rcu_reader &&other) noexcept
	{
	    active = other.active;
	    other.active = false;
	}
	rcu_reader&operator=(const rcu_reader &) = delete;
	rcu_reader&operator=(rcu_reader &&other)
	{
	    if (this != &other) {
		other.active = false;
		active = true;
	    }
	}
	~rcu_reader() noexcept
	{
	    if (active)
		    rcu_read_unlock();
	}

    private:
	bool active;
    };

} // namespace std
