
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_HAS_NOTHROW_ASSIGN_HPP_INCLUDED
#define NDNBOOST_TT_HAS_NOTHROW_ASSIGN_HPP_INCLUDED

#include <ndnboost/type_traits/has_trivial_assign.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail{

template <class T>
struct has_nothrow_assign_imp{
#ifndef NDNBOOST_HAS_NOTHROW_ASSIGN
   NDNBOOST_STATIC_CONSTANT(bool, value = ::ndnboost::has_trivial_assign<T>::value);
#else
   NDNBOOST_STATIC_CONSTANT(bool, value = NDNBOOST_HAS_NOTHROW_ASSIGN(T));
#endif
};

}

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_nothrow_assign,T,::ndnboost::detail::has_nothrow_assign_imp<T>::value)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_assign,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_assign,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_assign,void const volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_nothrow_assign,void volatile,false)
#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_HAS_NOTHROW_ASSIGN_HPP_INCLUDED
