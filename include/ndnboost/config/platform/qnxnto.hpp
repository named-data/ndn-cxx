//  (C) Copyright Jim Douglas 2005. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  QNX specific config options:

#define NDNBOOST_PLATFORM "QNX"

#define NDNBOOST_HAS_UNISTD_H
#include <ndnboost/config/posix_features.hpp>

// QNX claims XOpen version 5 compatibility, but doesn't have an nl_types.h
// or log1p and expm1:
#undef  NDNBOOST_HAS_NL_TYPES_H
#undef  NDNBOOST_HAS_LOG1P
#undef  NDNBOOST_HAS_EXPM1

#define NDNBOOST_HAS_PTHREADS
#define NDNBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE

#define NDNBOOST_HAS_GETTIMEOFDAY
#define NDNBOOST_HAS_CLOCK_GETTIME
#define NDNBOOST_HAS_NANOSLEEP





