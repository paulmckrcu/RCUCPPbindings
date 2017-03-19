#pragma once

#include "rcu_domain.hpp"

#include <urcu-bp.h>

class rcu_domain_bp {
public:
    static constexpr bool register_thread_needed() { return false; }
    void register_thread() { rcu_register_thread(); }
    void unregister_thread() { rcu_unregister_thread(); }
    void thread_offline() noexcept { rcu_thread_offline(); }
    void thread_online() noexcept { rcu_thread_online(); }

    static constexpr bool quiescent_state_needed() { return false; }
    void quiescent_state() noexcept { rcu_quiescent_state(); }

    void read_lock() noexcept { rcu_read_lock(); }
    void read_unlock() noexcept { rcu_read_unlock(); }

    void retire(rcu_head *rhp, void (*cbf)(rcu_head *rhp)) { call_rcu(rhp, cbf); }

    void synchronize() noexcept { synchronize_rcu(); }
    void barrier() noexcept { rcu_barrier(); }
};
