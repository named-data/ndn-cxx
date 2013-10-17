// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Adapted from http://www.boost.org/more/separate_compilation.html, by
// John Maddock.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_CONFIG_DYN_LINK_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_CONFIG_DYN_LINK_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>

//------------------Enable dynamic linking on windows-------------------------// 

#ifdef NDNBOOST_HAS_DECLSPEC 
# if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_IOSTREAMS_DYN_LINK)
#  ifdef NDNBOOST_IOSTREAMS_SOURCE
#   define NDNBOOST_IOSTREAMS_DECL __declspec(dllexport)
#  else
#   define NDNBOOST_IOSTREAMS_DECL __declspec(dllimport)
#  endif  
# endif  
#endif 

#ifndef NDNBOOST_IOSTREAMS_DECL
# define NDNBOOST_IOSTREAMS_DECL
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_CONFIG_DYN_LINK_HPP_INCLUDED
