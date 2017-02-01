#pragma once

extern "C" struct rcu_head;

namespace std {
    class rcu_domain {
    public:
	constexpr explicit rcu_domain() noexcept { };
	rcu_domain(const rcu_domain&) = delete;
	rcu_domain(rcu_domain&&) = delete;
	rcu_domain& operator=(const rcu_domain&) = delete;
	rcu_domain& operator=(rcu_domain&&) = delete;
        virtual void register_thread() = 0;
        virtual void unregister_thread() = 0;
        virtual void read_lock() noexcept = 0;
        virtual void read_unlock() noexcept = 0;
        virtual void synchronize() noexcept = 0;
        virtual void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp)) = 0;
        virtual void barrier() noexcept = 0;
        virtual void quiescent_state() noexcept = 0;
        virtual void thread_offline() noexcept = 0;
        virtual void thread_online() noexcept = 0;

        static bool register_thread_needed() { return true; }
    };
} // namespace std
