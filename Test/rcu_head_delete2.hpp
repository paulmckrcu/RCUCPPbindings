#include <cstddef>
#include <tuple>
#include <memory>

// Derived-type approach, according to Isabella Muerte.

template <class T>
struct default_deleter {
	void operator () (T* ptr) const noexcept { delete ptr; }
};

template<class T, class Deleter=default_deleter<T>>
struct rcu_head_delete2: rcu_head, Deleter {

	Deleter& get_deleter () { return *this; }

	void call () {
		call_rcu(this, +[] (rcu_head * rhp) {
			auto self = static_cast<T*>(rhp);
			self->get_deleter()(self);
		});
	}

	void call (rcu_domain& rd) {
		rd.call(this, +[] (rcu_head * rhp) {
			auto self = static_cast<T*>(rhp);
			self->get_deleter()(self);
		});
	}
};
