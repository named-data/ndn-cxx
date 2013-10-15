
#ifndef NDNBOOST_MPL_AUX_PREPROCESSOR_DEF_PARAMS_TAIL_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_PREPROCESSOR_DEF_PARAMS_TAIL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: def_params_tail.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/limits/arity.hpp>
#include <ndnboost/mpl/aux_/config/dtp.hpp>
#include <ndnboost/mpl/aux_/config/preprocessor.hpp>

#include <ndnboost/preprocessor/comma_if.hpp>
#include <ndnboost/preprocessor/logical/and.hpp>
#include <ndnboost/preprocessor/identity.hpp>
#include <ndnboost/preprocessor/empty.hpp>

// NDNBOOST_MPL_PP_DEF_PARAMS_TAIL(1,T,value): , T1 = value, .., Tn = value
// NDNBOOST_MPL_PP_DEF_PARAMS_TAIL(2,T,value): , T2 = value, .., Tn = value
// NDNBOOST_MPL_PP_DEF_PARAMS_TAIL(n,T,value): <nothing>

#if !defined(NDNBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)

#   include <ndnboost/mpl/aux_/preprocessor/filter_params.hpp>
#   include <ndnboost/mpl/aux_/preprocessor/sub.hpp>

#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, value_func) \
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_1( \
          i \
        , NDNBOOST_MPL_PP_SUB(NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY,i) \
        , param \
        , value_func \
        ) \
    /**/

#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_1(i, n, param, value_func) \
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_2(i,n,param,value_func) \
    /**/

#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_DELAY_2(i, n, param, value_func) \
    NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_AND(i,n)) \
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_##i(n,param,value_func) \
    /**/

#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_0(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##1 v(),p##2 v(),p##3 v(),p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v())
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_1(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##2 v(),p##3 v(),p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_2(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##3 v(),p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_3(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##4 v(),p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2,p3)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_4(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##5 v(),p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2,p3,p4)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_5(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##6 v(),p##7 v(),p##8 v(),p##9 v(),p1,p2,p3,p4,p5)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_6(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##7 v(),p##8 v(),p##9 v(),p1,p2,p3,p4,p5,p6)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_7(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##8 v(),p##9 v(),p1,p2,p3,p4,p5,p6,p7)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_8(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p##9 v(),p1,p2,p3,p4,p5,p6,p7,p8)
#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_9(i,p,v) NDNBOOST_MPL_PP_FILTER_PARAMS_##i(p1,p2,p3,p4,p5,p6,p7,p8,p9)

#else

#   include <ndnboost/preprocessor/arithmetic/add.hpp>
#   include <ndnboost/preprocessor/arithmetic/sub.hpp>
#   include <ndnboost/preprocessor/inc.hpp>
#   include <ndnboost/preprocessor/tuple/elem.hpp>
#   include <ndnboost/preprocessor/repeat.hpp>
#   include <ndnboost/preprocessor/cat.hpp>

#   define NDNBOOST_MPL_PP_AUX_TAIL_PARAM_FUNC(unused, i, op) \
    , NDNBOOST_PP_CAT( \
          NDNBOOST_PP_TUPLE_ELEM(3, 1, op) \
        , NDNBOOST_PP_ADD_D(1, i, NDNBOOST_PP_INC(NDNBOOST_PP_TUPLE_ELEM(3, 0, op))) \
        ) NDNBOOST_PP_TUPLE_ELEM(3, 2, op)() \
    /**/

#   define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, value_func) \
    NDNBOOST_PP_REPEAT( \
          NDNBOOST_PP_SUB_D(1, NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY, i) \
        , NDNBOOST_MPL_PP_AUX_TAIL_PARAM_FUNC \
        , (i, param, value_func) \
        ) \
    /**/


#endif // NDNBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES

#define NDNBOOST_MPL_PP_DEF_PARAMS_TAIL(i, param, value) \
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, NDNBOOST_PP_IDENTITY(=value)) \
    /**/

#if !defined(NDNBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define NDNBOOST_MPL_PP_NESTED_DEF_PARAMS_TAIL(i, param, value) \
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, NDNBOOST_PP_IDENTITY(=value)) \
    /**/
#else
#   define NDNBOOST_MPL_PP_NESTED_DEF_PARAMS_TAIL(i, param, value) \
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL_IMPL(i, param, NDNBOOST_PP_EMPTY) \
    /**/
#endif

#endif // NDNBOOST_MPL_AUX_PREPROCESSOR_DEF_PARAMS_TAIL_HPP_INCLUDED
