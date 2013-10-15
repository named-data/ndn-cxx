//  Boost config.hpp configuration header file  ------------------------------//
//	boostinspect:ndprecated_macros	-- tell the inspect tool to ignore this file

//  Copyright (c) 2001-2003 John Maddock
//  Copyright (c) 2001 Darin Adler
//  Copyright (c) 2001 Peter Dimov
//  Copyright (c) 2002 Bill Kempf 
//  Copyright (c) 2002 Jens Maurer
//  Copyright (c) 2002-2003 David Abrahams
//  Copyright (c) 2003 Gennaro Prota
//  Copyright (c) 2003 Eric Friedman
//  Copyright (c) 2010 Eric Jourdanneau, Joel Falcou
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config/
//
//  This file is intended to be stable, and relatively unchanging.
//  It should contain boilerplate code only - no compiler specific
//  code unless it is unavoidable - no changes unless unavoidable.

#ifndef NDNBOOST_CONFIG_SUFFIX_HPP
#define NDNBOOST_CONFIG_SUFFIX_HPP

#if defined(__GNUC__) && (__GNUC__ >= 4)
//
// Some GCC-4.x versions issue warnings even when __extension__ is used,
// so use this as a workaround:
//
#pragma GCC system_header
#endif

//
// ensure that visibility macros are always defined, thus symplifying use
//
#ifndef NDNBOOST_SYMBOL_EXPORT
# define NDNBOOST_SYMBOL_EXPORT
#endif
#ifndef NDNBOOST_SYMBOL_IMPORT
# define NDNBOOST_SYMBOL_IMPORT
#endif
#ifndef NDNBOOST_SYMBOL_VISIBLE
# define NDNBOOST_SYMBOL_VISIBLE
#endif

//
// look for long long by looking for the appropriate macros in <limits.h>.
// Note that we use limits.h rather than climits for maximal portability,
// remember that since these just declare a bunch of macros, there should be
// no namespace issues from this.
//
#if !defined(NDNBOOST_HAS_LONG_LONG) && !defined(NDNBOOST_NO_LONG_LONG)                                              \
   && !defined(NDNBOOST_MSVC) && !defined(__BORLANDC__)
# include <limits.h>
# if (defined(ULLONG_MAX) || defined(ULONG_LONG_MAX) || defined(ULONGLONG_MAX))
#   define NDNBOOST_HAS_LONG_LONG
# else
#   define NDNBOOST_NO_LONG_LONG
# endif
#endif

// GCC 3.x will clean up all of those nasty macro definitions that
// NDNBOOST_NO_CTYPE_FUNCTIONS is intended to help work around, so undefine
// it under GCC 3.x.
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(NDNBOOST_NO_CTYPE_FUNCTIONS)
#  undef NDNBOOST_NO_CTYPE_FUNCTIONS
#endif

//
// Assume any extensions are in namespace std:: unless stated otherwise:
//
#  ifndef NDNBOOST_STD_EXTENSION_NAMESPACE
#    define NDNBOOST_STD_EXTENSION_NAMESPACE std
#  endif

//
// If cv-qualified specializations are not allowed, then neither are cv-void ones:
//
#  if defined(NDNBOOST_NO_CV_SPECIALIZATIONS) \
      && !defined(NDNBOOST_NO_CV_VOID_SPECIALIZATIONS)
#     define NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
#  endif

//
// If there is no numeric_limits template, then it can't have any compile time
// constants either!
//
#  if defined(NDNBOOST_NO_LIMITS) \
      && !defined(NDNBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS)
#     define NDNBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
#     define NDNBOOST_NO_MS_INT64_NUMERIC_LIMITS
#     define NDNBOOST_NO_LONG_LONG_NUMERIC_LIMITS
#  endif

//
// if there is no long long then there is no specialisation
// for numeric_limits<long long> either:
//
#if !defined(NDNBOOST_HAS_LONG_LONG) && !defined(NDNBOOST_NO_LONG_LONG_NUMERIC_LIMITS)
#  define NDNBOOST_NO_LONG_LONG_NUMERIC_LIMITS
#endif

//
// if there is no __int64 then there is no specialisation
// for numeric_limits<__int64> either:
//
#if !defined(NDNBOOST_HAS_MS_INT64) && !defined(NDNBOOST_NO_MS_INT64_NUMERIC_LIMITS)
#  define NDNBOOST_NO_MS_INT64_NUMERIC_LIMITS
#endif

//
// if member templates are supported then so is the
// VC6 subset of member templates:
//
#  if !defined(NDNBOOST_NO_MEMBER_TEMPLATES) \
       && !defined(NDNBOOST_MSVC6_MEMBER_TEMPLATES)
#     define NDNBOOST_MSVC6_MEMBER_TEMPLATES
#  endif

//
// Without partial specialization, can't test for partial specialisation bugs:
//
#  if defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(NDNBOOST_BCB_PARTIAL_SPECIALIZATION_BUG)
#     define NDNBOOST_BCB_PARTIAL_SPECIALIZATION_BUG
#  endif

//
// Without partial specialization, we can't have array-type partial specialisations:
//
#  if defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
#     define NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS
#  endif

//
// Without partial specialization, std::iterator_traits can't work:
//
#  if defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(NDNBOOST_NO_STD_ITERATOR_TRAITS)
#     define NDNBOOST_NO_STD_ITERATOR_TRAITS
#  endif

//
// Without partial specialization, partial 
// specialization with default args won't work either:
//
#  if defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(NDNBOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS)
#     define NDNBOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS
#  endif

//
// Without member template support, we can't have template constructors
// in the standard library either:
//
#  if defined(NDNBOOST_NO_MEMBER_TEMPLATES) \
      && !defined(NDNBOOST_MSVC6_MEMBER_TEMPLATES) \
      && !defined(NDNBOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS)
#     define NDNBOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
#  endif

//
// Without member template support, we can't have a conforming
// std::allocator template either:
//
#  if defined(NDNBOOST_NO_MEMBER_TEMPLATES) \
      && !defined(NDNBOOST_MSVC6_MEMBER_TEMPLATES) \
      && !defined(NDNBOOST_NO_STD_ALLOCATOR)
#     define NDNBOOST_NO_STD_ALLOCATOR
#  endif

//
// without ADL support then using declarations will break ADL as well:
//
#if defined(NDNBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP) && !defined(NDNBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
#  define NDNBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#endif

//
// Without typeid support we have no dynamic RTTI either:
//
#if defined(NDNBOOST_NO_TYPEID) && !defined(NDNBOOST_NO_RTTI)
#  define NDNBOOST_NO_RTTI
#endif

//
// If we have a standard allocator, then we have a partial one as well:
//
#if !defined(NDNBOOST_NO_STD_ALLOCATOR)
#  define NDNBOOST_HAS_PARTIAL_STD_ALLOCATOR
#endif

//
// We can't have a working std::use_facet if there is no std::locale:
//
#  if defined(NDNBOOST_NO_STD_LOCALE) && !defined(NDNBOOST_NO_STD_USE_FACET)
#     define NDNBOOST_NO_STD_USE_FACET
#  endif

//
// We can't have a std::messages facet if there is no std::locale:
//
#  if defined(NDNBOOST_NO_STD_LOCALE) && !defined(NDNBOOST_NO_STD_MESSAGES)
#     define NDNBOOST_NO_STD_MESSAGES
#  endif

//
// We can't have a working std::wstreambuf if there is no std::locale:
//
#  if defined(NDNBOOST_NO_STD_LOCALE) && !defined(NDNBOOST_NO_STD_WSTREAMBUF)
#     define NDNBOOST_NO_STD_WSTREAMBUF
#  endif

//
// We can't have a <cwctype> if there is no <cwchar>:
//
#  if defined(NDNBOOST_NO_CWCHAR) && !defined(NDNBOOST_NO_CWCTYPE)
#     define NDNBOOST_NO_CWCTYPE
#  endif

//
// We can't have a swprintf if there is no <cwchar>:
//
#  if defined(NDNBOOST_NO_CWCHAR) && !defined(NDNBOOST_NO_SWPRINTF)
#     define NDNBOOST_NO_SWPRINTF
#  endif

//
// If Win32 support is turned off, then we must turn off
// threading support also, unless there is some other
// thread API enabled:
//
#if defined(NDNBOOST_DISABLE_WIN32) && defined(_WIN32) \
   && !defined(NDNBOOST_DISABLE_THREADS) && !defined(NDNBOOST_HAS_PTHREADS)
#  define NDNBOOST_DISABLE_THREADS
#endif

//
// Turn on threading support if the compiler thinks that it's in
// multithreaded mode.  We put this here because there are only a
// limited number of macros that identify this (if there's any missing
// from here then add to the appropriate compiler section):
//
#if (defined(__MT__) || defined(_MT) || defined(_REENTRANT) \
    || defined(_PTHREADS) || defined(__APPLE__) || defined(__DragonFly__)) \
    && !defined(NDNBOOST_HAS_THREADS)
#  define NDNBOOST_HAS_THREADS
#endif

//
// Turn threading support off if NDNBOOST_DISABLE_THREADS is defined:
//
#if defined(NDNBOOST_DISABLE_THREADS) && defined(NDNBOOST_HAS_THREADS)
#  undef NDNBOOST_HAS_THREADS
#endif

//
// Turn threading support off if we don't recognise the threading API:
//
#if defined(NDNBOOST_HAS_THREADS) && !defined(NDNBOOST_HAS_PTHREADS)\
      && !defined(NDNBOOST_HAS_WINTHREADS) && !defined(NDNBOOST_HAS_BETHREADS)\
      && !defined(NDNBOOST_HAS_MPTASKS)
#  undef NDNBOOST_HAS_THREADS
#endif

//
// Turn threading detail macros off if we don't (want to) use threading
//
#ifndef NDNBOOST_HAS_THREADS
#  undef NDNBOOST_HAS_PTHREADS
#  undef NDNBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#  undef NDNBOOST_HAS_PTHREAD_YIELD
#  undef NDNBOOST_HAS_PTHREAD_DELAY_NP
#  undef NDNBOOST_HAS_WINTHREADS
#  undef NDNBOOST_HAS_BETHREADS
#  undef NDNBOOST_HAS_MPTASKS
#endif

//
// If the compiler claims to be C99 conformant, then it had better
// have a <stdint.h>:
//
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#     define NDNBOOST_HAS_STDINT_H
#     ifndef NDNBOOST_HAS_LOG1P
#        define NDNBOOST_HAS_LOG1P
#     endif
#     ifndef NDNBOOST_HAS_EXPM1
#        define NDNBOOST_HAS_EXPM1
#     endif
#  endif

//
// Define NDNBOOST_NO_SLIST and NDNBOOST_NO_HASH if required.
// Note that this is for backwards compatibility only.
//
#  if !defined(NDNBOOST_HAS_SLIST) && !defined(NDNBOOST_NO_SLIST)
#     define NDNBOOST_NO_SLIST
#  endif

#  if !defined(NDNBOOST_HAS_HASH) && !defined(NDNBOOST_NO_HASH)
#     define NDNBOOST_NO_HASH
#  endif

//
// Set NDNBOOST_SLIST_HEADER if not set already:
//
#if defined(NDNBOOST_HAS_SLIST) && !defined(NDNBOOST_SLIST_HEADER)
#  define NDNBOOST_SLIST_HEADER <slist>
#endif

//
// Set NDNBOOST_HASH_SET_HEADER if not set already:
//
#if defined(NDNBOOST_HAS_HASH) && !defined(NDNBOOST_HASH_SET_HEADER)
#  define NDNBOOST_HASH_SET_HEADER <hash_set>
#endif

//
// Set NDNBOOST_HASH_MAP_HEADER if not set already:
//
#if defined(NDNBOOST_HAS_HASH) && !defined(NDNBOOST_HASH_MAP_HEADER)
#  define NDNBOOST_HASH_MAP_HEADER <hash_map>
#endif

//  NDNBOOST_HAS_ABI_HEADERS
//  This macro gets set if we have headers that fix the ABI,
//  and prevent ODR violations when linking to external libraries:
#if defined(NDNBOOST_ABI_PREFIX) && defined(NDNBOOST_ABI_SUFFIX) && !defined(NDNBOOST_HAS_ABI_HEADERS)
#  define NDNBOOST_HAS_ABI_HEADERS
#endif

#if defined(NDNBOOST_HAS_ABI_HEADERS) && defined(NDNBOOST_DISABLE_ABI_HEADERS)
#  undef NDNBOOST_HAS_ABI_HEADERS
#endif

//  NDNBOOST_NO_STDC_NAMESPACE workaround  --------------------------------------//
//  Because std::size_t usage is so common, even in boost headers which do not
//  otherwise use the C library, the <cstddef> workaround is included here so
//  that ugly workaround code need not appear in many other boost headers.
//  NOTE WELL: This is a workaround for non-conforming compilers; <cstddef>
//  must still be #included in the usual places so that <cstddef> inclusion
//  works as expected with standard conforming compilers.  The resulting
//  double inclusion of <cstddef> is harmless.

# if defined(NDNBOOST_NO_STDC_NAMESPACE) && defined(__cplusplus)
#   include <cstddef>
    namespace std { using ::ptrdiff_t; using ::size_t; }
# endif

//  Workaround for the unfortunate min/max macros defined by some platform headers

#define NDNBOOST_PREVENT_MACRO_SUBSTITUTION

#ifndef NDNBOOST_USING_STD_MIN
#  define NDNBOOST_USING_STD_MIN() using std::min
#endif

#ifndef NDNBOOST_USING_STD_MAX
#  define NDNBOOST_USING_STD_MAX() using std::max
#endif

//  NDNBOOST_NO_STD_MIN_MAX workaround  -----------------------------------------//

#  if defined(NDNBOOST_NO_STD_MIN_MAX) && defined(__cplusplus)

namespace std {
  template <class _Tp>
  inline const _Tp& min NDNBOOST_PREVENT_MACRO_SUBSTITUTION (const _Tp& __a, const _Tp& __b) {
    return __b < __a ? __b : __a;
  }
  template <class _Tp>
  inline const _Tp& max NDNBOOST_PREVENT_MACRO_SUBSTITUTION (const _Tp& __a, const _Tp& __b) {
    return  __a < __b ? __b : __a;
  }
}

#  endif

// NDNBOOST_STATIC_CONSTANT workaround --------------------------------------- //
// On compilers which don't allow in-class initialization of static integral
// constant members, we must use enums as a workaround if we want the constants
// to be available at compile-time. This macro gives us a convenient way to
// declare such constants.

#  ifdef NDNBOOST_NO_INCLASS_MEMBER_INITIALIZATION
#       define NDNBOOST_STATIC_CONSTANT(type, assignment) enum { assignment }
#  else
#     define NDNBOOST_STATIC_CONSTANT(type, assignment) static const type assignment
#  endif

// NDNBOOST_USE_FACET / HAS_FACET workaround ----------------------------------//
// When the standard library does not have a conforming std::use_facet there
// are various workarounds available, but they differ from library to library.
// The same problem occurs with has_facet.
// These macros provide a consistent way to access a locale's facets.
// Usage:
//    replace
//       std::use_facet<Type>(loc);
//    with
//       NDNBOOST_USE_FACET(Type, loc);
//    Note do not add a std:: prefix to the front of NDNBOOST_USE_FACET!
//  Use for NDNBOOST_HAS_FACET is analogous.

#if defined(NDNBOOST_NO_STD_USE_FACET)
#  ifdef NDNBOOST_HAS_TWO_ARG_USE_FACET
#     define NDNBOOST_USE_FACET(Type, loc) std::use_facet(loc, static_cast<Type*>(0))
#     define NDNBOOST_HAS_FACET(Type, loc) std::has_facet(loc, static_cast<Type*>(0))
#  elif defined(NDNBOOST_HAS_MACRO_USE_FACET)
#     define NDNBOOST_USE_FACET(Type, loc) std::_USE(loc, Type)
#     define NDNBOOST_HAS_FACET(Type, loc) std::_HAS(loc, Type)
#  elif defined(NDNBOOST_HAS_STLP_USE_FACET)
#     define NDNBOOST_USE_FACET(Type, loc) (*std::_Use_facet<Type >(loc))
#     define NDNBOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#  endif
#else
#  define NDNBOOST_USE_FACET(Type, loc) std::use_facet< Type >(loc)
#  define NDNBOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#endif

// NDNBOOST_NESTED_TEMPLATE workaround ------------------------------------------//
// Member templates are supported by some compilers even though they can't use
// the A::template member<U> syntax, as a workaround replace:
//
// typedef typename A::template rebind<U> binder;
//
// with:
//
// typedef typename A::NDNBOOST_NESTED_TEMPLATE rebind<U> binder;

#ifndef NDNBOOST_NO_MEMBER_TEMPLATE_KEYWORD
#  define NDNBOOST_NESTED_TEMPLATE template
#else
#  define NDNBOOST_NESTED_TEMPLATE
#endif

// NDNBOOST_UNREACHABLE_RETURN(x) workaround -------------------------------------//
// Normally evaluates to nothing, unless NDNBOOST_NO_UNREACHABLE_RETURN_DETECTION
// is defined, in which case it evaluates to return x; Use when you have a return
// statement that can never be reached.

#ifdef NDNBOOST_NO_UNREACHABLE_RETURN_DETECTION
#  define NDNBOOST_UNREACHABLE_RETURN(x) return x;
#else
#  define NDNBOOST_UNREACHABLE_RETURN(x)
#endif

// NDNBOOST_DEDUCED_TYPENAME workaround ------------------------------------------//
//
// Some compilers don't support the use of `typename' for dependent
// types in deduced contexts, e.g.
//
//     template <class T> void f(T, typename T::type);
//                                  ^^^^^^^^
// Replace these declarations with:
//
//     template <class T> void f(T, NDNBOOST_DEDUCED_TYPENAME T::type);

#ifndef NDNBOOST_NO_DEDUCED_TYPENAME
#  define NDNBOOST_DEDUCED_TYPENAME typename
#else
#  define NDNBOOST_DEDUCED_TYPENAME
#endif

#ifndef NDNBOOST_NO_TYPENAME_WITH_CTOR
#  define NDNBOOST_CTOR_TYPENAME typename
#else
#  define NDNBOOST_CTOR_TYPENAME
#endif

// long long workaround ------------------------------------------//
// On gcc (and maybe other compilers?) long long is alway supported
// but it's use may generate either warnings (with -ansi), or errors
// (with -pedantic -ansi) unless it's use is prefixed by __extension__
//
#if defined(NDNBOOST_HAS_LONG_LONG) && defined(__cplusplus)
namespace ndnboost{
#  ifdef __GNUC__
   __extension__ typedef long long long_long_type;
   __extension__ typedef unsigned long long ulong_long_type;
#  else
   typedef long long long_long_type;
   typedef unsigned long long ulong_long_type;
#  endif
}
#endif
// same again for __int128:
#if defined(NDNBOOST_HAS_INT128) && defined(__cplusplus)
namespace ndnboost{
#  ifdef __GNUC__
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
#  else
   typedef __int128 int128_type;
   typedef unsigned __int128 uint128_type;
#  endif
}
#endif

// NDNBOOST_[APPEND_]EXPLICIT_TEMPLATE_[NON_]TYPE macros --------------------------//
//
// Some compilers have problems with function templates whose template
// parameters don't appear in the function parameter list (basically
// they just link one instantiation of the template in the final
// executable). These macros provide a uniform way to cope with the
// problem with no effects on the calling syntax.

// Example:
//
//  #include <iostream>
//  #include <ostream>
//  #include <typeinfo>
//
//  template <int n>
//  void f() { std::cout << n << ' '; }
//
//  template <typename T>
//  void g() { std::cout << typeid(T).name() << ' '; }
//
//  int main() {
//    f<1>();
//    f<2>();
//
//    g<int>();
//    g<double>();
//  }
//
// With VC++ 6.0 the output is:
//
//   2 2 double double
//
// To fix it, write
//
//   template <int n>
//   void f(NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE(int, n)) { ... }
//
//   template <typename T>
//   void g(NDNBOOST_EXPLICIT_TEMPLATE_TYPE(T)) { ... }
//


#if defined(NDNBOOST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS) && defined(__cplusplus)

#  include "ndnboost/type.hpp"
#  include "ndnboost/non_type.hpp"

#  define NDNBOOST_EXPLICIT_TEMPLATE_TYPE(t)              ndnboost::type<t>* = 0
#  define NDNBOOST_EXPLICIT_TEMPLATE_TYPE_SPEC(t)         ndnboost::type<t>*
#  define NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE(t, v)       ndnboost::non_type<t, v>* = 0
#  define NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)  ndnboost::non_type<t, v>*

#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(t)        \
             , NDNBOOST_EXPLICIT_TEMPLATE_TYPE(t)
#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE_SPEC(t)   \
             , NDNBOOST_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE(t, v) \
             , NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)    \
             , NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)

#else

// no workaround needed: expand to nothing

#  define NDNBOOST_EXPLICIT_TEMPLATE_TYPE(t)
#  define NDNBOOST_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define NDNBOOST_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)

#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(t)
#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define NDNBOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)


#endif // defined NDNBOOST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS

// When NDNBOOST_NO_STD_TYPEINFO is defined, we can just import
// the global definition into std namespace:
#if defined(NDNBOOST_NO_STD_TYPEINFO) && defined(__cplusplus)
#include <typeinfo>
namespace std{ using ::type_info; }
#endif

// ---------------------------------------------------------------------------//

//
// Helper macro NDNBOOST_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define NDNBOOST_STRINGIZE(X) NDNBOOST_DO_STRINGIZE(X)
#define NDNBOOST_DO_STRINGIZE(X) #X

//
// Helper macro NDNBOOST_JOIN:
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in NDNBOOST_DO_JOIN2 but does in NDNBOOST_DO_JOIN.
//
#define NDNBOOST_JOIN( X, Y ) NDNBOOST_DO_JOIN( X, Y )
#define NDNBOOST_DO_JOIN( X, Y ) NDNBOOST_DO_JOIN2(X,Y)
#define NDNBOOST_DO_JOIN2( X, Y ) X##Y

//
// Set some default values for compiler/library/platform names.
// These are for debugging config setup only:
//
#  ifndef NDNBOOST_COMPILER
#     define NDNBOOST_COMPILER "Unknown ISO C++ Compiler"
#  endif
#  ifndef NDNBOOST_STDLIB
#     define NDNBOOST_STDLIB "Unknown ISO standard library"
#  endif
#  ifndef NDNBOOST_PLATFORM
#     if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) \
         || defined(_POSIX_SOURCE)
#        define NDNBOOST_PLATFORM "Generic Unix"
#     else
#        define NDNBOOST_PLATFORM "Unknown"
#     endif
#  endif

//
// Set some default values GPU support
//
#  ifndef NDNBOOST_GPU_ENABLED
#  define NDNBOOST_GPU_ENABLED 
#  endif

// NDNBOOST_FORCEINLINE ---------------------------------------------//
// Macro to use in place of 'inline' to force a function to be inline
#if !defined(NDNBOOST_FORCEINLINE)
#  if defined(_MSC_VER)
#    define NDNBOOST_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    define NDNBOOST_FORCEINLINE inline __attribute__ ((__always_inline__))
#  else
#    define NDNBOOST_FORCEINLINE inline
#  endif
#endif

//
// Set NDNBOOST_NO_DECLTYPE_N3276 when NDNBOOST_NO_DECLTYPE is defined
//
#if defined(NDNBOOST_NO_CXX11_DECLTYPE) && !defined(NDNBOOST_NO_CXX11_DECLTYPE_N3276)
#define	NDNBOOST_NO_CXX11_DECLTYPE_N3276 NDNBOOST_NO_CXX11_DECLTYPE
#endif

//  -------------------- Deprecated macros for 1.50 ---------------------------
//  These will go away in a future release

//  Use NDNBOOST_NO_CXX11_HDR_UNORDERED_SET or NDNBOOST_NO_CXX11_HDR_UNORDERED_MAP
//           instead of NDNBOOST_NO_STD_UNORDERED
#if defined(NDNBOOST_NO_CXX11_HDR_UNORDERED_MAP) || defined (NDNBOOST_NO_CXX11_HDR_UNORDERED_SET)
# ifndef NDNBOOST_NO_CXX11_STD_UNORDERED
#  define NDNBOOST_NO_CXX11_STD_UNORDERED
# endif
#endif

//  Use NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST instead of NDNBOOST_NO_INITIALIZER_LISTS
#if defined(NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST) && !defined(NDNBOOST_NO_INITIALIZER_LISTS)
#  define NDNBOOST_NO_INITIALIZER_LISTS
#endif

//  Use NDNBOOST_NO_CXX11_HDR_ARRAY instead of NDNBOOST_NO_0X_HDR_ARRAY
#if defined(NDNBOOST_NO_CXX11_HDR_ARRAY) && !defined(NDNBOOST_NO_0X_HDR_ARRAY)
#  define NDNBOOST_NO_0X_HDR_ARRAY
#endif
//  Use NDNBOOST_NO_CXX11_HDR_CHRONO instead of NDNBOOST_NO_0X_HDR_CHRONO
#if defined(NDNBOOST_NO_CXX11_HDR_CHRONO) && !defined(NDNBOOST_NO_0X_HDR_CHRONO)
#  define NDNBOOST_NO_0X_HDR_CHRONO
#endif
//  Use NDNBOOST_NO_CXX11_HDR_CODECVT instead of NDNBOOST_NO_0X_HDR_CODECVT
#if defined(NDNBOOST_NO_CXX11_HDR_CODECVT) && !defined(NDNBOOST_NO_0X_HDR_CODECVT)
#  define NDNBOOST_NO_0X_HDR_CODECVT
#endif
//  Use NDNBOOST_NO_CXX11_HDR_CONDITION_VARIABLE instead of NDNBOOST_NO_0X_HDR_CONDITION_VARIABLE
#if defined(NDNBOOST_NO_CXX11_HDR_CONDITION_VARIABLE) && !defined(NDNBOOST_NO_0X_HDR_CONDITION_VARIABLE)
#  define NDNBOOST_NO_0X_HDR_CONDITION_VARIABLE
#endif
//  Use NDNBOOST_NO_CXX11_HDR_FORWARD_LIST instead of NDNBOOST_NO_0X_HDR_FORWARD_LIST
#if defined(NDNBOOST_NO_CXX11_HDR_FORWARD_LIST) && !defined(NDNBOOST_NO_0X_HDR_FORWARD_LIST)
#  define NDNBOOST_NO_0X_HDR_FORWARD_LIST
#endif
//  Use NDNBOOST_NO_CXX11_HDR_FUTURE instead of NDNBOOST_NO_0X_HDR_FUTURE
#if defined(NDNBOOST_NO_CXX11_HDR_FUTURE) && !defined(NDNBOOST_NO_0X_HDR_FUTURE)
#  define NDNBOOST_NO_0X_HDR_FUTURE
#endif

//  Use NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST 
//  instead of NDNBOOST_NO_0X_HDR_INITIALIZER_LIST or NDNBOOST_NO_INITIALIZER_LISTS
#ifdef NDNBOOST_NO_CXX11_HDR_INITIALIZER_LIST
# ifndef NDNBOOST_NO_0X_HDR_INITIALIZER_LIST
#  define NDNBOOST_NO_0X_HDR_INITIALIZER_LIST
# endif
# ifndef NDNBOOST_NO_INITIALIZER_LISTS
#  define NDNBOOST_NO_INITIALIZER_LISTS
# endif
#endif

//  Use NDNBOOST_NO_CXX11_HDR_MUTEX instead of NDNBOOST_NO_0X_HDR_MUTEX
#if defined(NDNBOOST_NO_CXX11_HDR_MUTEX) && !defined(NDNBOOST_NO_0X_HDR_MUTEX)
#  define NDNBOOST_NO_0X_HDR_MUTEX
#endif
//  Use NDNBOOST_NO_CXX11_HDR_RANDOM instead of NDNBOOST_NO_0X_HDR_RANDOM
#if defined(NDNBOOST_NO_CXX11_HDR_RANDOM) && !defined(NDNBOOST_NO_0X_HDR_RANDOM)
#  define NDNBOOST_NO_0X_HDR_RANDOM
#endif
//  Use NDNBOOST_NO_CXX11_HDR_RATIO instead of NDNBOOST_NO_0X_HDR_RATIO
#if defined(NDNBOOST_NO_CXX11_HDR_RATIO) && !defined(NDNBOOST_NO_0X_HDR_RATIO)
#  define NDNBOOST_NO_0X_HDR_RATIO
#endif
//  Use NDNBOOST_NO_CXX11_HDR_REGEX instead of NDNBOOST_NO_0X_HDR_REGEX
#if defined(NDNBOOST_NO_CXX11_HDR_REGEX) && !defined(NDNBOOST_NO_0X_HDR_REGEX)
#  define NDNBOOST_NO_0X_HDR_REGEX
#endif
//  Use NDNBOOST_NO_CXX11_HDR_SYSTEM_ERROR instead of NDNBOOST_NO_0X_HDR_SYSTEM_ERROR
#if defined(NDNBOOST_NO_CXX11_HDR_SYSTEM_ERROR) && !defined(NDNBOOST_NO_0X_HDR_SYSTEM_ERROR)
#  define NDNBOOST_NO_0X_HDR_SYSTEM_ERROR
#endif
//  Use NDNBOOST_NO_CXX11_HDR_THREAD instead of NDNBOOST_NO_0X_HDR_THREAD
#if defined(NDNBOOST_NO_CXX11_HDR_THREAD) && !defined(NDNBOOST_NO_0X_HDR_THREAD)
#  define NDNBOOST_NO_0X_HDR_THREAD
#endif
//  Use NDNBOOST_NO_CXX11_HDR_TUPLE instead of NDNBOOST_NO_0X_HDR_TUPLE
#if defined(NDNBOOST_NO_CXX11_HDR_TUPLE) && !defined(NDNBOOST_NO_0X_HDR_TUPLE)
#  define NDNBOOST_NO_0X_HDR_TUPLE
#endif
//  Use NDNBOOST_NO_CXX11_HDR_TYPE_TRAITS instead of NDNBOOST_NO_0X_HDR_TYPE_TRAITS
#if defined(NDNBOOST_NO_CXX11_HDR_TYPE_TRAITS) && !defined(NDNBOOST_NO_0X_HDR_TYPE_TRAITS)
#  define NDNBOOST_NO_0X_HDR_TYPE_TRAITS
#endif
//  Use NDNBOOST_NO_CXX11_HDR_TYPEINDEX instead of NDNBOOST_NO_0X_HDR_TYPEINDEX
#if defined(NDNBOOST_NO_CXX11_HDR_TYPEINDEX) && !defined(NDNBOOST_NO_0X_HDR_TYPEINDEX)
#  define NDNBOOST_NO_0X_HDR_TYPEINDEX
#endif
//  Use NDNBOOST_NO_CXX11_HDR_UNORDERED_MAP instead of NDNBOOST_NO_0X_HDR_UNORDERED_MAP
#if defined(NDNBOOST_NO_CXX11_HDR_UNORDERED_MAP) && !defined(NDNBOOST_NO_0X_HDR_UNORDERED_MAP)
#  define NDNBOOST_NO_0X_HDR_UNORDERED_MAP
#endif
//  Use NDNBOOST_NO_CXX11_HDR_UNORDERED_SET instead of NDNBOOST_NO_0X_HDR_UNORDERED_SET
#if defined(NDNBOOST_NO_CXX11_HDR_UNORDERED_SET) && !defined(NDNBOOST_NO_0X_HDR_UNORDERED_SET)
#  define NDNBOOST_NO_0X_HDR_UNORDERED_SET
#endif

//  ------------------ End of deprecated macros for 1.50 ---------------------------

//  -------------------- Deprecated macros for 1.51 ---------------------------
//  These will go away in a future release

//  Use     NDNBOOST_NO_CXX11_AUTO_DECLARATIONS instead of   NDNBOOST_NO_AUTO_DECLARATIONS
#if defined(NDNBOOST_NO_CXX11_AUTO_DECLARATIONS) && !defined(NDNBOOST_NO_AUTO_DECLARATIONS)
#  define NDNBOOST_NO_AUTO_DECLARATIONS
#endif
//  Use     NDNBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS instead of   NDNBOOST_NO_AUTO_MULTIDECLARATIONS
#if defined(NDNBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS) && !defined(NDNBOOST_NO_AUTO_MULTIDECLARATIONS)
#  define NDNBOOST_NO_AUTO_MULTIDECLARATIONS
#endif
//  Use     NDNBOOST_NO_CXX11_CHAR16_T instead of   NDNBOOST_NO_CHAR16_T
#if defined(NDNBOOST_NO_CXX11_CHAR16_T) && !defined(NDNBOOST_NO_CHAR16_T)
#  define NDNBOOST_NO_CHAR16_T
#endif
//  Use     NDNBOOST_NO_CXX11_CHAR32_T instead of   NDNBOOST_NO_CHAR32_T
#if defined(NDNBOOST_NO_CXX11_CHAR32_T) && !defined(NDNBOOST_NO_CHAR32_T)
#  define NDNBOOST_NO_CHAR32_T
#endif
//  Use     NDNBOOST_NO_CXX11_TEMPLATE_ALIASES instead of   NDNBOOST_NO_TEMPLATE_ALIASES
#if defined(NDNBOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(NDNBOOST_NO_TEMPLATE_ALIASES)
#  define NDNBOOST_NO_TEMPLATE_ALIASES
#endif
//  Use     NDNBOOST_NO_CXX11_CONSTEXPR instead of   NDNBOOST_NO_CONSTEXPR
#if defined(NDNBOOST_NO_CXX11_CONSTEXPR) && !defined(NDNBOOST_NO_CONSTEXPR)
#  define NDNBOOST_NO_CONSTEXPR
#endif
//  Use     NDNBOOST_NO_CXX11_DECLTYPE_N3276 instead of   NDNBOOST_NO_DECLTYPE_N3276
#if defined(NDNBOOST_NO_CXX11_DECLTYPE_N3276) && !defined(NDNBOOST_NO_DECLTYPE_N3276)
#  define NDNBOOST_NO_DECLTYPE_N3276
#endif
//  Use     NDNBOOST_NO_CXX11_DECLTYPE instead of   NDNBOOST_NO_DECLTYPE
#if defined(NDNBOOST_NO_CXX11_DECLTYPE) && !defined(NDNBOOST_NO_DECLTYPE)
#  define NDNBOOST_NO_DECLTYPE
#endif
//  Use     NDNBOOST_NO_CXX11_DEFAULTED_FUNCTIONS instead of   NDNBOOST_NO_DEFAULTED_FUNCTIONS
#if defined(NDNBOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && !defined(NDNBOOST_NO_DEFAULTED_FUNCTIONS)
#  define NDNBOOST_NO_DEFAULTED_FUNCTIONS
#endif
//  Use     NDNBOOST_NO_CXX11_DELETED_FUNCTIONS instead of   NDNBOOST_NO_DELETED_FUNCTIONS
#if defined(NDNBOOST_NO_CXX11_DELETED_FUNCTIONS) && !defined(NDNBOOST_NO_DELETED_FUNCTIONS)
#  define NDNBOOST_NO_DELETED_FUNCTIONS
#endif
//  Use     NDNBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS instead of   NDNBOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#if defined(NDNBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS) && !defined(NDNBOOST_NO_EXPLICIT_CONVERSION_OPERATORS)
#  define NDNBOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#endif
//  Use     NDNBOOST_NO_CXX11_EXTERN_TEMPLATE instead of   NDNBOOST_NO_EXTERN_TEMPLATE
#if defined(NDNBOOST_NO_CXX11_EXTERN_TEMPLATE) && !defined(NDNBOOST_NO_EXTERN_TEMPLATE)
#  define NDNBOOST_NO_EXTERN_TEMPLATE
#endif
//  Use     NDNBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS instead of   NDNBOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#if defined(NDNBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(NDNBOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
#  define NDNBOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif
//  Use     NDNBOOST_NO_CXX11_LAMBDAS instead of   NDNBOOST_NO_LAMBDAS
#if defined(NDNBOOST_NO_CXX11_LAMBDAS) && !defined(NDNBOOST_NO_LAMBDAS)
#  define NDNBOOST_NO_LAMBDAS
#endif
//  Use     NDNBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS instead of   NDNBOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#if defined(NDNBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS) && !defined(NDNBOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
#  define NDNBOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif
//  Use     NDNBOOST_NO_CXX11_NOEXCEPT instead of   NDNBOOST_NO_NOEXCEPT
#if defined(NDNBOOST_NO_CXX11_NOEXCEPT) && !defined(NDNBOOST_NO_NOEXCEPT)
#  define NDNBOOST_NO_NOEXCEPT
#endif
//  Use     NDNBOOST_NO_CXX11_NULLPTR instead of   NDNBOOST_NO_NULLPTR
#if defined(NDNBOOST_NO_CXX11_NULLPTR) && !defined(NDNBOOST_NO_NULLPTR)
#  define NDNBOOST_NO_NULLPTR
#endif
//  Use     NDNBOOST_NO_CXX11_RAW_LITERALS instead of   NDNBOOST_NO_RAW_LITERALS
#if defined(NDNBOOST_NO_CXX11_RAW_LITERALS) && !defined(NDNBOOST_NO_RAW_LITERALS)
#  define NDNBOOST_NO_RAW_LITERALS
#endif
//  Use     NDNBOOST_NO_CXX11_RVALUE_REFERENCES instead of   NDNBOOST_NO_RVALUE_REFERENCES
#if defined(NDNBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(NDNBOOST_NO_RVALUE_REFERENCES)
#  define NDNBOOST_NO_RVALUE_REFERENCES
#endif
//  Use     NDNBOOST_NO_CXX11_SCOPED_ENUMS instead of   NDNBOOST_NO_SCOPED_ENUMS
#if defined(NDNBOOST_NO_CXX11_SCOPED_ENUMS) && !defined(NDNBOOST_NO_SCOPED_ENUMS)
#  define NDNBOOST_NO_SCOPED_ENUMS
#endif
//  Use     NDNBOOST_NO_CXX11_STATIC_ASSERT instead of   NDNBOOST_NO_STATIC_ASSERT
#if defined(NDNBOOST_NO_CXX11_STATIC_ASSERT) && !defined(NDNBOOST_NO_STATIC_ASSERT)
#  define NDNBOOST_NO_STATIC_ASSERT
#endif
//  Use     NDNBOOST_NO_CXX11_STD_UNORDERED instead of   NDNBOOST_NO_STD_UNORDERED
#if defined(NDNBOOST_NO_CXX11_STD_UNORDERED) && !defined(NDNBOOST_NO_STD_UNORDERED)
#  define NDNBOOST_NO_STD_UNORDERED
#endif
//  Use     NDNBOOST_NO_CXX11_UNICODE_LITERALS instead of   NDNBOOST_NO_UNICODE_LITERALS
#if defined(NDNBOOST_NO_CXX11_UNICODE_LITERALS) && !defined(NDNBOOST_NO_UNICODE_LITERALS)
#  define NDNBOOST_NO_UNICODE_LITERALS
#endif
//  Use     NDNBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX instead of   NDNBOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#if defined(NDNBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX) && !defined(NDNBOOST_NO_UNIFIED_INITIALIZATION_SYNTAX)
#  define NDNBOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#endif
//  Use     NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES instead of   NDNBOOST_NO_VARIADIC_TEMPLATES
#if defined(NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(NDNBOOST_NO_VARIADIC_TEMPLATES)
#  define NDNBOOST_NO_VARIADIC_TEMPLATES
#endif
//  Use     NDNBOOST_NO_CXX11_VARIADIC_MACROS instead of   NDNBOOST_NO_VARIADIC_MACROS
#if defined(NDNBOOST_NO_CXX11_VARIADIC_MACROS) && !defined(NDNBOOST_NO_VARIADIC_MACROS)
#  define NDNBOOST_NO_VARIADIC_MACROS
#endif
//  Use     NDNBOOST_NO_CXX11_NUMERIC_LIMITS instead of   NDNBOOST_NO_NUMERIC_LIMITS_LOWEST
#if defined(NDNBOOST_NO_CXX11_NUMERIC_LIMITS) && !defined(NDNBOOST_NO_NUMERIC_LIMITS_LOWEST)
#  define NDNBOOST_NO_NUMERIC_LIMITS_LOWEST
#endif
//  ------------------ End of deprecated macros for 1.51 ---------------------------



//
// Helper macros NDNBOOST_NOEXCEPT, NDNBOOST_NOEXCEPT_IF, NDNBOOST_NOEXCEPT_EXPR
// These aid the transition to C++11 while still supporting C++03 compilers
//
#ifdef NDNBOOST_NO_CXX11_NOEXCEPT
#  define NDNBOOST_NOEXCEPT
#  define NDNBOOST_NOEXCEPT_OR_NOTHROW throw()
#  define NDNBOOST_NOEXCEPT_IF(Predicate)
#  define NDNBOOST_NOEXCEPT_EXPR(Expression) false
#else
#  define NDNBOOST_NOEXCEPT noexcept
#  define NDNBOOST_NOEXCEPT_OR_NOTHROW noexcept
#  define NDNBOOST_NOEXCEPT_IF(Predicate) noexcept((Predicate))
#  define NDNBOOST_NOEXCEPT_EXPR(Expression) noexcept((Expression))
#endif
//
// Helper macro NDNBOOST_FALLTHROUGH 
// Fallback definition of NDNBOOST_FALLTHROUGH macro used to mark intended 
// fall-through between case labels in a switch statement. We use a definition 
// that requires a semicolon after it to avoid at least one type of misuse even 
// on unsupported compilers. 
// 
#ifndef NDNBOOST_FALLTHROUGH 
#  define NDNBOOST_FALLTHROUGH ((void)0) 
#endif 

//
// constexpr workarounds
// 
#if defined(NDNBOOST_NO_CXX11_CONSTEXPR)
#define NDNBOOST_CONSTEXPR
#define NDNBOOST_CONSTEXPR_OR_CONST const
#else
#define NDNBOOST_CONSTEXPR constexpr
#define NDNBOOST_CONSTEXPR_OR_CONST constexpr
#endif

#define NDNBOOST_STATIC_CONSTEXPR  static NDNBOOST_CONSTEXPR_OR_CONST

//
// Set NDNBOOST_HAS_STATIC_ASSERT when NDNBOOST_NO_CXX11_STATIC_ASSERT is not defined
//
#if !defined(NDNBOOST_NO_CXX11_STATIC_ASSERT) && !defined(NDNBOOST_HAS_STATIC_ASSERT)
#  define NDNBOOST_HAS_STATIC_ASSERT
#endif

//
// Set NDNBOOST_HAS_RVALUE_REFS when NDNBOOST_NO_CXX11_RVALUE_REFERENCES is not defined
//
#if !defined(NDNBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(NDNBOOST_HAS_RVALUE_REFS)
#define NDNBOOST_HAS_RVALUE_REFS
#endif

//
// Set NDNBOOST_HAS_VARIADIC_TMPL when NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES is not defined
//
#if !defined(NDNBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(NDNBOOST_HAS_VARIADIC_TMPL)
#define NDNBOOST_HAS_VARIADIC_TMPL
#endif


#endif
