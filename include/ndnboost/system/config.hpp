//  ndnboost/system/config.hpp  -----------------------------------------------------------//

//  Copyright Beman Dawes 2003, 2006

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/system for documentation.

#ifndef NDNBOOST_SYSTEM_CONFIG_HPP                  
#define NDNBOOST_SYSTEM_CONFIG_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/system/api_config.hpp>  // for NDNBOOST_POSIX_API or NDNBOOST_WINDOWS_API

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

//  normalize macros  ------------------------------------------------------------------//

#if !defined(NDNBOOST_SYSTEM_DYN_LINK) && !defined(NDNBOOST_SYSTEM_STATIC_LINK) \
  && !defined(NDNBOOST_ALL_DYN_LINK) && !defined(NDNBOOST_ALL_STATIC_LINK)
# define NDNBOOST_SYSTEM_STATIC_LINK
#endif

#if defined(NDNBOOST_ALL_DYN_LINK) && !defined(NDNBOOST_SYSTEM_DYN_LINK)
# define NDNBOOST_SYSTEM_DYN_LINK 
#elif defined(NDNBOOST_ALL_STATIC_LINK) && !defined(NDNBOOST_SYSTEM_STATIC_LINK)
# define NDNBOOST_SYSTEM_STATIC_LINK 
#endif

#if defined(NDNBOOST_SYSTEM_DYN_LINK) && defined(NDNBOOST_SYSTEM_STATIC_LINK)
# error Must not define both NDNBOOST_SYSTEM_DYN_LINK and NDNBOOST_SYSTEM_STATIC_LINK
#endif

//  enable dynamic or static linking as requested --------------------------------------//

#if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_SYSTEM_DYN_LINK)
# if defined(NDNBOOST_SYSTEM_SOURCE)
#   define NDNBOOST_SYSTEM_DECL NDNBOOST_SYMBOL_EXPORT
# else 
#   define NDNBOOST_SYSTEM_DECL NDNBOOST_SYMBOL_IMPORT
# endif
#else
# define NDNBOOST_SYSTEM_DECL
#endif

//  enable automatic library variant selection  ----------------------------------------// 

#if !defined(NDNBOOST_SYSTEM_SOURCE) && !defined(NDNBOOST_ALL_NO_LIB) && !defined(NDNBOOST_SYSTEM_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define NDNBOOST_LIB_NAME ndnboost_system
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_SYSTEM_DYN_LINK)
#  define NDNBOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <ndnboost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif // NDNBOOST_SYSTEM_CONFIG_HPP

