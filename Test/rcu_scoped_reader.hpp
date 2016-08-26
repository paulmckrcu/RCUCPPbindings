namespace std {
	class rcu_scoped_reader {
	public:
		rcu_scoped_reader()
		{
			this->rd = nullptr;
			rcu_read_lock();
		}

		rcu_scoped_reader(class rcu_domain *rd)
		{
			this->rd = rd;
			rd->read_lock();
		}

		~rcu_scoped_reader()
		{
			if (this->rd)
				this->rd->read_unlock();
			else
				rcu_read_unlock();
		}

	private:
		class rcu_domain *rd;
	};
}
