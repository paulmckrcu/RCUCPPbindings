namespace std {
	class rcu_domain {
	public:
		virtual void register_thread() = 0;
		virtual void unregister_thread() = 0;
		virtual void read_lock() noexcept = 0;
		virtual void read_unlock() noexcept = 0;
		virtual void synchronize() noexcept = 0;
		virtual void call(class rcu_head *rhp,
				  void cbf(class rcu_head *rhp)) = 0;
		virtual void barrier() noexcept = 0;
	};
}
