/*
 * Distributed under the Boost Software License, Version 1.0.(See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)
 * 
 * See http://www.boost.org/libs/iostreams for documentation.
 *
 * Defines preprocessor symbols expanding to the names of functions in the 
 * C runtime library used to access file descriptors and to the type used
 * to store file offsets for seeking.
 * 
 * File:        ndnboost/iostreams/detail/config/rtl.hpp
 * Date:        Wed Dec 26 11:58:11 MST 2007
 * 
 * Copyright:   2007-2008 CodeRage, LLC
 * Author:      Jonathan Turkanis
 * Contact:     turkanis at coderage dot com
 */

#ifndef NDNBOOST_IOSTREAMS_DETAIL_CONFIG_RTL_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_CONFIG_RTL_HPP_INCLUDED

#include <ndnboost/config.hpp>
#include <ndnboost/iostreams/detail/config/windows_posix.hpp>

// Handle open, close, read, and write
#ifdef __BORLANDC__
# define NDNBOOST_IOSTREAMS_RTL(x) NDNBOOST_JOIN(_rtl_, x)
#elif defined NDNBOOST_IOSTREAMS_WINDOWS
# define NDNBOOST_IOSTREAMS_RTL(x) NDNBOOST_JOIN(_, x)
#else
# define NDNBOOST_IOSTREAMS_RTL(x) ::x  // Distinguish from member function named x
#endif
#define NDNBOOST_IOSTREAMS_FD_OPEN   NDNBOOST_IOSTREAMS_RTL(open)
#define NDNBOOST_IOSTREAMS_FD_CLOSE  NDNBOOST_IOSTREAMS_RTL(close)
#define NDNBOOST_IOSTREAMS_FD_READ   NDNBOOST_IOSTREAMS_RTL(read)
#define NDNBOOST_IOSTREAMS_FD_WRITE  NDNBOOST_IOSTREAMS_RTL(write)

// Handle lseek, off_t, ftruncate, and stat
#ifdef NDNBOOST_IOSTREAMS_WINDOWS
# if defined(NDNBOOST_MSVC) || defined(__MSVCRT__) // MSVC, MinGW
#  define NDNBOOST_IOSTREAMS_FD_SEEK    _lseeki64
#  define NDNBOOST_IOSTREAMS_FD_OFFSET  __int64
# else                                          // Borland, Metrowerks, ...
#  define NDNBOOST_IOSTREAMS_FD_SEEK    lseek  
#  define NDNBOOST_IOSTREAMS_FD_OFFSET  long
# endif
#else // Non-windows
# if defined(_LARGEFILE64_SOURCE) && !defined(__APPLE__) && \
         (!defined(_FILE_OFFSET_BITS) || _FILE_OFFSET_BITS != 64) || \
     defined(_AIX) && !defined(_LARGE_FILES) || \
     defined(NDNBOOST_IOSTREAMS_HAS_LARGE_FILE_EXTENSIONS)
     /**/

    /* Systems with transitional extensions for large file support */

#  define NDNBOOST_IOSTREAMS_FD_SEEK      lseek64
#  define NDNBOOST_IOSTREAMS_FD_TRUNCATE  ftruncate64
#  define NDNBOOST_IOSTREAMS_FD_MMAP      mmap64
#  define NDNBOOST_IOSTREAMS_FD_STAT      stat64
#  define NDNBOOST_IOSTREAMS_FD_FSTAT     fstat64
#  define NDNBOOST_IOSTREAMS_FD_OFFSET    off64_t
# else
#  define NDNBOOST_IOSTREAMS_FD_SEEK      lseek
#  define NDNBOOST_IOSTREAMS_FD_TRUNCATE  ftruncate
#  define NDNBOOST_IOSTREAMS_FD_MMAP      mmap
#  define NDNBOOST_IOSTREAMS_FD_STAT      stat
#  define NDNBOOST_IOSTREAMS_FD_FSTAT     fstat
#  define NDNBOOST_IOSTREAMS_FD_OFFSET    off_t
# endif
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_CONFIG_RTL_HPP_INCLUDED
