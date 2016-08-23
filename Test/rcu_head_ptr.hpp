#include <cstddef>

// Added-pointer approach

namespace std {
	template<typename T>
	class rcu_head_ptr: public rcu_head {
	public:
		rcu_head_ptr()
		{
			this->container_ptr = NULL;
		}

		rcu_head_ptr(T *containing_class)
		{
			this->container_ptr = containing_class;
		}

		T *enclosing_class()
		{
			return this->container_ptr;
		}

		class rcu_head_ptr<T> *
		rcu_head_to_rcu_head_ptr(struct rcu_head *rhp)
		{
			return static_cast<rcu_head_ptr<T> *>(rhp);
		}

	private:
		T *container_ptr;
	};
}
