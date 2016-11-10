#include <cstddef>
#include <memory>
#include <utility>

// Derived-type approach.  All RCU-protected data structures using this
// approach must derive from std::rcu_head_delete, which in turn derives
// from std::rcu_head.  No idea what happens in case of multiple inheritance.

template<typename T, typename D = std::default_delete<T>, bool E = std::is_empty<D>::value>
class rcu_head_delete: private rcu_head {
	D deleter;
public:
	static void trampoline(rcu_head *rhp)
	{
		auto rhdp = static_cast<rcu_head_delete *>(rhp);
		auto obj = static_cast<T *>(rhdp);
		rhdp->deleter(obj);
	}

	void call(D d = {})
	{
		deleter = d;
		call_rcu(static_cast<rcu_head *>(this), trampoline);
	}

	void call(rcu_domain &rd, D d = {})
	{
		deleter = d;
		rd.call(static_cast<rcu_head *>(this), trampoline);
	}
};

template<typename T, typename D>
class rcu_head_delete<T,D,true>: private rcu_head {
public:
	static void trampoline(rcu_head *rhp)
	{
		auto rhdp = static_cast<rcu_head_delete *>(rhp);
		auto obj = static_cast<T *>(rhdp);
		D()(obj);
	}

	void call(D d = {})
	{
		call_rcu(static_cast<rcu_head *>(this), trampoline);
	}

	void call(rcu_domain &rd, D d = {})
	{
		rd.call(static_cast<rcu_head *>(this), trampoline);
	}
};
