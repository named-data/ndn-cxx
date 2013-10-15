
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000-2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_REMOVE_EXTENT_HPP_INCLUDED
#define NDNBOOST_TT_REMOVE_EXTENT_HPP_INCLUDED

#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>
#include <cstddef>

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC,<=1300)
#include <ndnboost/type_traits/msvc/remove_extent.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/type_trait_def.hpp>

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC,<=1300)

namespace ndnboost {

NDNBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_extent,T,T)

#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T[N],T type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T const[N],T const type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T volatile[N],T volatile type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_extent,T const volatile[N],T const volatile type)
#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x610)) && !defined(__IBMCPP__) &&  !NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840))
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T[],T)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T const[],T const)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T volatile[],T volatile)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_extent,T const volatile[],T const volatile)
#endif
#endif

} // namespace ndnboost

#endif

#include <ndnboost/type_traits/detail/type_trait_undef.hpp>

#endif // NDNBOOST_TT_REMOVE_BOUNDS_HPP_INCLUDED
