
#ifndef NDNBOOST_MPL_AUX_VALUE_WKND_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_VALUE_WKND_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: value_wknd.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/static_cast.hpp>
#include <ndnboost/mpl/aux_/config/integral.hpp>
#include <ndnboost/mpl/aux_/config/eti.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

#if defined(NDNBOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS) \
    || defined(NDNBOOST_MPL_CFG_MSVC_60_ETI_BUG)

#   include <ndnboost/mpl/int.hpp>

namespace ndnboost { namespace mpl { namespace aux {
template< typename C_ > struct value_wknd
    : C_
{
};

#if defined(NDNBOOST_MPL_CFG_MSVC_60_ETI_BUG)
template<> struct value_wknd<int>
    : int_<1>
{
    using int_<1>::value;
};
#endif
}}}


#if !defined(NDNBOOST_MPL_CFG_MSVC_60_ETI_BUG)
#   define NDNBOOST_MPL_AUX_VALUE_WKND(C) \
    ::NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::aux::value_wknd< C > \
/**/
#    define NDNBOOST_MPL_AUX_MSVC_VALUE_WKND(C) NDNBOOST_MPL_AUX_VALUE_WKND(C)
#else
#   define NDNBOOST_MPL_AUX_VALUE_WKND(C) C
#   define NDNBOOST_MPL_AUX_MSVC_VALUE_WKND(C) \
    ::ndnboost::mpl::aux::value_wknd< C > \
/**/
#endif

#else // NDNBOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS

#   define NDNBOOST_MPL_AUX_VALUE_WKND(C) C
#   define NDNBOOST_MPL_AUX_MSVC_VALUE_WKND(C) C

#endif

#if NDNBOOST_WORKAROUND(__EDG_VERSION__, <= 238)
#   define NDNBOOST_MPL_AUX_NESTED_VALUE_WKND(T, C) \
    NDNBOOST_MPL_AUX_STATIC_CAST(T, C::value) \
/**/
#else
#   define NDNBOOST_MPL_AUX_NESTED_VALUE_WKND(T, C) \
    NDNBOOST_MPL_AUX_VALUE_WKND(C)::value \
/**/
#endif


namespace ndnboost { namespace mpl { namespace aux {

template< typename T > struct value_type_wknd
{
    typedef typename T::value_type type;
};

#if defined(NDNBOOST_MPL_CFG_MSVC_ETI_BUG)
template<> struct value_type_wknd<int>
{
    typedef int type;
};
#endif

}}}

#endif // NDNBOOST_MPL_AUX_VALUE_WKND_HPP_INCLUDED
