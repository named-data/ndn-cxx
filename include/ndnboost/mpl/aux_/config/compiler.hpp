
#ifndef NDNBOOST_MPL_AUX_CONFIG_COMPILER_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_CONFIG_COMPILER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2008
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: compiler.hpp 53189 2009-05-22 20:07:55Z hkaiser $
// $Date: 2009-05-22 13:07:55 -0700 (Fri, 22 May 2009) $
// $Revision: 53189 $

#if !defined(NDNBOOST_MPL_CFG_COMPILER_DIR)

#   include <ndnboost/mpl/aux_/config/dtp.hpp>
#   include <ndnboost/mpl/aux_/config/ttp.hpp>
#   include <ndnboost/mpl/aux_/config/ctps.hpp>
#   include <ndnboost/mpl/aux_/config/msvc.hpp>
#   include <ndnboost/mpl/aux_/config/gcc.hpp>
#   include <ndnboost/mpl/aux_/config/workaround.hpp>

#   if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
#       define NDNBOOST_MPL_CFG_COMPILER_DIR msvc60

#   elif NDNBOOST_WORKAROUND(NDNBOOST_MSVC, == 1300)
#       define NDNBOOST_MPL_CFG_COMPILER_DIR msvc70

#   elif NDNBOOST_WORKAROUND(NDNBOOST_MPL_CFG_GCC, NDNBOOST_TESTED_AT(0x0304))
#       define NDNBOOST_MPL_CFG_COMPILER_DIR gcc

#   elif NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x610))
#       if !defined(NDNBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#           define NDNBOOST_MPL_CFG_COMPILER_DIR bcc551
#       elif NDNBOOST_WORKAROUND(__BORLANDC__, >= 0x590)
#           define NDNBOOST_MPL_CFG_COMPILER_DIR bcc
#       else
#           define NDNBOOST_MPL_CFG_COMPILER_DIR bcc_pre590
#       endif

#   elif NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840))
#       define NDNBOOST_MPL_CFG_COMPILER_DIR dmc

#   elif defined(__MWERKS__)
#       if defined(NDNBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#           define NDNBOOST_MPL_CFG_COMPILER_DIR mwcw
#       else
#           define NDNBOOST_MPL_CFG_COMPILER_DIR plain
#       endif

#   elif defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#       define NDNBOOST_MPL_CFG_COMPILER_DIR no_ctps

#   elif defined(NDNBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS)
#       define NDNBOOST_MPL_CFG_COMPILER_DIR no_ttp

#   else
#       define NDNBOOST_MPL_CFG_COMPILER_DIR plain
#   endif

#endif // NDNBOOST_MPL_CFG_COMPILER_DIR

#endif // NDNBOOST_MPL_AUX_CONFIG_COMPILER_HPP_INCLUDED
