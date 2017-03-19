#pragma once

#include <type_traits>
#include <utility> // std::forward
#include <memory> // std::default_delete

#include "urcu-signal.hpp"

namespace std { namespace experimental {

//==============================================================================
// Defined by the basic "C-style" interface
// (Paul's code would go here)

using rcu_head = ::rcu_head;

void call_rcu(
  rcu_head *_Rhp,
  void (*_Cb)(rcu_head*)
) { ::call_rcu(_Rhp, _Cb); }

//==============================================================================

// for use with SFINAE
struct __nat {};

//==============================================================================
// Type trait for detecting RCUDomain objects

template <typename _T>
struct is_rcu_domain {
  // for now, just use is_base_of...
  static constexpr bool value = ::std::is_base_of<
    ::std::rcu::rcu_domain_base, _T
  >::value;
};

template <typename _T>
constexpr bool is_rcu_domain_v = is_rcu_domain<_T>::value;


//==============================================================================
// __rcu_head_impl and base class

struct __rcu_head_impl_base: rcu_head {
  void* _M_ptr;
  virtual void __do_callback() =0;
  virtual ~__rcu_head_impl_base() = default;
  __rcu_head_impl_base(void* _Ptr): _M_ptr(_Ptr) { }
};

template <typename _Tp, typename _UnaryOperation>
struct __rcu_head_impl: __rcu_head_impl_base {

  __rcu_head_impl(_Tp* _Ptr, _UnaryOperation&& _Op)
    : __rcu_head_impl_base(_Ptr),
      _M_op(::std::forward<_UnaryOperation>(_Op))
  { }

  _UnaryOperation _M_op;

  void __do_callback() override {
    _M_op(static_cast<_Tp*>(_M_ptr));
  }

  ~__rcu_head_impl() = default;
};


//==============================================================================
// Non-intrusive rcu_ptr implementation analogous to std::shared_ptr

template <typename T, typename _DefaultUnaryOperation=::std::default_delete<T>>
class rcu_ptr {
  private:
    __rcu_head_impl_base* _M_head;

    static void __trampoline(rcu_head* rhp) {
      auto* base_ptr = static_cast<__rcu_head_impl_base*>(rhp);
      base_ptr->__do_callback();
    }

  public:

    //==========================================================================
    // retire() overloads

    void retire() {
      ::std::experimental::call_rcu(_M_head, __trampoline);
    }

    template <typename _UnaryOperation>
    void retire(_UnaryOperation&& _Op) {
      auto* _M_ptr = static_cast<T*>(_M_head->_M_ptr);
      delete _M_head;
      _M_head = new __rcu_head_impl<T, _UnaryOperation>(
        _M_ptr,
        ::std::forward<_UnaryOperation>(_Op)
      );
      ::std::experimental::call_rcu(_M_head, rcu_ptr::__trampoline);
    }

    template <
      typename _RCUDomain,
      typename _UnaryOperation
    >
    ::std::enable_if_t<
      is_rcu_domain_v<_RCUDomain>
    >
    retire(
      _RCUDomain&& _Dom,
      _UnaryOperation&& _Op
    ) {
      auto* _M_ptr = static_cast<T*>(_M_head->_M_ptr);
      delete _M_head;
      _M_head = new __rcu_head_impl<T, _UnaryOperation>(
        _M_ptr,
        ::std::forward<_UnaryOperation>(_Op)
      );
      ::std::forward<_RCUDomain>(_Dom).retire(_M_head, rcu_ptr::__trampoline);
    }

    template <
      typename _RCUDomain
    >
    ::std::enable_if_t<
      is_rcu_domain_v<_RCUDomain>
    >
    retire(
      _RCUDomain&& _Dom
    ) {
      ::std::forward<_RCUDomain>(_Dom).retire(_M_head, rcu_ptr::__trampoline);
    }

    //==========================================================================
    // Ctors

    // Default and nullptr_t constructors
    rcu_ptr(::std::nullptr_t _Ptr = nullptr)
      : _M_head(
          new __rcu_head_impl<T, _DefaultUnaryOperation>(_DefaultUnaryOperation{})
        )
    { }

    // Pointer wrapping constructor
    template <typename _Up>
    rcu_ptr(_Up* _Ptr,
      ::std::enable_if_t<::std::is_convertible<_Up*, T*>::value, __nat> = __nat{}
    ): _M_head(
          new __rcu_head_impl<T, _DefaultUnaryOperation>(
            _Ptr, _DefaultUnaryOperation{}
          )
        )
    { }

    // Pointer wrapping constructor with custom unary op
    template <typename _Up, typename _UnaryOperation>
    rcu_ptr(_Up* _Ptr,
      _UnaryOperation&& _Op,
      ::std::enable_if_t<::std::is_convertible<_Up*, T*>::value, __nat> = __nat{}
    ): _M_head(
          new __rcu_head_impl<T, _UnaryOperation>(
            _Ptr, ::std::forward<_UnaryOperation>(_Op)
          )
        )
    { }

    //==========================================================================
    // Destructor

    ~rcu_ptr() { delete _M_head; }

};

}} // end namespace std::experimental
