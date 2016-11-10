#include <cstddef>

// Added-pointer approach

template<typename T>
class rcu_head_funcptr: public rcu_head {
	std::function<F> func;

	static void trampoline(rcu_head *rhp)
	{
		this->func();
	}

public:

	void call(std::function<F> func)
	{
		this->func = func;
		call_rcu(static_cast<rcu_head *>(this), trampoline);
	}

	void call(std::function<F> func, class rcu_domain &rd)
	{
		this->func = func;
		rd.call(static_cast<rcu_head *>(this), trampoline);
	}
};
