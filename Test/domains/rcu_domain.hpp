#pragma once

extern "C" struct rcu_head;

namespace std {
namespace rcu {
    // See Lawrence Crowl's P0260 "C++ Concurrent Queues" for this API's rationale.
    // Basically, there is an implicit concept RcuDomain that is satisfied by any
    // domain providing these functions; rcu_domain_base is a reification of that
    // concept into a classical polymorphic class; rcu_domain_wrapper<D> is a
    // classical polymorphic class derived from rcu_domain_base which implements
    // the same semantics as class D (which must satisfy the RcuDomain concept).
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0260r0.html#Binary
    //
    class rcu_domain_base {
    public:
	rcu_domain_base() noexcept = default;
	rcu_domain_base(const rcu_domain_base&) = delete;
	virtual ~rcu_domain_base() = default;

	virtual bool register_thread_needed() const noexcept = 0;
	virtual void register_thread() = 0;
	virtual void unregister_thread() = 0;
	virtual void thread_offline() noexcept = 0;
	virtual void thread_online() noexcept = 0;

	virtual bool quiescent_state_needed() const noexcept = 0;
	virtual void quiescent_state() noexcept = 0;

	virtual void read_lock() noexcept = 0;
	virtual void read_unlock() noexcept = 0;

	virtual void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp)) = 0;

	virtual void synchronize() noexcept = 0;
	virtual void barrier() noexcept = 0;
    };

    template<class Domain>
    class rcu_domain_wrapper : public virtual rcu_domain_base {
	Domain *d;
    public:
	rcu_domain_wrapper(Domain& d) noexcept : d(&d) {}

	bool register_thread_needed() const noexcept override { return d->register_thread_needed(); }
	void register_thread() override { d->register_thread(); }
	void unregister_thread() override { d->unregister_thread(); }
	void thread_offline() noexcept override { d->thread_offline(); }
	void thread_online() noexcept override { d->thread_online(); }

	bool quiescent_state_needed() const noexcept override { return d->quiescent_state_needed(); }
	void quiescent_state() noexcept override { d->quiescent_state(); }

	void read_lock() noexcept override { d->read_lock(); }
	void read_unlock() noexcept override { d->read_unlock(); }

	void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp)) override { d->retire(rhp, cbf); }

	void synchronize() noexcept override { d->synchronize(); }
	void barrier() noexcept override { d->barrier(); }
    };
} // namespace rcu
} // namespace std
