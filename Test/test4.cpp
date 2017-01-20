#include <iostream>
#include "urcu-signal.hpp"
#include "rcu_guard.hpp"

int main()
{
    std::rcu_signal rs;
    rcu_register_thread();
    {
            rcu_guard rr;
    }
    {
            rcu_guard rrs(&rs);
    }
    rcu_unregister_thread();

    return 0;
}
