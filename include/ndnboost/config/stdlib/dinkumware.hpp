//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Jens Maurer 2001.
//  (C) Copyright Peter Dimov 2001.
//  (C) Copyright David Abrahams 2002.
//  (C) Copyright Guillaume Melquiond 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Dinkumware standard library config:

#if !defined(_YVALS) && !defined(_CPPLIB_VER)
#include <ndnboost/config/no_tr1/utility.hpp>
#if !defined(_YVALS) && !defined(_CPPLIB_VER)
#error This is not the Dinkumware lib!
#endif
#endif


#if defined(_CPPLIB_VER) && (_CPPLIB_VER >= 306)
   // full dinkumware 3.06 and above
   // fully conforming provided the compiler supports it:
#  if !(defined(_GLOBAL_USING) && (_GLOBAL_USING+0 > 0)) && !defined(__BORLANDC__) && !defined(_STD) && !(defined(__ICC) && (__ICC >= 700))   // can be defined in yvals.h
#     define NDNBOOST_NO_STDC_NAMESPACE
#  endif
#  if !(defined(_HAS_MEMBER_TEMPLATES_REBIND) && (_HAS_MEMBER_TEMPLATES_REBIND+0 > 0)) && !(defined(_MSC_VER) && (_MSC_VER > 1300)) && defined(NDNBOOST_MSVC)
#     define NDNBOOST_NO_STD_ALLOCATOR
#  endif
#  define NDNBOOST_HAS_PARTIAL_STD_ALLOCATOR
#  if defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1300)
      // if this lib version is set up for vc6 then there is no std::use_facet:
#     define NDNBOOST_NO_STD_USE_FACET
#     define NDNBOOST_HAS_TWO_ARG_USE_FACET
      // C lib functions aren't in namespace std either:
#     define NDNBOOST_NO_STDC_NAMESPACE
      // and nor is <exception>
#     define NDNBOOST_NO_EXCEPTION_STD_NAMESPACE
#  endif
// There's no numeric_limits<long long> support unless _LONGLONG is defined:
#  if !defined(_LONGLONG) && (_CPPLIB_VER <= 310)
#     define NDNBOOST_NO_MS_INT64_NUMERIC_LIMITS
#  endif
// 3.06 appears to have (non-sgi versions of) <hash_set> & <hash_map>,
// and no <slist> at all
#else
#  define NDNBOOST_MSVC_STD_ITERATOR 1
#  define NDNBOOST_NO_STD_ITERATOR
#  define NDNBOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
#  define NDNBOOST_NO_STD_ALLOCATOR
#  define NDNBOOST_NO_STDC_NAMESPACE
#  define NDNBOOST_NO_STD_USE_FACET
#  define NDNBOOST_NO_STD_OUTPUT_ITERATOR_ASSIGN
#  define NDNBOOST_HAS_MACRO_USE_FACET
#  ifndef _CPPLIB_VER
      // Updated Dinkum library defines this, and provides
      // its own min and max definitions, as does MTA version.
#     ifndef __MTA__ 
#        define NDNBOOST_NO_STD_MIN_MAX
#     endif
#     define NDNBOOST_NO_MS_INT64_NUMERIC_LIMITS
#  endif
#endif

//
// std extension namespace is stdext for vc7.1 and later, 
// the same applies to other compilers that sit on top
// of vc7.1 (Intel and Comeau):
//
#if defined(_MSC_VER) && (_MSC_VER >= 1310) && !defined(__BORLANDC__)
#  define NDNBOOST_STD_EXTENSION_NAMESPACE stdext
#endif


#if (defined(_MSC_VER) && (_MSC_VER <= 1300) && !defined(__BORLANDC__)) || !defined(_CPPLIB_VER) || (_CPPLIB_VER < 306)
   // if we're using a dinkum lib that's
   // been configured for VC6/7 then there is
   // no iterator traits (true even for icl)
#  define NDNBOOST_NO_STD_ITERATOR_TRAITS
#endif

#if defined(__ICL) && (__ICL < 800) && defined(_CPPLIB_VER) && (_CPPLIB_VER <= 310)
// Intel C++ chokes over any non-trivial use of <locale>
// this may be an overly restrictive define, but regex fails without it:
#  define NDNBOOST_NO_STD_LOCALE
#endif

#include <typeinfo>
#if ( (!_HAS_EXCEPTIONS && !defined(__ghs__)) || (!_HAS_NAMESPACE && defined(__ghs__)) ) && !defined(__TI_COMPILER_VERSION__)
#  define NDNBOOST_NO_STD_TYPEINFO    
#endif  

//  C++0x headers implemented in 520 (as shipped by Microsoft)
//
#if !defined(_CPPLIB_VER) || _CPPLIB_VER < 520
#  define NDNBOOST_NO_CXX11_HDR_ARRAY
#  define NDNBOOST_NO_CXX11_HDR_CODECVT
#  define NDNBOOST_NO_CXX11_HDR_FORWARD_LIST
#  define NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define NDNBOOST_NO_CXX11_HDR_RANDOM
#  define NDNBOOST_NO_CXX11_HDR_REGEX
#  define NDNBOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define NDNBOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define NDNBOOST_NO_CXX11_HDR_UNORDERED_SET
#  define NDNBOOST_NO_CXX11_HDR_TUPLE
#  define NDNBOOST_NO_CXX11_HDR_TYPEINDEX
#  define NDNBOOST_NO_CXX11_HDR_FUNCTIONAL
#  define NDNBOOST_NO_CXX11_NUMERIC_LIMITS
#  define NDNBOOST_NO_CXX11_SMART_PTR
#endif

#if (!defined(_HAS_TR1_IMPORTS) || (_HAS_TR1_IMPORTS+0 == 0)) && !defined(NDNBOOST_NO_CXX11_HDR_TUPLE)
#  define NDNBOOST_NO_CXX11_HDR_TUPLE
#endif

//  C++0x headers implemented in 540 (as shipped by Microsoft)
//
#if !defined(_CPPLIB_VER) || _CPPLIB_VER < 540
#  define NDNBOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define NDNBOOST_NO_CXX11_HDR_CHRONO
#  define NDNBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define NDNBOOST_NO_CXX11_HDR_FUTURE
#  define NDNBOOST_NO_CXX11_HDR_MUTEX
#  define NDNBOOST_NO_CXX11_HDR_RATIO
#  define NDNBOOST_NO_CXX11_HDR_THREAD
#  define NDNBOOST_NO_CXX11_ALLOCATOR
#  define NDNBOOST_NO_CXX11_ATOMIC_SMART_PTR
#endif

//
//  C++0x headers not yet (fully) implemented:
//
#  define NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST

#ifdef _CPPLIB_VER
#  define NDNBOOST_DINKUMWARE_STDLIB _CPPLIB_VER
#else
#  define NDNBOOST_DINKUMWARE_STDLIB 1
#endif

#ifdef _CPPLIB_VER
#  define NDNBOOST_STDLIB "Dinkumware standard library version " NDNBOOST_STRINGIZE(_CPPLIB_VER)
#else
#  define NDNBOOST_STDLIB "Dinkumware standard library version 1.x"
#endif









