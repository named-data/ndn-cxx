//  (C) Copyright Gennadiy Rozental 2001-2008.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 63441 $
//
//  Description : as a central place for global configuration switches
// ***************************************************************************

#ifndef NDNBOOST_TEST_CONFIG_HPP_071894GER
#define NDNBOOST_TEST_CONFIG_HPP_071894GER

// Boost
#include <ndnboost/config.hpp> // compilers workarounds
#include <ndnboost/detail/workaround.hpp>

//____________________________________________________________________________//

#if NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x570)) || \
    NDNBOOST_WORKAROUND(__IBMCPP__, NDNBOOST_TESTED_AT(600))     || \
    (defined __sgi && NDNBOOST_WORKAROUND(_COMPILER_VERSION, NDNBOOST_TESTED_AT(730)))
#  define NDNBOOST_TEST_SHIFTED_LINE
#endif

//____________________________________________________________________________//

#if defined(NDNBOOST_MSVC) || (defined(__BORLANDC__) && !defined(NDNBOOST_DISABLE_WIN32))
#  define NDNBOOST_TEST_CALL_DECL __cdecl
#else
#  define NDNBOOST_TEST_CALL_DECL /**/
#endif

//____________________________________________________________________________//

#if !defined(NDNBOOST_NO_STD_LOCALE) &&            \
    !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1310)  &&   \
    !defined(__MWERKS__) 
#  define NDNBOOST_TEST_USE_STD_LOCALE 1
#endif

//____________________________________________________________________________//

#if NDNBOOST_WORKAROUND(__BORLANDC__, <= 0x570)            || \
    NDNBOOST_WORKAROUND( __COMO__, <= 0x433 )              || \
    NDNBOOST_WORKAROUND( __INTEL_COMPILER, <= 800 )        || \
    defined(__sgi) && _COMPILER_VERSION <= 730          || \
    NDNBOOST_WORKAROUND(__IBMCPP__, NDNBOOST_TESTED_AT(600))  || \
    defined(__DECCXX)                                   || \
    defined(__DMC__)
#  define NDNBOOST_TEST_NO_PROTECTED_USING
#endif

//____________________________________________________________________________//

#if defined(__GNUC__) || NDNBOOST_WORKAROUND(NDNBOOST_MSVC, == 1400)
#define NDNBOOST_TEST_PROTECTED_VIRTUAL virtual
#else
#define NDNBOOST_TEST_PROTECTED_VIRTUAL
#endif

//____________________________________________________________________________//

#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <1310) && \
    !NDNBOOST_WORKAROUND(__SUNPRO_CC, NDNBOOST_TESTED_AT(0x530))
#  define NDNBOOST_TEST_SUPPORT_INTERACTION_TESTING 1
#endif

//____________________________________________________________________________//

#if defined(NDNBOOST_ALL_DYN_LINK) && !defined(NDNBOOST_TEST_DYN_LINK)
#  define NDNBOOST_TEST_DYN_LINK
#endif

#if defined(NDNBOOST_TEST_INCLUDED)
#  undef NDNBOOST_TEST_DYN_LINK
#endif

#if defined(NDNBOOST_TEST_DYN_LINK)
#  define NDNBOOST_TEST_ALTERNATIVE_INIT_API

#  ifdef NDNBOOST_TEST_SOURCE
#    define NDNBOOST_TEST_DECL NDNBOOST_SYMBOL_EXPORT
#  else
#    define NDNBOOST_TEST_DECL NDNBOOST_SYMBOL_IMPORT
#  endif  // NDNBOOST_TEST_SOURCE
#else
#  define NDNBOOST_TEST_DECL
#endif

#if !defined(NDNBOOST_TEST_MAIN) && defined(NDNBOOST_AUTO_TEST_MAIN)
#define NDNBOOST_TEST_MAIN NDNBOOST_AUTO_TEST_MAIN
#endif

#if !defined(NDNBOOST_TEST_MAIN) && defined(NDNBOOST_TEST_MODULE)
#define NDNBOOST_TEST_MAIN NDNBOOST_TEST_MODULE
#endif

#endif // NDNBOOST_TEST_CONFIG_HPP_071894GER
