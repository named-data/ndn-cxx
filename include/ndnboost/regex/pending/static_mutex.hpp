/*
 *
 * Copyright (c) 2004
 * John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
 
 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         static_mutex.hpp
  *   VERSION      see <ndnboost/version.hpp>
  *   DESCRIPTION: Declares static_mutex lock type, there are three different
  *                implementations: POSIX pthreads, WIN32 threads, and portable,
  *                these are described in more detail below.
  */

#ifndef NDNBOOST_REGEX_STATIC_MUTEX_HPP
#define NDNBOOST_REGEX_STATIC_MUTEX_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/regex/config.hpp> // dll import/export options.

#ifdef NDNBOOST_HAS_PTHREADS
#include <pthread.h>
#endif

#if defined(NDNBOOST_HAS_PTHREADS) && defined(PTHREAD_MUTEX_INITIALIZER)
//
// pthreads version:
// simple wrap around a pthread_mutex_t initialized with
// PTHREAD_MUTEX_INITIALIZER.
//
namespace ndnboost{

class static_mutex;

#define NDNBOOST_STATIC_MUTEX_INIT { PTHREAD_MUTEX_INITIALIZER, }

class NDNBOOST_REGEX_DECL scoped_static_mutex_lock
{
public:
   scoped_static_mutex_lock(static_mutex& mut, bool lk = true);
   ~scoped_static_mutex_lock();
   inline bool locked()const
   {
      return m_have_lock;
   }
   inline operator void const*()const
   {
      return locked() ? this : 0;
   }
   void lock();
   void unlock();
private:
   static_mutex& m_mutex;
   bool m_have_lock;
};

class static_mutex
{
public:
   typedef scoped_static_mutex_lock scoped_lock;
   pthread_mutex_t m_mutex;
};

} // namespace ndnboost
#elif defined(NDNBOOST_HAS_WINTHREADS)
//
// Win32 version:
// Use a 32-bit int as a lock, along with a test-and-set
// implementation using InterlockedCompareExchange.
//

#include <ndnboost/cstdint.hpp>

namespace ndnboost{

class NDNBOOST_REGEX_DECL scoped_static_mutex_lock;

class static_mutex
{
public:
   typedef scoped_static_mutex_lock scoped_lock;
   ndnboost::int32_t m_mutex;
};

#define NDNBOOST_STATIC_MUTEX_INIT { 0, }

class NDNBOOST_REGEX_DECL scoped_static_mutex_lock
{
public:
   scoped_static_mutex_lock(static_mutex& mut, bool lk = true);
   ~scoped_static_mutex_lock();
   operator void const*()const
   {
      return locked() ? this : 0;
   }
   bool locked()const
   {
      return m_have_lock;
   }
   void lock();
   void unlock();
private:
   static_mutex& m_mutex;
   bool m_have_lock;
   scoped_static_mutex_lock(const scoped_static_mutex_lock&);
   scoped_static_mutex_lock& operator=(const scoped_static_mutex_lock&);
};

} // namespace

#else
//
// Portable version of a static mutex based on Boost.Thread library:
// This has to use a single mutex shared by all instances of static_mutex
// because ndnboost::call_once doesn't alow us to pass instance information
// down to the initialisation proceedure.  In fact the initialisation routine
// may need to be called more than once - but only once per instance.
//
// Since this preprocessor path is almost never taken, we hide these header
// dependencies so that build tools don't find them.
//
#define B1 <ndnboost/thread/once.hpp>
#define B2 <ndnboost/thread/recursive_mutex.hpp>
#include B1
#include B2
#undef B1
#undef B2

namespace ndnboost{

class NDNBOOST_REGEX_DECL scoped_static_mutex_lock;
extern "C" NDNBOOST_REGEX_DECL void ndnboost_regex_free_static_mutex();

class NDNBOOST_REGEX_DECL static_mutex
{
public:
   typedef scoped_static_mutex_lock scoped_lock;
   static void init();
   static ndnboost::recursive_mutex* m_pmutex;
   static ndnboost::once_flag m_once;
};

#define NDNBOOST_STATIC_MUTEX_INIT {  }

class NDNBOOST_REGEX_DECL scoped_static_mutex_lock
{
public:
   scoped_static_mutex_lock(static_mutex& mut, bool lk = true);
   ~scoped_static_mutex_lock();
   operator void const*()const;
   bool locked()const;
   void lock();
   void unlock();
private:
   ndnboost::recursive_mutex::scoped_lock* m_plock;
   bool m_have_lock;
};

inline scoped_static_mutex_lock::operator void const*()const
{
   return locked() ? this : 0;
}

inline bool scoped_static_mutex_lock::locked()const
{
   return m_have_lock;
}

} // namespace

#endif

#endif
