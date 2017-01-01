#include <iostream>
#include "urcu-signal.hpp"

class rcu_scoped_reader {
public:
    rcu_scoped_reader() noexcept
    {
        this->rd = nullptr;
        ::rcu_read_lock();
    }

    explicit rcu_scoped_reader(std::rcu_domain *rd)
    {
        this->rd = rd;
        rd->read_lock();
    }

    rcu_scoped_reader(const rcu_scoped_reader&) = delete;
    rcu_scoped_reader& operator=(const rcu_scoped_reader&) = delete;

    ~rcu_scoped_reader() noexcept
    {
        if (this->rd)
            this->rd->read_unlock();
        else
            ::rcu_read_unlock();
    }

private:
    std::rcu_domain *rd;
};

int main()
{
    std::rcu_signal rs;
    rcu_register_thread();
    {
            rcu_scoped_reader rr;
    }
    {
            rcu_scoped_reader rrs(&rs);
    }
    rcu_unregister_thread();

    return 0;
}
