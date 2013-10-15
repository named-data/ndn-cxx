//  (C) Copyright John Maddock 2001 - 2003. 
//  (C) Copyright Darin Adler 2001 - 2002. 
//  (C) Copyright Bill Kempf 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Mac OS specific config options:

#define NDNBOOST_PLATFORM "Mac OS"

#if __MACH__ && !defined(_MSL_USING_MSL_C)

// Using the Mac OS X system BSD-style C library.

#  ifndef NDNBOOST_HAS_UNISTD_H
#    define NDNBOOST_HAS_UNISTD_H
#  endif
//
// Begin by including our boilerplate code for POSIX
// feature detection, this is safe even when using
// the MSL as Metrowerks supply their own <unistd.h>
// to replace the platform-native BSD one. G++ users
// should also always be able to do this on MaxOS X.
//
#  include <ndnboost/config/posix_features.hpp>
#  ifndef NDNBOOST_HAS_STDINT_H
#     define NDNBOOST_HAS_STDINT_H
#  endif

//
// BSD runtime has pthreads, sigaction, sched_yield and gettimeofday,
// of these only pthreads are advertised in <unistd.h>, so set the 
// other options explicitly:
//
#  define NDNBOOST_HAS_SCHED_YIELD
#  define NDNBOOST_HAS_GETTIMEOFDAY
#  define NDNBOOST_HAS_SIGACTION

#  if (__GNUC__ < 3) && !defined( __APPLE_CC__)

// GCC strange "ignore std" mode works better if you pretend everything
// is in the std namespace, for the most part.

#    define NDNBOOST_NO_STDC_NAMESPACE
#  endif

#  if (__GNUC__ == 4)

// Both gcc and intel require these.  
#    define NDNBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#    define NDNBOOST_HAS_NANOSLEEP

#  endif

#else

// Using the MSL C library.

// We will eventually support threads in non-Carbon builds, but we do
// not support this yet.
#  if ( defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON ) || ( defined(TARGET_CARBON) && TARGET_CARBON )

#  if !defined(NDNBOOST_HAS_PTHREADS)
// MPTasks support is deprecated/removed from Boost:
//#    define NDNBOOST_HAS_MPTASKS
#  elif ( __dest_os == __mac_os_x )
// We are doing a Carbon/Mach-O/MSL build which has pthreads, but only the
// gettimeofday and no posix.
#  define NDNBOOST_HAS_GETTIMEOFDAY
#  endif

#ifdef NDNBOOST_HAS_PTHREADS
#  define NDNBOOST_HAS_THREADS
#endif

// The remote call manager depends on this.
#    define NDNBOOST_BIND_ENABLE_PASCAL

#  endif

#endif



