	class rcu_domain {
	public:
		void read_lock() noexcept;
		void read_unlock() noexcept;
		void synchronize() noexcept;
		void call(void cbf(class rcu_head *rhp), class rcu_head *rhp);
	};
