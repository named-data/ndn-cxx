
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_HAS_NOTHROW_COPY_HPP_INCLUDED
#define NDNBOOST_TT_HAS_NOTHROW_COPY_HPP_INCLUDED

#include <ndnboost/type_traits/has_trivial_copy.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail{

template <class T>
struct has_nothrow_copy_imp{
#ifdef NDNBOOST_HAS_NOTHROW_COPY
   NDNBOOST_STATIC_CONSTANT(bool, value = NDNBOOST_HAS_NOTHROW_COPY(T));
#else
   NDNBOOST_STATIC_CONSTANT(bool, value = ::ndnboost::has_trivial_copy<T>::value);
#endif
};

}

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_nothrow_copy,T,::ndnboost::detail::has_nothrow_copy_imp<T>::value)
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_nothrow_copy_constructor,T,::ndnboost::detail::has_nothrow_copy_imp<T>::value)

NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy,void const volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy,void volatile,false)
#endif

NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy_constructor,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy_constructor,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy_constructor,void const volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_copy_constructor,void volatile,false)
#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_HAS_NOTHROW_COPY_HPP_INCLUDED
