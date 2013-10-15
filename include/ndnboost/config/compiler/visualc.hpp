//  (C) Copyright John Maddock 2001 - 2003. 
//  (C) Copyright Darin Adler 2001 - 2002. 
//  (C) Copyright Peter Dimov 2001. 
//  (C) Copyright Aleksey Gurtovoy 2002. 
//  (C) Copyright David Abrahams 2002 - 2003. 
//  (C) Copyright Beman Dawes 2002 - 2003. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.
//
//  Microsoft Visual C++ compiler setup:
//
//  We need to be careful with the checks in this file, as contrary
//  to popular belief there are versions with _MSC_VER with the final
//  digit non-zero (mainly the MIPS cross compiler).
//
//  So we either test _MSC_VER >= XXXX or else _MSC_VER < XXXX.
//  No other comparisons (==, >, or <=) are safe.
//

#define NDNBOOST_MSVC _MSC_VER

//
// Helper macro NDNBOOST_MSVC_FULL_VER for use in Boost code:
//
#if _MSC_FULL_VER > 100000000
#  define NDNBOOST_MSVC_FULL_VER _MSC_FULL_VER
#else
#  define NDNBOOST_MSVC_FULL_VER (_MSC_FULL_VER * 10)
#endif

// Attempt to suppress VC6 warnings about the length of decorated names (obsolete):
#pragma warning( disable : 4503 ) // warning: decorated name length exceeded

//
// versions check:
// we don't support Visual C++ prior to version 6:
#if _MSC_VER < 1200
#  error "Compiler not supported or configured - please reconfigure"
#endif

#if _MSC_VER < 1300  // 1200 == VC++ 6.0, 1200-1202 == eVC++4
#  pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#  define NDNBOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#  define NDNBOOST_NO_VOID_RETURNS
#  define NDNBOOST_NO_EXCEPTION_STD_NAMESPACE

#  if _MSC_VER == 1202
#    define NDNBOOST_NO_STD_TYPEINFO
#  endif

#endif

/// Visual Studio has no fenv.h
#define NDNBOOST_NO_FENV_H

#if (_MSC_VER < 1310)  // 130X == VC++ 7.0

#  if !defined(_MSC_EXTENSIONS) && !defined(NDNBOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS)      // VC7 bug with /Za
#    define NDNBOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#  endif

#  define NDNBOOST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
#  define NDNBOOST_NO_INCLASS_MEMBER_INITIALIZATION
#  define NDNBOOST_NO_PRIVATE_IN_AGGREGATE
#  define NDNBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
#  define NDNBOOST_NO_INTEGRAL_INT64_T
#  define NDNBOOST_NO_DEDUCED_TYPENAME
#  define NDNBOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE

//    VC++ 6/7 has member templates but they have numerous problems including
//    cases of silent failure, so for safety we define:
#  define NDNBOOST_NO_MEMBER_TEMPLATES
//    For VC++ experts wishing to attempt workarounds, we define:
#  define NDNBOOST_MSVC6_MEMBER_TEMPLATES

#  define NDNBOOST_NO_MEMBER_TEMPLATE_FRIENDS
#  define NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#  define NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
#  define NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#  define NDNBOOST_NO_USING_TEMPLATE
#  define NDNBOOST_NO_SWPRINTF
#  define NDNBOOST_NO_TEMPLATE_TEMPLATES
#  define NDNBOOST_NO_SFINAE
#  define NDNBOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
#  define NDNBOOST_NO_IS_ABSTRACT
#  define NDNBOOST_NO_FUNCTION_TYPE_SPECIALIZATIONS
// TODO: what version is meant here? Have there really been any fixes in cl 12.01 (as e.g. shipped with eVC4)?
#  if (_MSC_VER >= 1300)
#     define NDNBOOST_NO_MEMBER_FUNCTION_SPECIALIZATIONS
#  endif

#endif

#if _MSC_VER < 1400 
// although a conforming signature for swprint exists in VC7.1
// it appears not to actually work:
#  define NDNBOOST_NO_SWPRINTF
// Our extern template tests also fail for this compiler:
#  define NDNBOOST_NO_CXX11_EXTERN_TEMPLATE
// Variadic macros do not exist for VC7.1 and lower
#  define NDNBOOST_NO_CXX11_VARIADIC_MACROS
#endif

#if defined(UNDER_CE)
// Windows CE does not have a conforming signature for swprintf
#  define NDNBOOST_NO_SWPRINTF
#endif

#if _MSC_VER < 1500  // 140X == VC++ 8.0
#  define NDNBOOST_NO_MEMBER_TEMPLATE_FRIENDS
#endif

#if _MSC_VER < 1600  // 150X == VC++ 9.0
   // A bug in VC9:
#  define NDNBOOST_NO_ADL_BARRIER
#endif


// MSVC (including the latest checked version) has not yet completely 
// implemented value-initialization, as is reported:
// "VC++ does not value-initialize members of derived classes without 
// user-declared constructor", reported in 2009 by Sylvester Hesp:
// https://connect.microsoft.com/VisualStudio/feedback/details/484295
// "Presence of copy constructor breaks member class initialization",
// reported in 2009 by Alex Vakulenko:
// https://connect.microsoft.com/VisualStudio/feedback/details/499606
// "Value-initialization in new-expression", reported in 2005 by
// Pavel Kuznetsov (MetaCommunications Engineering):
// https://connect.microsoft.com/VisualStudio/feedback/details/100744
// See also: http://www.boost.org/libs/utility/value_init.htm#compiler_issues
// (Niels Dekker, LKEB, May 2010)
#  define NDNBOOST_NO_COMPLETE_VALUE_INITIALIZATION

#ifndef _NATIVE_WCHAR_T_DEFINED
#  define NDNBOOST_NO_INTRINSIC_WCHAR_T
#endif

#if defined(_WIN32_WCE) || defined(UNDER_CE)
#  define NDNBOOST_NO_SWPRINTF
#endif

// we have ThreadEx or GetSystemTimeAsFileTime unless we're running WindowsCE
#if !defined(_WIN32_WCE) && !defined(UNDER_CE)
#  define NDNBOOST_HAS_THREADEX
#  define NDNBOOST_HAS_GETSYSTEMTIMEASFILETIME
#endif

//   
// check for exception handling support:   
#if !defined(_CPPUNWIND) && !defined(NDNBOOST_NO_EXCEPTIONS)
#  define NDNBOOST_NO_EXCEPTIONS   
#endif 

//
// __int64 support:
//
#if (_MSC_VER >= 1200)
#   define NDNBOOST_HAS_MS_INT64
#endif
#if (_MSC_VER >= 1310) && (defined(_MSC_EXTENSIONS) || (_MSC_VER >= 1400))
#   define NDNBOOST_HAS_LONG_LONG
#else
#   define NDNBOOST_NO_LONG_LONG
#endif
#if (_MSC_VER >= 1400) && !defined(_DEBUG)
#   define NDNBOOST_HAS_NRVO
#endif
//
// disable Win32 API's if compiler extentions are
// turned off:
//
#if !defined(_MSC_EXTENSIONS) && !defined(NDNBOOST_DISABLE_WIN32)
#  define NDNBOOST_DISABLE_WIN32
#endif
#if !defined(_CPPRTTI) && !defined(NDNBOOST_NO_RTTI)
#  define NDNBOOST_NO_RTTI
#endif

//
// TR1 features:
//
#if _MSC_VER >= 1700
// # define NDNBOOST_HAS_TR1_HASH			// don't know if this is true yet.
// # define NDNBOOST_HAS_TR1_TYPE_TRAITS	// don't know if this is true yet.
# define NDNBOOST_HAS_TR1_UNORDERED_MAP
# define NDNBOOST_HAS_TR1_UNORDERED_SET
#endif

//
// C++0x features
//
//   See above for NDNBOOST_NO_LONG_LONG

// C++ features supported by VC++ 10 (aka 2010)
//
#if _MSC_VER < 1600
#  define NDNBOOST_NO_CXX11_AUTO_DECLARATIONS
#  define NDNBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#  define NDNBOOST_NO_CXX11_LAMBDAS
#  define NDNBOOST_NO_CXX11_RVALUE_REFERENCES
#  define NDNBOOST_NO_CXX11_STATIC_ASSERT
#  define NDNBOOST_NO_CXX11_NULLPTR
#  define NDNBOOST_NO_CXX11_DECLTYPE
#endif // _MSC_VER < 1600

#if _MSC_VER >= 1600
#  define NDNBOOST_HAS_STDINT_H
#endif

// C++ features supported by VC++ 11 (aka 2012)
//
#if _MSC_VER < 1700
#  define NDNBOOST_NO_CXX11_RANGE_BASED_FOR
#  define NDNBOOST_NO_CXX11_SCOPED_ENUMS
#endif // _MSC_VER < 1700

// C++11 features supported by VC++ 11 (aka 2012) November 2012 CTP
// Because the CTP is unsupported, unrelease, and only alpha quality,
// it is only supported if NDNBOOST_MSVC_ENABLE_2012_NOV_CTP is defined.
//
#if _MSC_FULL_VER < 170051025 || !defined(NDNBOOST_MSVC_ENABLE_2012_NOV_CTP)
#  define NDNBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#  define NDNBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#  define NDNBOOST_NO_CXX11_RAW_LITERALS
#  define NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES
#  define NDNBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#endif

// C++11 features not supported by any versions
#define NDNBOOST_NO_CXX11_CHAR16_T
#define NDNBOOST_NO_CXX11_CHAR32_T
#define NDNBOOST_NO_CXX11_CONSTEXPR
#define NDNBOOST_NO_CXX11_DECLTYPE_N3276
#define NDNBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define NDNBOOST_NO_CXX11_DELETED_FUNCTIONS
#define NDNBOOST_NO_CXX11_NOEXCEPT
#define NDNBOOST_NO_CXX11_TEMPLATE_ALIASES
#define NDNBOOST_NO_CXX11_UNICODE_LITERALS
#define NDNBOOST_NO_SFINAE_EXPR
#define NDNBOOST_NO_TWO_PHASE_NAME_LOOKUP
#define NDNBOOST_NO_CXX11_USER_DEFINED_LITERALS

//
// prefix and suffix headers:
//
#ifndef NDNBOOST_ABI_PREFIX
#  define NDNBOOST_ABI_PREFIX "ndnboost/config/abi/msvc_prefix.hpp"
#endif
#ifndef NDNBOOST_ABI_SUFFIX
#  define NDNBOOST_ABI_SUFFIX "ndnboost/config/abi/msvc_suffix.hpp"
#endif

#ifndef NDNBOOST_COMPILER
// TODO:
// these things are mostly bogus. 1200 means version 12.0 of the compiler. The 
// artificial versions assigned to them only refer to the versions of some IDE
// these compilers have been shipped with, and even that is not all of it. Some
// were shipped with freely downloadable SDKs, others as crosscompilers in eVC.
// IOW, you can't use these 'versions' in any sensible way. Sorry.
# if defined(UNDER_CE)
#   if _MSC_VER < 1200
      // Note: these are so far off, they are not really supported
#   elif _MSC_VER < 1300 // eVC++ 4 comes with 1200-1202
#     define NDNBOOST_COMPILER_VERSION evc4.0
#   elif _MSC_VER < 1400
      // Note: I'm not aware of any CE compiler with version 13xx
#      if defined(NDNBOOST_ASSERT_CONFIG)
#         error "Unknown EVC++ compiler version - please run the configure tests and report the results"
#      else
#         pragma message("Unknown EVC++ compiler version - please run the configure tests and report the results")
#      endif
#   elif _MSC_VER < 1500
#     define NDNBOOST_COMPILER_VERSION evc8
#   elif _MSC_VER < 1600
#     define NDNBOOST_COMPILER_VERSION evc9
#   elif _MSC_VER < 1700
#     define NDNBOOST_COMPILER_VERSION evc10
#   elif _MSC_VER < 1800 
#     define NDNBOOST_COMPILER_VERSION evc11 
#   else
#      if defined(NDNBOOST_ASSERT_CONFIG)
#         error "Unknown EVC++ compiler version - please run the configure tests and report the results"
#      else
#         pragma message("Unknown EVC++ compiler version - please run the configure tests and report the results")
#      endif
#   endif
# else
#   if _MSC_VER < 1200
      // Note: these are so far off, they are not really supported
#     define NDNBOOST_COMPILER_VERSION 5.0
#   elif _MSC_VER < 1300
#       define NDNBOOST_COMPILER_VERSION 6.0
#   elif _MSC_VER < 1310
#     define NDNBOOST_COMPILER_VERSION 7.0
#   elif _MSC_VER < 1400
#     define NDNBOOST_COMPILER_VERSION 7.1
#   elif _MSC_VER < 1500
#     define NDNBOOST_COMPILER_VERSION 8.0
#   elif _MSC_VER < 1600
#     define NDNBOOST_COMPILER_VERSION 9.0
#   elif _MSC_VER < 1700
#     define NDNBOOST_COMPILER_VERSION 10.0
#   elif _MSC_VER < 1800 
#     define NDNBOOST_COMPILER_VERSION 11.0 
#   else
#     define NDNBOOST_COMPILER_VERSION _MSC_VER
#   endif
# endif

#  define NDNBOOST_COMPILER "Microsoft Visual C++ version " NDNBOOST_STRINGIZE(NDNBOOST_COMPILER_VERSION)
#endif

//
// last known and checked version is 1700 (VC11, aka 2011):
#if (_MSC_VER > 1700)
#  if defined(NDNBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  else
#     pragma message("Unknown compiler version - please run the configure tests and report the results")
#  endif
#endif
