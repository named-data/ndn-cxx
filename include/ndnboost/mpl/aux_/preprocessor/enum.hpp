
#ifndef NDNBOOST_MPL_AUX_PREPROCESSOR_ENUM_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_PREPROCESSOR_ENUM_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: enum.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/config/preprocessor.hpp>

// NDNBOOST_MPL_PP_ENUM(0,int): <nothing>
// NDNBOOST_MPL_PP_ENUM(1,int): int
// NDNBOOST_MPL_PP_ENUM(2,int): int, int
// NDNBOOST_MPL_PP_ENUM(n,int): int, int, .., int

#if !defined(NDNBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <ndnboost/preprocessor/cat.hpp>

#   define NDNBOOST_MPL_PP_ENUM(n, param) \
    NDNBOOST_PP_CAT(NDNBOOST_MPL_PP_ENUM_,n)(param) \
    /**/
    
#   define NDNBOOST_MPL_PP_ENUM_0(p)
#   define NDNBOOST_MPL_PP_ENUM_1(p) p
#   define NDNBOOST_MPL_PP_ENUM_2(p) p,p
#   define NDNBOOST_MPL_PP_ENUM_3(p) p,p,p
#   define NDNBOOST_MPL_PP_ENUM_4(p) p,p,p,p
#   define NDNBOOST_MPL_PP_ENUM_5(p) p,p,p,p,p
#   define NDNBOOST_MPL_PP_ENUM_6(p) p,p,p,p,p,p
#   define NDNBOOST_MPL_PP_ENUM_7(p) p,p,p,p,p,p,p
#   define NDNBOOST_MPL_PP_ENUM_8(p) p,p,p,p,p,p,p,p
#   define NDNBOOST_MPL_PP_ENUM_9(p) p,p,p,p,p,p,p,p,p

#else

#   include <ndnboost/preprocessor/comma_if.hpp>
#   include <ndnboost/preprocessor/repeat.hpp>

#   define NDNBOOST_MPL_PP_AUX_ENUM_FUNC(unused, i, param) \
    NDNBOOST_PP_COMMA_IF(i) param \
    /**/

#   define NDNBOOST_MPL_PP_ENUM(n, param) \
    NDNBOOST_PP_REPEAT( \
          n \
        , NDNBOOST_MPL_PP_AUX_ENUM_FUNC \
        , param \
        ) \
    /**/

#endif

#endif // NDNBOOST_MPL_AUX_PREPROCESSOR_ENUM_HPP_INCLUDED
