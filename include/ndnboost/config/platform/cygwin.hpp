//  (C) Copyright John Maddock 2001 - 2003. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  cygwin specific config options:

#define NDNBOOST_PLATFORM "Cygwin"
#define NDNBOOST_HAS_DIRENT_H
#define NDNBOOST_HAS_LOG1P
#define NDNBOOST_HAS_EXPM1

//
// Threading API:
// See if we have POSIX threads, if we do use them, otherwise
// revert to native Win threads.
#define NDNBOOST_HAS_UNISTD_H
#include <unistd.h>
#if defined(_POSIX_THREADS) && (_POSIX_THREADS+0 >= 0) && !defined(NDNBOOST_HAS_WINTHREADS)
#  define NDNBOOST_HAS_PTHREADS
#  define NDNBOOST_HAS_SCHED_YIELD
#  define NDNBOOST_HAS_GETTIMEOFDAY
#  define NDNBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#  define NDNBOOST_HAS_SIGACTION
#else
#  if !defined(NDNBOOST_HAS_WINTHREADS)
#     define NDNBOOST_HAS_WINTHREADS
#  endif
#  define NDNBOOST_HAS_FTIME
#endif

//
// find out if we have a stdint.h, there should be a better way to do this:
//
#include <sys/types.h>
#ifdef _STDINT_H
#define NDNBOOST_HAS_STDINT_H
#endif

/// Cygwin has no fenv.h
#define NDNBOOST_NO_FENV_H

// boilerplate code:
#include <ndnboost/config/posix_features.hpp>

//
// Cygwin lies about XSI conformance, there is no nl_types.h:
//
#ifdef NDNBOOST_HAS_NL_TYPES_H
#  undef NDNBOOST_HAS_NL_TYPES_H
#endif
 




