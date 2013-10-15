//  (C) Copyright John Maddock 2001. 
//  (C) Copyright Darin Adler 2001. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Metrowerks standard library:

#ifndef __MSL_CPP__
#  include <ndnboost/config/no_tr1/utility.hpp>
#  ifndef __MSL_CPP__
#     error This is not the MSL standard library!
#  endif
#endif

#if __MSL_CPP__ >= 0x6000  // Pro 6
#  define NDNBOOST_HAS_HASH
#  define NDNBOOST_STD_EXTENSION_NAMESPACE Metrowerks
#endif
#define NDNBOOST_HAS_SLIST

#if __MSL_CPP__ < 0x6209
#  define NDNBOOST_NO_STD_MESSAGES
#endif

// check C lib version for <stdint.h>
#include <cstddef>

#if defined(__MSL__) && (__MSL__ >= 0x5000)
#  define NDNBOOST_HAS_STDINT_H
#  if !defined(__PALMOS_TRAPS__)
#    define NDNBOOST_HAS_UNISTD_H
#  endif
   // boilerplate code:
#  include <ndnboost/config/posix_features.hpp>
#endif

#if defined(_MWMT) || _MSL_THREADSAFE
#  define NDNBOOST_HAS_THREADS
#endif

#ifdef _MSL_NO_EXPLICIT_FUNC_TEMPLATE_ARG
#  define NDNBOOST_NO_STD_USE_FACET
#  define NDNBOOST_HAS_TWO_ARG_USE_FACET
#endif

//  C++0x headers not yet implemented
//
#  define NDNBOOST_NO_CXX11_HDR_ARRAY
#  define NDNBOOST_NO_CXX11_HDR_CHRONO
#  define NDNBOOST_NO_CXX11_HDR_CODECVT
#  define NDNBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define NDNBOOST_NO_CXX11_HDR_FORWARD_LIST
#  define NDNBOOST_NO_CXX11_HDR_FUTURE
#  define NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define NDNBOOST_NO_CXX11_HDR_MUTEX
#  define NDNBOOST_NO_CXX11_HDR_RANDOM
#  define NDNBOOST_NO_CXX11_HDR_RATIO
#  define NDNBOOST_NO_CXX11_HDR_REGEX
#  define NDNBOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define NDNBOOST_NO_CXX11_HDR_THREAD
#  define NDNBOOST_NO_CXX11_HDR_TUPLE
#  define NDNBOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define NDNBOOST_NO_CXX11_HDR_TYPEINDEX
#  define NDNBOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define NDNBOOST_NO_CXX11_HDR_UNORDERED_SET
#  define NDNBOOST_NO_CXX11_NUMERIC_LIMITS
#  define NDNBOOST_NO_CXX11_ALLOCATOR
#  define NDNBOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define NDNBOOST_NO_CXX11_SMART_PTR
#  define NDNBOOST_NO_CXX11_HDR_FUNCTIONAL

#define NDNBOOST_STDLIB "Metrowerks Standard Library version " NDNBOOST_STRINGIZE(__MSL_CPP__)









