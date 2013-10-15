//  Boost string_algo library config.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef NDNBOOST_STRING_CONFIG_HPP
#define NDNBOOST_STRING_CONFIG_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>

#ifdef NDNBOOST_STRING_DEDUCED_TYPENAME
#   error "macro already defined!"
#endif

#define NDNBOOST_STRING_TYPENAME NDNBOOST_DEDUCED_TYPENAME

// Metrowerks workaround
#if NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) // 8.x
#pragma parse_func_templ off
#endif

#endif  // NDNBOOST_STRING_CONFIG_HPP
