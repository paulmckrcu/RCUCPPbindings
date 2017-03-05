#pragma once

#include "rcu_flavor_base.hpp"

#include <urcu-qsbr.h>
#define _LGPL_SOURCE

namespace std {
    class rcu_qsbr: public rcu_flavor_base {
public:
        void register_thread() { rcu_register_thread(); }
        void unregister_thread() { rcu_unregister_thread(); }
        cookie_t read_lock() noexcept { rcu_read_lock(); }
        void read_unlock(cookie_t c) noexcept { rcu_read_unlock(); }
        void synchronize() noexcept { synchronize_rcu(); }
        void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp)) { call_rcu(rhp, cbf); }
        void barrier() noexcept { rcu_barrier(); }
        void quiescent_state() noexcept { rcu_quiescent_state(); }
        void thread_offline() noexcept { rcu_thread_offline(); }
        void thread_online() noexcept { rcu_thread_online(); }
	static constexpr bool quiescent_state_needed() { return true; }
    };
} // namespace std
