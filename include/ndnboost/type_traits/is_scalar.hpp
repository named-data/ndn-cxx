
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_IS_SCALAR_HPP_INCLUDED
#define NDNBOOST_TT_IS_SCALAR_HPP_INCLUDED

#include <ndnboost/type_traits/is_arithmetic.hpp>
#include <ndnboost/type_traits/is_enum.hpp>
#include <ndnboost/type_traits/is_pointer.hpp>
#include <ndnboost/type_traits/is_member_pointer.hpp>
#include <ndnboost/type_traits/detail/ice_or.hpp>
#include <ndnboost/config.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail {

template <typename T>
struct is_scalar_impl
{ 
   NDNBOOST_STATIC_CONSTANT(bool, value =
      (::ndnboost::type_traits::ice_or<
         ::ndnboost::is_arithmetic<T>::value,
         ::ndnboost::is_enum<T>::value,
         ::ndnboost::is_pointer<T>::value,
         ::ndnboost::is_member_pointer<T>::value
      >::value));
};

// these specializations are only really needed for compilers 
// without partial specialization support:
template <> struct is_scalar_impl<void>{ NDNBOOST_STATIC_CONSTANT(bool, value = false ); };
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct is_scalar_impl<void const>{ NDNBOOST_STATIC_CONSTANT(bool, value = false ); };
template <> struct is_scalar_impl<void volatile>{ NDNBOOST_STATIC_CONSTANT(bool, value = false ); };
template <> struct is_scalar_impl<void const volatile>{ NDNBOOST_STATIC_CONSTANT(bool, value = false ); };
#endif

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_scalar,T,::ndnboost::detail::is_scalar_impl<T>::value)

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_SCALAR_HPP_INCLUDED
