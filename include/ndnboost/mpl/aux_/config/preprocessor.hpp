
#ifndef NDNBOOST_MPL_AUX_CONFIG_PREPROCESSOR_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_CONFIG_PREPROCESSOR_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: preprocessor.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/config/workaround.hpp>

#if !defined(NDNBOOST_MPL_CFG_BROKEN_PP_MACRO_EXPANSION) \
    && (   NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) \
        || NDNBOOST_WORKAROUND(__BORLANDC__, < 0x582) \
        || NDNBOOST_WORKAROUND(__IBMCPP__, NDNBOOST_TESTED_AT(502)) \
        )

#   define NDNBOOST_MPL_CFG_BROKEN_PP_MACRO_EXPANSION

#endif

#if !defined(NDNBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES)
#   define NDNBOOST_MPL_CFG_NO_OWN_PP_PRIMITIVES
#endif

#if !defined(NDNBOOST_NEEDS_TOKEN_PASTING_OP_FOR_TOKENS_JUXTAPOSING) \
    && NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840))
#   define NDNBOOST_NEEDS_TOKEN_PASTING_OP_FOR_TOKENS_JUXTAPOSING
#endif


#endif // NDNBOOST_MPL_AUX_CONFIG_PREPROCESSOR_HPP_INCLUDED
