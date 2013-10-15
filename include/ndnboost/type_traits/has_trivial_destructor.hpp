
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_HAS_TRIVIAL_DESTRUCTOR_HPP_INCLUDED
#define NDNBOOST_TT_HAS_TRIVIAL_DESTRUCTOR_HPP_INCLUDED

#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/type_traits/intrinsics.hpp>
#include <ndnboost/type_traits/is_pod.hpp>
#include <ndnboost/type_traits/detail/ice_or.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail {

template <typename T>
struct has_trivial_dtor_impl
{
#ifdef NDNBOOST_HAS_TRIVIAL_DESTRUCTOR
   NDNBOOST_STATIC_CONSTANT(bool, value = NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T));
#else
   NDNBOOST_STATIC_CONSTANT(bool, value = ::ndnboost::is_pod<T>::value);
#endif
};

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_destructor,T,::ndnboost::detail::has_trivial_dtor_impl<T>::value)

NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_destructor,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_destructor,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_destructor,void const volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_destructor,void volatile,false)
#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_HAS_TRIVIAL_DESTRUCTOR_HPP_INCLUDED
