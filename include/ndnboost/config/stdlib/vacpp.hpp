//  (C) Copyright John Maddock 2001 - 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

#if __IBMCPP__ <= 501
#  define NDNBOOST_NO_STD_ALLOCATOR
#endif

#define NDNBOOST_HAS_MACRO_USE_FACET
#define NDNBOOST_NO_STD_MESSAGES

// Apple doesn't seem to reliably defined a *unix* macro
#if !defined(CYGWIN) && (  defined(__unix__)  \
                        || defined(__unix)    \
                        || defined(unix)      \
                        || defined(__APPLE__) \
                        || defined(__APPLE)   \
                        || defined(APPLE))
#  include <unistd.h>
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

#define NDNBOOST_STDLIB "Visual Age default standard library"



