//  (C) Copyright John Maddock 2001 - 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  IBM/Aix specific config options:

#define NDNBOOST_PLATFORM "IBM Aix"

#define NDNBOOST_HAS_UNISTD_H
#define NDNBOOST_HAS_NL_TYPES_H
#define NDNBOOST_HAS_NANOSLEEP
#define NDNBOOST_HAS_CLOCK_GETTIME

// This needs support in "ndnboost/cstdint.hpp" exactly like FreeBSD.
// This platform has header named <inttypes.h> which includes all
// the things needed.
#define NDNBOOST_HAS_STDINT_H

// Threading API's:
#define NDNBOOST_HAS_PTHREADS
#define NDNBOOST_HAS_PTHREAD_DELAY_NP
#define NDNBOOST_HAS_SCHED_YIELD
//#define NDNBOOST_HAS_PTHREAD_YIELD

// boilerplate code:
#include <ndnboost/config/posix_features.hpp>




