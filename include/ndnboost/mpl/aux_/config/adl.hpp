
#ifndef NDNBOOST_MPL_AUX_CONFIG_ADL_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_CONFIG_ADL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: adl.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/config/msvc.hpp>
#include <ndnboost/mpl/aux_/config/intel.hpp>
#include <ndnboost/mpl/aux_/config/gcc.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

// agurt, 25/apr/04: technically, the ADL workaround is only needed for GCC,
// but putting everything expect public, user-specializable metafunctions into
// a separate global namespace has a nice side effect of reducing the length 
// of template instantiation symbols, so we apply the workaround on all 
// platforms that can handle it

#if !defined(NDNBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE) \
    && (   NDNBOOST_WORKAROUND(NDNBOOST_MSVC, NDNBOOST_TESTED_AT(1400)) \
        || NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x610)) \
        || NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840)) \
        || NDNBOOST_WORKAROUND(__MWERKS__, NDNBOOST_TESTED_AT(0x3202)) \
        || NDNBOOST_WORKAROUND(NDNBOOST_INTEL_CXX_VERSION, NDNBOOST_TESTED_AT(810)) \
        )

#   define NDNBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE

#endif

#endif // NDNBOOST_MPL_AUX_CONFIG_ADL_HPP_INCLUDED
