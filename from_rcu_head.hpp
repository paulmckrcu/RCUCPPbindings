#include <cstddef>

namespace std {
	template<typename T>
	class from_rcu_head {
	public:
		from_rcu_head(const struct rcu_head T::*rh_field)
		{
			T t;
			T *p = &t;

			this->rh_offset = ((char *)&(p->*rh_field)) - (char *)p;
		}

		T *enclosing_class(struct rcu_head *rhp)
		{
			return (T *)((char *)rhp - this->rh_offset);
		}

	private:
		size_t rh_offset;
	};
}
