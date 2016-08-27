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

		class rcu_head_ptr<T> *
		rcu_head_to_rcu_head_ptr(struct rcu_head *rhp)
		{
			return static_cast<rcu_head_ptr<T> *>(rhp);
		}

		static void trampoline(rcu_head *rhp)
		{
			T *obj;
			rcu_head_ptr<T> *rhdp;

			rhdp = static_cast<rcu_head_ptr<T> *>(rhp);
			obj = rhdp->container_ptr;
			if (rhdp->callback_func)
				rhdp->callback_func(obj);
			else
				delete obj;
		}

		void call()
		{
			this->callback_func = nullptr;
			call_rcu(static_cast<rcu_head *>(this), trampoline);
		}

		void call(class rcu_domain &rd)
		{
			this->callback_func = nullptr;
			rd.call(static_cast<rcu_head *>(this), trampoline);
		}

		void call(void callback_func(T *obj))
		{
			this->callback_func = callback_func;
			call_rcu(static_cast<rcu_head *>(this), trampoline);
		}

		void call(void callback_func(T *obj), class rcu_domain &rd)
		{
			this->callback_func = callback_func;
			rd.call(static_cast<rcu_head *>(this), trampoline);
		}

	private:
		void (*callback_func)(T *obj);
		T *container_ptr;
	};
}
