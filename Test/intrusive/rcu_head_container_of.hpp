#include <cstddef>

// container_of() approach.

namespace std {
    template<typename T>
    class rcu_head_container_of {
    public:
        static void set_field(const rcu_head T::*rh_field)
        {
            T t;
            T *p = &t;

            rh_offset = ((char *)&(p->*rh_field)) - (char *)p;
        }

        static T *enclosing_class(rcu_head *rhp)
        {
            return (T *)((char *)rhp - rh_offset);
        }

    private:
        static size_t rh_offset;
    };

    template<typename T>
    size_t rcu_head_container_of<T>::rh_offset;
}
