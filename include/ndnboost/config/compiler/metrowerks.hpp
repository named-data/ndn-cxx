//  (C) Copyright John Maddock 2001. 
//  (C) Copyright Darin Adler 2001. 
//  (C) Copyright Peter Dimov 2001. 
//  (C) Copyright David Abrahams 2001 - 2002. 
//  (C) Copyright Beman Dawes 2001 - 2003. 
//  (C) Copyright Stefan Slapeta 2004. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Metrowerks C++ compiler setup:

// locale support is disabled when linking with the dynamic runtime
#   ifdef _MSL_NO_LOCALE
#     define NDNBOOST_NO_STD_LOCALE
#   endif 

#   if __MWERKS__ <= 0x2301  // 5.3
#     define NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#     define NDNBOOST_NO_POINTER_TO_MEMBER_CONST
#     define NDNBOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#     define NDNBOOST_NO_MEMBER_TEMPLATE_KEYWORD
#   endif

#   if __MWERKS__ <= 0x2401  // 6.2
//#     define NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#   endif

#   if(__MWERKS__ <= 0x2407)  // 7.x
#     define NDNBOOST_NO_MEMBER_FUNCTION_SPECIALIZATIONS
#     define NDNBOOST_NO_UNREACHABLE_RETURN_DETECTION
#   endif

#   if(__MWERKS__ <= 0x3003)  // 8.x
#     define NDNBOOST_NO_SFINAE
#    endif

// the "|| !defined(NDNBOOST_STRICT_CONFIG)" part should apply to the last
// tested version *only*:
#   if(__MWERKS__ <= 0x3207) || !defined(NDNBOOST_STRICT_CONFIG) // 9.6
#     define NDNBOOST_NO_MEMBER_TEMPLATE_FRIENDS
#     define NDNBOOST_NO_IS_ABSTRACT
#    endif

#if !__option(wchar_type)
#   define NDNBOOST_NO_INTRINSIC_WCHAR_T
#endif

#if !__option(exceptions) && !defined(NDNBOOST_NO_EXCEPTIONS)
#   define NDNBOOST_NO_EXCEPTIONS
#endif

#if (__INTEL__ && _WIN32) || (__POWERPC__ && macintosh)
#   if __MWERKS__ == 0x3000
#     define NDNBOOST_COMPILER_VERSION 8.0
#   elif __MWERKS__ == 0x3001
#     define NDNBOOST_COMPILER_VERSION 8.1
#   elif __MWERKS__ == 0x3002
#     define NDNBOOST_COMPILER_VERSION 8.2
#   elif __MWERKS__ == 0x3003
#     define NDNBOOST_COMPILER_VERSION 8.3
#   elif __MWERKS__ == 0x3200
#     define NDNBOOST_COMPILER_VERSION 9.0
#   elif __MWERKS__ == 0x3201
#     define NDNBOOST_COMPILER_VERSION 9.1
#   elif __MWERKS__ == 0x3202
#     define NDNBOOST_COMPILER_VERSION 9.2
#   elif __MWERKS__ == 0x3204
#     define NDNBOOST_COMPILER_VERSION 9.3
#   elif __MWERKS__ == 0x3205
#     define NDNBOOST_COMPILER_VERSION 9.4
#   elif __MWERKS__ == 0x3206
#     define NDNBOOST_COMPILER_VERSION 9.5
#   elif __MWERKS__ == 0x3207
#     define NDNBOOST_COMPILER_VERSION 9.6
#   else
#     define NDNBOOST_COMPILER_VERSION __MWERKS__
#   endif
#else
#  define NDNBOOST_COMPILER_VERSION __MWERKS__
#endif

//
// C++0x features
//
//   See boost\config\suffix.hpp for NDNBOOST_NO_LONG_LONG
//
#if __MWERKS__ > 0x3206 && __option(rvalue_refs)
#  define NDNBOOST_HAS_RVALUE_REFS
#else
#  define NDNBOOST_NO_CXX11_RVALUE_REFERENCES              
#endif
#define NDNBOOST_NO_CXX11_AUTO_DECLARATIONS
#define NDNBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define NDNBOOST_NO_CXX11_CHAR16_T
#define NDNBOOST_NO_CXX11_CHAR32_T
#define NDNBOOST_NO_CXX11_CONSTEXPR
#define NDNBOOST_NO_CXX11_DECLTYPE
#define NDNBOOST_NO_CXX11_DECLTYPE_N3276
#define NDNBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define NDNBOOST_NO_CXX11_DELETED_FUNCTIONS
#define NDNBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#define NDNBOOST_NO_CXX11_EXTERN_TEMPLATE
#define NDNBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#define NDNBOOST_NO_CXX11_LAMBDAS
#define NDNBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define NDNBOOST_NO_CXX11_NOEXCEPT
#define NDNBOOST_NO_CXX11_NULLPTR
#define NDNBOOST_NO_CXX11_RANGE_BASED_FOR
#define NDNBOOST_NO_CXX11_RAW_LITERALS
#define NDNBOOST_NO_CXX11_SCOPED_ENUMS
#define NDNBOOST_NO_SFINAE_EXPR
#define NDNBOOST_NO_CXX11_STATIC_ASSERT
#define NDNBOOST_NO_CXX11_TEMPLATE_ALIASES
#define NDNBOOST_NO_CXX11_UNICODE_LITERALS
#define NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES
#define NDNBOOST_NO_CXX11_VARIADIC_MACROS
#define NDNBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define NDNBOOST_NO_CXX11_USER_DEFINED_LITERALS

#define NDNBOOST_COMPILER "Metrowerks CodeWarrior C++ version " NDNBOOST_STRINGIZE(NDNBOOST_COMPILER_VERSION)

//
// versions check:
// we don't support Metrowerks prior to version 5.3:
#if __MWERKS__ < 0x2301
#  error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version:
#if (__MWERKS__ > 0x3205)
#  if defined(NDNBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif







