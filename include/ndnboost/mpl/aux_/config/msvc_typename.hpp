
#ifndef NDNBOOST_MPL_AUX_CONFIG_MSVC_TYPENAME_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_CONFIG_MSVC_TYPENAME_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: msvc_typename.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/config/msvc.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300)
#   define NDNBOOST_MSVC_TYPENAME
#else
#   define NDNBOOST_MSVC_TYPENAME typename
#endif

#endif // NDNBOOST_MPL_AUX_CONFIG_MSVC_TYPENAME_HPP_INCLUDED
