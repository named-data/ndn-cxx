
#ifndef NDNBOOST_MPL_AUX_CONFIG_BCC_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_CONFIG_BCC_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2008
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: bcc.hpp 49272 2008-10-11 06:50:46Z agurtovoy $
// $Date: 2004-09-02 10:41:37 -0500 (Thu, 02 Sep 2004) $
// $Revision: 24874 $

#include <ndnboost/mpl/aux_/config/workaround.hpp>

#if    !defined(NDNBOOST_MPL_CFG_BCC590_WORKAROUNDS) \
    && !defined(NDNBOOST_MPL_PREPROCESSING_MODE) \
    && NDNBOOST_WORKAROUND(__BORLANDC__, >= 0x590) \
    && NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x610))

#   define NDNBOOST_MPL_CFG_BCC590_WORKAROUNDS

#endif

#endif // NDNBOOST_MPL_AUX_CONFIG_BCC_HPP_INCLUDED
