//  (C) Copyright 2009-2011 Frederic Bron, Robert Stewart, Steven Watanabe & Roman Perepelitsa.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#include <ndnboost/config.hpp>
#include <ndnboost/type_traits/ice.hpp>
#include <ndnboost/type_traits/integral_constant.hpp>
#include <ndnboost/type_traits/is_const.hpp>
#include <ndnboost/type_traits/is_fundamental.hpp>
#include <ndnboost/type_traits/is_integral.hpp>
#include <ndnboost/type_traits/is_pointer.hpp>
#include <ndnboost/type_traits/is_same.hpp>
#include <ndnboost/type_traits/is_void.hpp>
#include <ndnboost/type_traits/remove_cv.hpp>
#include <ndnboost/type_traits/remove_pointer.hpp>
#include <ndnboost/type_traits/remove_reference.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

// cannot include this header without getting warnings of the kind:
// gcc:
//    warning: value computed is not used
//    warning: comparison between signed and unsigned integer expressions
// msvc:
//    warning C4146: unary minus operator applied to unsigned type, result still unsigned
//    warning C4804: '-' : unsafe use of type 'bool' in operation
// cannot find another implementation -> declared as system header to suppress these warnings.
#if defined(__GNUC__) && ((__GNUC__==3 && __GNUC_MINOR__>=1) || (__GNUC__>3))
#   pragma GCC system_header
#elif defined(NDNBOOST_MSVC)
#   pragma warning ( push )
#   pragma warning ( disable : 4146 4804 4913 4244 )
#endif

namespace ndnboost {
namespace detail {

// This namespace ensures that argument-dependent name lookup does not mess things up.
namespace NDNBOOST_JOIN(NDNBOOST_TT_TRAIT_NAME,_impl) {

// 1. a function to have an instance of type T without requiring T to be default
// constructible
template <typename T> T &make();


// 2. we provide our operator definition for types that do not have one already

// a type returned from operator NDNBOOST_TT_TRAIT_OP when no such operator is
// found in the type's own namespace (our own operator is used) so that we have
// a means to know that our operator was used
struct no_operator { };

// this class allows implicit conversions and makes the following operator
// definition less-preferred than any other such operators that might be found
// via argument-dependent name lookup
struct any { template <class T> any(T const&); };

// when operator NDNBOOST_TT_TRAIT_OP is not available, this one is used
no_operator operator NDNBOOST_TT_TRAIT_OP (const any&);


// 3. checks if the operator returns void or not
// conditions: Rhs!=void

// we first redefine "operator," so that we have no compilation error if
// operator NDNBOOST_TT_TRAIT_OP returns void and we can use the return type of
// (NDNBOOST_TT_TRAIT_OP rhs, returns_void_t()) to deduce if
// operator NDNBOOST_TT_TRAIT_OP returns void or not:
// - operator NDNBOOST_TT_TRAIT_OP returns void   -> (NDNBOOST_TT_TRAIT_OP rhs, returns_void_t()) returns returns_void_t
// - operator NDNBOOST_TT_TRAIT_OP returns !=void -> (NDNBOOST_TT_TRAIT_OP rhs, returns_void_t()) returns int
struct returns_void_t { };
template <typename T> int operator,(const T&, returns_void_t);
template <typename T> int operator,(const volatile T&, returns_void_t);

// this intermediate trait has member value of type bool:
// - value==true -> operator NDNBOOST_TT_TRAIT_OP returns void
// - value==false -> operator NDNBOOST_TT_TRAIT_OP does not return void
template < typename Rhs >
struct operator_returns_void {
   // overloads of function returns_void make the difference
   // yes_type and no_type have different size by construction
   static ::ndnboost::type_traits::yes_type returns_void(returns_void_t);
   static ::ndnboost::type_traits::no_type returns_void(int);
   NDNBOOST_STATIC_CONSTANT(bool, value = (sizeof(::ndnboost::type_traits::yes_type)==sizeof(returns_void((NDNBOOST_TT_TRAIT_OP make<Rhs>(),returns_void_t())))));
};


// 4. checks if the return type is Ret or Ret==dont_care
// conditions: Rhs!=void

struct dont_care { };

template < typename Rhs, typename Ret, bool Returns_void >
struct operator_returns_Ret;

template < typename Rhs >
struct operator_returns_Ret < Rhs, dont_care, true > {
   NDNBOOST_STATIC_CONSTANT(bool, value = true);
};

template < typename Rhs >
struct operator_returns_Ret < Rhs, dont_care, false > {
   NDNBOOST_STATIC_CONSTANT(bool, value = true);
};

template < typename Rhs >
struct operator_returns_Ret < Rhs, void, true > {
   NDNBOOST_STATIC_CONSTANT(bool, value = true);
};

template < typename Rhs >
struct operator_returns_Ret < Rhs, void, false > {
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

template < typename Rhs, typename Ret >
struct operator_returns_Ret < Rhs, Ret, true > {
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

// otherwise checks if it is convertible to Ret using the sizeof trick
// based on overload resolution
// condition: Ret!=void and Ret!=dont_care and the operator does not return void
template < typename Rhs, typename Ret >
struct operator_returns_Ret < Rhs, Ret, false > {
   static ::ndnboost::type_traits::yes_type is_convertible_to_Ret(Ret); // this version is preferred for types convertible to Ret
   static ::ndnboost::type_traits::no_type is_convertible_to_Ret(...); // this version is used otherwise

   NDNBOOST_STATIC_CONSTANT(bool, value = (sizeof(is_convertible_to_Ret(NDNBOOST_TT_TRAIT_OP make<Rhs>()))==sizeof(::ndnboost::type_traits::yes_type)));
};


// 5. checks for operator existence
// condition: Rhs!=void

// checks if our definition of operator NDNBOOST_TT_TRAIT_OP is used or an other
// existing one;
// this is done with redefinition of "operator," that returns no_operator or has_operator
struct has_operator { };
no_operator operator,(no_operator, has_operator);

template < typename Rhs >
struct operator_exists {
   static ::ndnboost::type_traits::yes_type check(has_operator); // this version is preferred when operator exists
   static ::ndnboost::type_traits::no_type check(no_operator); // this version is used otherwise

   NDNBOOST_STATIC_CONSTANT(bool, value = (sizeof(check(((NDNBOOST_TT_TRAIT_OP make<Rhs>()),make<has_operator>())))==sizeof(::ndnboost::type_traits::yes_type)));
};


// 6. main trait: to avoid any compilation error, this class behaves
// differently when operator NDNBOOST_TT_TRAIT_OP(Rhs) is forbidden by the
// standard.
// Forbidden_if is a bool that is:
// - true when the operator NDNBOOST_TT_TRAIT_OP(Rhs) is forbidden by the standard
//   (would yield compilation error if used)
// - false otherwise
template < typename Rhs, typename Ret, bool Forbidden_if >
struct trait_impl1;

template < typename Rhs, typename Ret >
struct trait_impl1 < Rhs, Ret, true > {
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

template < typename Rhs, typename Ret >
struct trait_impl1 < Rhs, Ret, false > {
   NDNBOOST_STATIC_CONSTANT(bool,
      value = (
         ::ndnboost::type_traits::ice_and<
            operator_exists < Rhs >::value,
            operator_returns_Ret < Rhs, Ret, operator_returns_void < Rhs >::value >::value
         >::value
      )
   );
};

// specialization needs to be declared for the special void case
template < typename Ret >
struct trait_impl1 < void, Ret, false > {
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

// defines some typedef for convenience
template < typename Rhs, typename Ret >
struct trait_impl {
   typedef typename ::ndnboost::remove_reference<Rhs>::type Rhs_noref;
   typedef typename ::ndnboost::remove_cv<Rhs_noref>::type Rhs_nocv;
   typedef typename ::ndnboost::remove_cv< typename ::ndnboost::remove_reference< typename ::ndnboost::remove_pointer<Rhs_noref>::type >::type >::type Rhs_noptr;
   NDNBOOST_STATIC_CONSTANT(bool, value = (trait_impl1 < Rhs_noref, Ret, NDNBOOST_TT_FORBIDDEN_IF >::value));
};

} // namespace impl
} // namespace detail

// this is the accessible definition of the trait to end user
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF2(NDNBOOST_TT_TRAIT_NAME, Rhs, Ret=::ndnboost::detail::NDNBOOST_JOIN(NDNBOOST_TT_TRAIT_NAME,_impl)::dont_care, (::ndnboost::detail::NDNBOOST_JOIN(NDNBOOST_TT_TRAIT_NAME,_impl)::trait_impl < Rhs, Ret >::value))

} // namespace ndnboost

#if defined(NDNBOOST_MSVC)
#   pragma warning ( pop )
#endif

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>
