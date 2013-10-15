//  (C) Copyright Artyom Beilis 2010.  
//  Use, modification and distribution are subject to the  
//  Boost Software License, Version 1.0. (See accompanying file  
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 

#ifndef NDNBOOST_CONFIG_PLATFORM_VMS_HPP 
#define NDNBOOST_CONFIG_PLATFORM_VMS_HPP 

#define NDNBOOST_PLATFORM "OpenVMS" 

#undef  NDNBOOST_HAS_STDINT_H 
#define NDNBOOST_HAS_UNISTD_H 
#define NDNBOOST_HAS_NL_TYPES_H 
#define NDNBOOST_HAS_GETTIMEOFDAY 
#define NDNBOOST_HAS_DIRENT_H 
#define NDNBOOST_HAS_PTHREADS 
#define NDNBOOST_HAS_NANOSLEEP 
#define NDNBOOST_HAS_CLOCK_GETTIME 
#define NDNBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE 
#define NDNBOOST_HAS_LOG1P 
#define NDNBOOST_HAS_EXPM1 
#define NDNBOOST_HAS_THREADS 
#undef  NDNBOOST_HAS_SCHED_YIELD 

#endif 
