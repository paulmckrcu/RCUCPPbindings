#pragma once

extern "C" struct rcu_head;

namespace std {
    class rcu_flavor_base {
    public:
	typedef int cookie_t;
	constexpr explicit rcu_flavor_base() noexcept { };
	rcu_flavor_base(const rcu_flavor_base&) = delete;
	rcu_flavor_base(rcu_flavor_base&&) = delete;
	rcu_flavor_base& operator=(const rcu_flavor_base&) = delete;
	rcu_flavor_base& operator=(rcu_flavor_base&&) = delete;
	void register_thread(); // Call from library thread create
	void unregister_thread(); // Call from library thread destroy
	void quiescent_state() noexcept;
	void thread_offline() noexcept;
	void thread_online() noexcept;
	static constexpr bool quiescent_state_needed() { return false; }
	cookie_t read_lock() noexcept;
	void read_unlock(cookie_t c) noexcept;
	void synchronize() noexcept;
	void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp));
	void barrier() noexcept;
    };
} // namespace std
