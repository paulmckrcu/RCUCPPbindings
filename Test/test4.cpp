#include "urcu-signal.hpp"
#include "rcu_guard.hpp"

int main()
{
    rcu_domain_signal rs;
    rcu_register_thread();
    {
            rcu_guard<rcu_domain_signal> rr;
    }
    {
            rcu_guard<rcu_domain_signal> rrs(rs);
    }
    rcu_unregister_thread();

    return 0;
}
