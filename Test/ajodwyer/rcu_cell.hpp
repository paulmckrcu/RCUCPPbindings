#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include "rcu.hpp"

namespace std {
namespace rcu {

// Class template std::rcu::cell<T> is provided in the std namespace, but it
// can be implemented "user-side" if need be; it does not have to be part of an
// initial proposal. It implements a wrapper which is either empty or holds a
// single object of type T. Rather than providing direct access to the stored
// object, it allows the user to obtain a snapshot of the current state of
// the object by calling get_snapshot(). The get_snapshot() function is
// guaranteed to be concurrency-safe.

// get_snapshot() returns an object of type snapshot_ptr<T>, which is essentially
// unique_ptr<T> with a custom deleter and with some "leaky" member functions
// such as release() and get_deleter() quietly removed.

// A snapshot_ptr<T> either is null or points to a live object of type T, and it
// is null only if it has been constructed from nullptr, has been moved-from,
// or is the result of invoking get_snapshot() on an empty cell. In particular,
// a snapshot_ptr<T> cannot spontaneously become null due to the actions of other
// threads. The guarantee that the object is live means that calling get_snapshot()
// is equivalent to incrementing the reference count on the controlled object,
// and destroying the resulting snapshot_ptr<T> is equivalent to decrementing
// the reference count. The controlled object is retired when the reference count
// reaches zero.

// P0561R0 "An RAII Interface for Deferred Reclamation" (Geoff Romer and Andrew Hunter)
// describes these abstractions. It also proposes a type trait is_race_free_v<T>
// and a helper class cell_init<T>; I don't implement those things here because
// the former seems actively harmful and the latter is simply out of scope for
// the moment.

template <typename T, typename Alloc> class cell;
template <typename T> class snapshot_ptr;

namespace detail {
    template<class CB>
    struct cell_control_block_deleter {
        void operator()(CB *cb) {
            auto a = std::move(cb->a);
            using Alloc = decltype(a);
            delete cb->t;
            std::allocator_traits<Alloc>::deallocate(a, cb, 1);
        }
    };

    template<class T, class Alloc>
    struct cell_control_block : std::rcu_obj_base<cell_control_block<T, Alloc>, cell_control_block_deleter<cell_control_block<T, Alloc>>> {
        using cb_allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<cell_control_block>;
        T *t;
        cb_allocator a;
        explicit cell_control_block(T *t, Alloc a) : t(t), a(std::move(a)) {}
    };
} // namespace detail

template <typename T, typename Alloc = std::allocator<T>>
class cell {
    using control_block = detail::cell_control_block<T, Alloc>;
    using cb_allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<control_block>;
    using cb_pointer = typename std::allocator_traits<cb_allocator>::pointer;
    std::shared_ptr<control_block> cb;
    cb_allocator a;

    static_assert(std::is_same<
        typename std::allocator_traits<cb_allocator>::pointer,
        control_block *
    >::value, "cell<T,A> requires that A::rebind_alloc<CB>::pointer be exactly CB*");

  public:
    cell(cell&&) = delete;
    cell(const cell&) = delete;
    cell& operator=(cell&&) = delete;
    cell& operator=(const cell&) = delete;

    cell() : cb(nullptr), a(Alloc()) {}
    explicit cell(nullptr_t, Alloc alloc = Alloc()) : cb(nullptr), a(std::move(alloc)) {}
    explicit cell(std::unique_ptr<T> u, Alloc alloc = Alloc()) : a(std::move(alloc)) {
        control_block *new_cb = std::allocator_traits<cb_allocator>::allocate(a, 1);
        std::allocator_traits<cb_allocator>::construct(a, new_cb, u.release(), a);
        cb = std::shared_ptr<control_block>(new_cb, [](control_block *p) { p->retire(); });
    }

    void update(nullptr_t) {
        // "Update" the cell to become "empty", which means to abandon (retire) it.
        std::atomic_store(&cb, decltype(cb)(nullptr));
    }

    void update(unique_ptr<T> u) {
        // "Update" the cell to contain a new value, which means to allocate
        // a new cell and abandon (retire) the old one.
        if (u == nullptr) {
            this->update(nullptr);
        } else {
            control_block *new_cb = std::allocator_traits<cb_allocator>::allocate(a, 1);
            std::allocator_traits<cb_allocator>::construct(a, new_cb, u.release(), a);
            std::shared_ptr<control_block> sptr(new_cb, [](control_block *p) { p->retire(); });
            std::atomic_store(&cb, sptr);
        }
    }

    snapshot_ptr<T> get_snapshot() const {
        std::shared_ptr<control_block> old_cb = std::atomic_load(&cb);
        if (old_cb) {
            std::shared_ptr<T> ptr(old_cb, old_cb->t);
            return snapshot_ptr<T>(std::move(ptr));
        } else {
            return snapshot_ptr<T>(nullptr);
        }
    }
};

template <typename T>
class snapshot_ptr {
    friend class cell<T>;

    std::shared_ptr<T> ptr;

    explicit snapshot_ptr(std::shared_ptr<T> p) : ptr(std::move(p)) {}

  public:
    constexpr snapshot_ptr(snapshot_ptr&&) = default;
    snapshot_ptr& operator=(snapshot_ptr&&) = default;
    snapshot_ptr(const snapshot_ptr&) = delete;
    snapshot_ptr& operator=(const snapshot_ptr&) = delete;
    ~snapshot_ptr() = default;

    constexpr snapshot_ptr() = default;
    constexpr snapshot_ptr(nullptr_t) {}

    // Converting operations, enabled if U* is convertible to T*
    template <typename U, typename = std::enable_if_t<std::is_convertible<U*,T*>::value>>
    snapshot_ptr(snapshot_ptr<U>&& rhs) noexcept : ptr(std::move(rhs.ptr)) {}

    template <typename U, typename = std::enable_if_t<std::is_convertible<U*,T*>::value>>
    snapshot_ptr& operator=(snapshot_ptr<U>&& rhs) noexcept {
        ptr = std::move(rhs.ptr);
        return *this;
    }

    T* get() const noexcept { return ptr.get(); }
    T& operator*() const { return *ptr; }
    T* operator->() const noexcept { return ptr.get(); }

    explicit operator bool() const noexcept { return ptr; }
    operator std::shared_ptr<T>() && {
        return std::move(ptr);
    }

    void swap(snapshot_ptr& other) {
        using std::swap;
        swap(ptr, other.ptr);
    }
};

template <typename T>
void swap(snapshot_ptr<T>& lhs, snapshot_ptr<T>& rhs)
{
    lhs.swap(rhs);
}

template <typename T> bool operator==(const snapshot_ptr<T>& lhs, const snapshot_ptr<T>& rhs) noexcept { return lhs.get() == rhs.get(); }
template <typename T> bool operator==(const snapshot_ptr<T>& lhs, nullptr_t) noexcept { return lhs.get() == nullptr; }
template <typename T> bool operator==(nullptr_t, const snapshot_ptr<T>& rhs) noexcept { return rhs.get() == nullptr; }
template <typename T> bool operator!=(const snapshot_ptr<T>& lhs, const snapshot_ptr<T>& rhs) noexcept { return lhs.get() != rhs.get(); }
template <typename T> bool operator!=(const snapshot_ptr<T>& lhs, nullptr_t) noexcept { return lhs.get() != nullptr; }
template <typename T> bool operator!=(nullptr_t, const snapshot_ptr<T>& rhs) noexcept { return rhs.get() != nullptr; }
template <typename T> bool operator<(const snapshot_ptr<T>& lhs, const snapshot_ptr<T>& rhs) noexcept { return std::less<T*>(lhs.get(), rhs.get()); }
template <typename T> bool operator<(const snapshot_ptr<T>& lhs, nullptr_t) noexcept { return std::less<T*>(lhs.get(), nullptr); }
template <typename T> bool operator<(nullptr_t, const snapshot_ptr<T>& rhs) noexcept { return std::less<T*>(nullptr, rhs.get()); }
template <typename T> bool operator>(const snapshot_ptr<T>& lhs, const snapshot_ptr<T>& rhs) noexcept { return rhs < lhs; }
template <typename T> bool operator>(const snapshot_ptr<T>& lhs, nullptr_t rhs) noexcept { return rhs < lhs; }
template <typename T> bool operator>(nullptr_t lhs, const snapshot_ptr<T>& rhs) noexcept { return rhs < lhs; }
template <typename T> bool operator<=(const snapshot_ptr<T>& lhs, const snapshot_ptr<T>& rhs) noexcept { return !(rhs < lhs); }
template <typename T> bool operator<=(const snapshot_ptr<T>& lhs, nullptr_t rhs) noexcept { return !(rhs < lhs); }
template <typename T> bool operator<=(nullptr_t lhs, const snapshot_ptr<T>& rhs) noexcept { return !(rhs < lhs); }
template <typename T> bool operator>=(const snapshot_ptr<T>& lhs, const snapshot_ptr<T>& rhs) noexcept { return !(lhs < rhs); }
template <typename T> bool operator>=(const snapshot_ptr<T>& lhs, nullptr_t rhs) noexcept { return !(lhs < rhs); }
template <typename T> bool operator>=(nullptr_t lhs, const snapshot_ptr<T>& rhs) noexcept { return !(lhs < rhs); }

}} // namespace std::rcu


namespace std {

template <typename T>
struct hash<std::rcu::snapshot_ptr<T>>
{
    size_t operator()(const std::rcu::snapshot_ptr<T>& p) const noexcept { return std::hash<T*>()(p.get()); }
};

} // namespace std
