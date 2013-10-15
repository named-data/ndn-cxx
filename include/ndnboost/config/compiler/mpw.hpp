//  (C) Copyright John Maddock 2001 - 2002. 
//  (C) Copyright Aleksey Gurtovoy 2002. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  MPW C++ compilers setup:

#   if    defined(__SC__)
#     define NDNBOOST_COMPILER "MPW SCpp version " NDNBOOST_STRINGIZE(__SC__)
#   elif defined(__MRC__)
#     define NDNBOOST_COMPILER "MPW MrCpp version " NDNBOOST_STRINGIZE(__MRC__)
#   else
#     error "Using MPW compiler configuration by mistake.  Please update."
#   endif

//
// MPW 8.90:
//
#if (MPW_CPLUS <= 0x890) || !defined(NDNBOOST_STRICT_CONFIG)
#  define NDNBOOST_NO_CV_SPECIALIZATIONS
#  define NDNBOOST_NO_DEPENDENT_NESTED_DERIVATIONS
#  define NDNBOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#  define NDNBOOST_NO_INCLASS_MEMBER_INITIALIZATION
#  define NDNBOOST_NO_INTRINSIC_WCHAR_T
#  define NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#  define NDNBOOST_NO_USING_TEMPLATE

#  define NDNBOOST_NO_CWCHAR
#  define NDNBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS

#  define NDNBOOST_NO_STD_ALLOCATOR /* actually a bug with const reference overloading */

#endif

//
// C++0x features
//
//   See boost\config\suffix.hpp for NDNBOOST_NO_LONG_LONG
//
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
#define NDNBOOST_NO_CXX11_RVALUE_REFERENCES
#define NDNBOOST_NO_CXX11_SCOPED_ENUMS
#define NDNBOOST_NO_SFINAE_EXPR
#define NDNBOOST_NO_CXX11_STATIC_ASSERT
#define NDNBOOST_NO_CXX11_TEMPLATE_ALIASES
#define NDNBOOST_NO_CXX11_UNICODE_LITERALS
#define NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES
#define NDNBOOST_NO_CXX11_VARIADIC_MACROS
#define NDNBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define NDNBOOST_NO_CXX11_USER_DEFINED_LITERALS

//
// versions check:
// we don't support MPW prior to version 8.9:
#if MPW_CPLUS < 0x890
#  error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version is 0x890:
#if (MPW_CPLUS > 0x890)
#  if defined(NDNBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif


