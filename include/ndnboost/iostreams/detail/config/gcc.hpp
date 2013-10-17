// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Adapted from <ndnboost/config/auto_link.hpp> and from
// http://www.boost.org/more/separate_compilation.html, by John Maddock.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_CONFIG_GCC_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_CONFIG_GCC_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <ndnboost/config.hpp> // NDNBOOST_INTEL.

#if defined(__GNUC__) && !defined(NDNBOOST_INTEL)
# define NDNBOOST_IOSTREAMS_GCC (__GNUC__ * 100 + __GNUC_MINOR__)
# define NDNBOOST_IOSTREAMS_GCC_WORKAROUND_GUARD 1
#else
# define NDNBOOST_IOSTREAMS_GCC_WORKAROUND_GUARD 0
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_CONFIG_GCC_HPP_INCLUDED
