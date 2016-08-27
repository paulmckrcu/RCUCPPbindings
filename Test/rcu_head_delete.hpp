#include <cstddef>

// Derived-type approach.  All RCU-protected data structures using this
// approach must derive from std::rcu_head_delete, which in turn derives
// from std::rcu_head.  No idea what happens in case of multiple inheritance.

namespace std {
	template<typename T>
	class rcu_head_delete: public rcu_head {
	public:
		static void trampoline(rcu_head *rhp)
		{
			T *obj;
			rcu_head_delete<T> *rhdp;

			rhdp = static_cast<rcu_head_delete<T> *>(rhp);
			obj = static_cast<T *>(rhdp);
			delete obj;
		}

		void call()
		{
			call_rcu(static_cast<rcu_head *>(this), trampoline);
		}

		void call(class rcu_domain &rd)
		{
			rd.call(static_cast<rcu_head *>(this), trampoline);
		}
	};
}
