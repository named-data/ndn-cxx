//  ndnboost/filesystem/v3/config.hpp  ----------------------------------------------------//

//  Copyright Beman Dawes 2003

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------// 

#ifndef NDNBOOST_FILESYSTEM3_CONFIG_HPP
#define NDNBOOST_FILESYSTEM3_CONFIG_HPP

# if defined(NDNBOOST_FILESYSTEM_VERSION) && NDNBOOST_FILESYSTEM_VERSION != 3
#   error Compiling Filesystem version 3 file with NDNBOOST_FILESYSTEM_VERSION defined != 3
# endif

# if !defined(NDNBOOST_FILESYSTEM_VERSION)
#   define NDNBOOST_FILESYSTEM_VERSION 3
# endif

#define NDNBOOST_FILESYSTEM_I18N  // aid users wishing to compile several versions

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <ndnboost/config.hpp>
#include <ndnboost/system/api_config.hpp>  // for NDNBOOST_POSIX_API or NDNBOOST_WINDOWS_API
#include <ndnboost/detail/workaround.hpp> 

//  NDNBOOST_FILESYSTEM_DEPRECATED needed for source compiles -----------------------------//

# ifdef NDNBOOST_FILESYSTEM_SOURCE
#   define NDNBOOST_FILESYSTEM_DEPRECATED
# endif

//  throw an exception  ----------------------------------------------------------------//
//
//  Exceptions were originally thrown via ndnboost::throw_exception().
//  As throw_exception() became more complex, it caused user error reporting
//  to be harder to interpret, since the exception reported became much more complex.
//  The immediate fix was to throw directly, wrapped in a macro to make any later change
//  easier.

#define NDNBOOST_FILESYSTEM_THROW(EX) throw EX

# if defined( NDNBOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

//  This header implements separate compilation features as described in
//  http://www.boost.org/more/separate_compilation.html

//  normalize macros  ------------------------------------------------------------------//

#if !defined(NDNBOOST_FILESYSTEM_DYN_LINK) && !defined(NDNBOOST_FILESYSTEM_STATIC_LINK) \
  && !defined(NDNBOOST_ALL_DYN_LINK) && !defined(NDNBOOST_ALL_STATIC_LINK)
# define NDNBOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(NDNBOOST_ALL_DYN_LINK) && !defined(NDNBOOST_FILESYSTEM_DYN_LINK)
# define NDNBOOST_FILESYSTEM_DYN_LINK 
#elif defined(NDNBOOST_ALL_STATIC_LINK) && !defined(NDNBOOST_FILESYSTEM_STATIC_LINK)
# define NDNBOOST_FILESYSTEM_STATIC_LINK 
#endif

#if defined(NDNBOOST_FILESYSTEM_DYN_LINK) && defined(NDNBOOST_FILESYSTEM_STATIC_LINK)
# error Must not define both NDNBOOST_FILESYSTEM_DYN_LINK and NDNBOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(NDNBOOST_ALL_NO_LIB) && !defined(NDNBOOST_FILESYSTEM_NO_LIB)
# define NDNBOOST_FILESYSTEM_NO_LIB 
#endif

//  enable dynamic linking  ------------------------------------------------------------//

#if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_FILESYSTEM_DYN_LINK)
# if defined(NDNBOOST_FILESYSTEM_SOURCE)
#   define NDNBOOST_FILESYSTEM_DECL NDNBOOST_SYMBOL_EXPORT
# else 
#   define NDNBOOST_FILESYSTEM_DECL NDNBOOST_SYMBOL_IMPORT
# endif
#else
# define NDNBOOST_FILESYSTEM_DECL
#endif

//  enable automatic library variant selection  ----------------------------------------// 

#if !defined(NDNBOOST_FILESYSTEM_SOURCE) && !defined(NDNBOOST_ALL_NO_LIB) \
  && !defined(NDNBOOST_FILESYSTEM_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define NDNBOOST_LIB_NAME ndnboost_filesystem
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_FILESYSTEM_DYN_LINK)
#  define NDNBOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <ndnboost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif // NDNBOOST_FILESYSTEM3_CONFIG_HPP
