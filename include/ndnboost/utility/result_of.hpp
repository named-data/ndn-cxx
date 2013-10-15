// Boost result_of library

//  Copyright Douglas Gregor 2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org/libs/utility
#ifndef NDNBOOST_RESULT_OF_HPP
#define NDNBOOST_RESULT_OF_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/iteration/iterate.hpp>
#include <ndnboost/preprocessor/repetition/enum_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_binary_params.hpp>
#include <ndnboost/preprocessor/repetition/enum_shifted_params.hpp>
#include <ndnboost/preprocessor/facilities/intercept.hpp>
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/mpl/has_xxx.hpp>
#include <ndnboost/mpl/if.hpp>
#include <ndnboost/mpl/eval_if.hpp>
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/mpl/identity.hpp>
#include <ndnboost/mpl/or.hpp>
#include <ndnboost/type_traits/is_class.hpp>
#include <ndnboost/type_traits/is_pointer.hpp>
#include <ndnboost/type_traits/is_member_function_pointer.hpp>
#include <ndnboost/type_traits/remove_cv.hpp>
#include <ndnboost/type_traits/remove_reference.hpp>
#include <ndnboost/utility/declval.hpp>
#include <ndnboost/utility/enable_if.hpp>

#ifndef NDNBOOST_RESULT_OF_NUM_ARGS
#  define NDNBOOST_RESULT_OF_NUM_ARGS 16
#endif

// Use the decltype-based version of result_of by default if the compiler
// supports N3276 <http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2011/n3276.pdf>.
// The user can force the choice by defining either NDNBOOST_RESULT_OF_USE_DECLTYPE or
// NDNBOOST_RESULT_OF_USE_TR1, but not both!
#if defined(NDNBOOST_RESULT_OF_USE_DECLTYPE) && defined(NDNBOOST_RESULT_OF_USE_TR1)
#  error Both NDNBOOST_RESULT_OF_USE_DECLTYPE and NDNBOOST_RESULT_OF_USE_TR1 cannot be defined at the same time.
#endif

#ifndef NDNBOOST_RESULT_OF_USE_TR1
#  ifndef NDNBOOST_RESULT_OF_USE_DECLTYPE
#    ifndef NDNBOOST_NO_CXX11_DECLTYPE_N3276 // this implies !defined(NDNBOOST_NO_CXX11_DECLTYPE)
#      define NDNBOOST_RESULT_OF_USE_DECLTYPE
#    else
#      define NDNBOOST_RESULT_OF_USE_TR1
#    endif
#  endif
#endif

namespace ndnboost {

template<typename F> struct result_of;
template<typename F> struct tr1_result_of; // a TR1-style implementation of result_of

#if !defined(NDNBOOST_NO_SFINAE) && !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
namespace detail {

NDNBOOST_MPL_HAS_XXX_TRAIT_DEF(result_type)

template<typename F, typename FArgs, bool HasResultType> struct tr1_result_of_impl;

#ifdef NDNBOOST_NO_SFINAE_EXPR

// There doesn't seem to be any other way to turn this off such that the presence of
// the user-defined operator,() below doesn't cause spurious warning all over the place,
// so unconditionally turn it off.
#if NDNBOOST_MSVC
#  pragma warning(disable: 4913) // user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used
#endif

struct result_of_private_type {};

struct result_of_weird_type {
  friend result_of_private_type operator,(result_of_private_type, result_of_weird_type);
};

typedef char result_of_yes_type;      // sizeof(result_of_yes_type) == 1
typedef char (&result_of_no_type)[2]; // sizeof(result_of_no_type)  == 2

template<typename T>
result_of_no_type result_of_is_private_type(T const &);
result_of_yes_type result_of_is_private_type(result_of_private_type);

template<typename C>
struct result_of_callable_class : C {
    result_of_callable_class();
    typedef result_of_private_type const &(*pfn_t)(...);
    operator pfn_t() const volatile;
};

template<typename C>
struct result_of_wrap_callable_class {
  typedef result_of_callable_class<C> type;
};

template<typename C>
struct result_of_wrap_callable_class<C const> {
  typedef result_of_callable_class<C> const type;
};

template<typename C>
struct result_of_wrap_callable_class<C volatile> {
  typedef result_of_callable_class<C> volatile type;
};

template<typename C>
struct result_of_wrap_callable_class<C const volatile> {
  typedef result_of_callable_class<C> const volatile type;
};

template<typename C>
struct result_of_wrap_callable_class<C &> {
  typedef typename result_of_wrap_callable_class<C>::type &type;
};

template<typename F, bool TestCallability = true> struct cpp0x_result_of_impl;

#else // NDNBOOST_NO_SFINAE_EXPR

template<typename T>
struct result_of_always_void
{
  typedef void type;
};

template<typename F, typename Enable = void> struct cpp0x_result_of_impl {};

#endif // NDNBOOST_NO_SFINAE_EXPR

template<typename F>
struct result_of_void_impl
{
  typedef void type;
};

template<typename R>
struct result_of_void_impl<R (*)(void)>
{
  typedef R type;
};

template<typename R>
struct result_of_void_impl<R (&)(void)>
{
  typedef R type;
};

// Determine the return type of a function pointer or pointer to member.
template<typename F, typename FArgs>
struct result_of_pointer
  : tr1_result_of_impl<typename remove_cv<F>::type, FArgs, false> { };

template<typename F, typename FArgs>
struct tr1_result_of_impl<F, FArgs, true>
{
  typedef typename F::result_type type;
};

template<typename FArgs>
struct is_function_with_no_args : mpl::false_ {};

template<typename F>
struct is_function_with_no_args<F(void)> : mpl::true_ {};

template<typename F, typename FArgs>
struct result_of_nested_result : F::template result<FArgs>
{};

template<typename F, typename FArgs>
struct tr1_result_of_impl<F, FArgs, false>
  : mpl::if_<is_function_with_no_args<FArgs>,
             result_of_void_impl<F>,
             result_of_nested_result<F, FArgs> >::type
{};

} // end namespace detail

#define NDNBOOST_PP_ITERATION_PARAMS_1 (3,(0,NDNBOOST_RESULT_OF_NUM_ARGS,<ndnboost/utility/detail/result_of_iterate.hpp>))
#include NDNBOOST_PP_ITERATE()

#else
#  define NDNBOOST_NO_RESULT_OF 1
#endif

}

#endif // NDNBOOST_RESULT_OF_HPP
