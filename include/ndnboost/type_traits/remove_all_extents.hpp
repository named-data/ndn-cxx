
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_REMOVE_ALL_EXTENTS_HPP_INCLUDED
#define NDNBOOST_TT_REMOVE_ALL_EXTENTS_HPP_INCLUDED

#include <ndnboost/config.hpp>
#include <cstddef>
#include <ndnboost/detail/workaround.hpp>

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC,<=1300)
#include <ndnboost/type_traits/msvc/remove_all_extents.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/type_trait_def.hpp>

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC,<=1300)

namespace ndnboost {

NDNBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_all_extents,T,T)

#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T[N],typename ndnboost::remove_all_extents<T>::type type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T const[N],typename ndnboost::remove_all_extents<T const>::type type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T volatile[N],typename ndnboost::remove_all_extents<T volatile>::type type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_all_extents,T const volatile[N],typename ndnboost::remove_all_extents<T const volatile>::type type)
#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x610)) && !defined(__IBMCPP__) &&  !NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840))
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T[],typename ndnboost::remove_all_extents<T>::type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T const[],typename ndnboost::remove_all_extents<T const>::type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T volatile[],typename ndnboost::remove_all_extents<T volatile>::type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_all_extents,T const volatile[],typename ndnboost::remove_all_extents<T const volatile>::type)
#endif
#endif

} // namespace ndnboost

#endif

#include <ndnboost/type_traits/detail/type_trait_undef.hpp>

#endif // NDNBOOST_TT_REMOVE_BOUNDS_HPP_INCLUDED
