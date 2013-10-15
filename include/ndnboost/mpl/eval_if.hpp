
#ifndef NDNBOOST_MPL_EVAL_IF_HPP_INCLUDED
#define NDNBOOST_MPL_EVAL_IF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: eval_if.hpp 61921 2010-05-11 21:33:24Z neilgroves $
// $Date: 2010-05-11 14:33:24 -0700 (Tue, 11 May 2010) $
// $Revision: 61921 $

#include <ndnboost/mpl/if.hpp>
#include <ndnboost/mpl/aux_/na_spec.hpp>
#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/mpl/aux_/config/msvc.hpp>
#include <ndnboost/mpl/aux_/config/gcc.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

namespace ndnboost { namespace mpl {

template<
      typename NDNBOOST_MPL_AUX_NA_PARAM(C)
    , typename NDNBOOST_MPL_AUX_NA_PARAM(F1)
    , typename NDNBOOST_MPL_AUX_NA_PARAM(F2)
    >
struct eval_if
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) \
     || ( NDNBOOST_WORKAROUND(NDNBOOST_MPL_CFG_GCC, >= 0x0300) \
        && NDNBOOST_WORKAROUND(NDNBOOST_MPL_CFG_GCC, NDNBOOST_TESTED_AT(0x0304)) \
        )
{
    typedef typename if_<C,F1,F2>::type f_;
    typedef typename f_::type type;
#else
    : if_<C,F1,F2>::type
{
#endif
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(3,eval_if,(C,F1,F2))
};

// (almost) copy & paste in order to save one more 
// recursively nested template instantiation to user
template<
      bool C
    , typename F1
    , typename F2
    >
struct eval_if_c
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) \
     || ( NDNBOOST_WORKAROUND(NDNBOOST_MPL_CFG_GCC, >= 0x0300) \
        && NDNBOOST_WORKAROUND(NDNBOOST_MPL_CFG_GCC, NDNBOOST_TESTED_AT(0x0304)) \
        )
{
    typedef typename if_c<C,F1,F2>::type f_;
    typedef typename f_::type type;
#else
    : if_c<C,F1,F2>::type
{
#endif
};

NDNBOOST_MPL_AUX_NA_SPEC(3, eval_if)

}}

#endif // NDNBOOST_MPL_EVAL_IF_HPP_INCLUDED
