namespace std {
	class rcu_scoped_reader {
	public:
		rcu_scoped_reader() noexcept
		{
			this->rd = nullptr;
			rcu_read_lock();
		}

		explicit rcu_scoped_reader(rcu_domain *rd)
		{
			this->rd = rd;
			rd->read_lock();
		}

		rcu_scoped_reader(const rcu_scoped_reader &) = delete;
		
		rcu_scoped_reader&operator=(const rcu_scoped_reader &) = delete;

		~rcu_scoped_reader() noexcept
		{
			if (this->rd)
				this->rd->read_unlock();
			else
				rcu_read_unlock();
		}

	private:
		rcu_domain *rd;
	};
}
