// Common code for userspace RCU concrete class definition.

public:
	void register_thread()
	{
		rcu_register_thread();
	}

	void unregister_thread()
	{
		rcu_unregister_thread();
	}

	void read_lock() noexcept
	{
		rcu_read_lock();
	}

	void read_unlock() noexcept
	{
		rcu_read_unlock();
	}

	void synchronize() noexcept
	{
		synchronize_rcu();
	}

	void call(class rcu_head *rhp,
		  void cbf(class rcu_head *rhp))
	{
		call_rcu(rhp, cbf);
	}
