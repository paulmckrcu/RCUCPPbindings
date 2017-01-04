#include <cstddef>

// Added-pointer approach

namespace std {
	template<typename T>
	class rcu_head_funcptr: public rcu_head {
		std::function<F> func;

		static void trampoline(rcu_head *rhp)
		{
			this->func();
		}

	public:

		void retire(std::function<F> func)
		{
			this->func = func;
			call_rcu(static_cast<rcu_head *>(this), trampoline);
		}

		void retire(std::function<F> func, class rcu_domain &rd)
		{
			this->func = func;
			rd.retire(static_cast<rcu_head *>(this), trampoline);
		}
	};
}
