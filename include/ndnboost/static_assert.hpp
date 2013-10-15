//  (C) Copyright John Maddock 2000.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/static_assert for documentation.

/*
 Revision history:
   02 August 2000
      Initial version.
*/

#ifndef NDNBOOST_STATIC_ASSERT_HPP
#define NDNBOOST_STATIC_ASSERT_HPP

#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>

#if defined(__GNUC__) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
//
// This is horrible, but it seems to be the only we can shut up the
// "anonymous variadic macros were introduced in C99 [-Wvariadic-macros]"
// warning that get spewed out otherwise in non-C++11 mode.
//
#pragma GCC system_header
#endif

#ifndef NDNBOOST_NO_CXX11_STATIC_ASSERT
#  ifndef NDNBOOST_NO_CXX11_VARIADIC_MACROS
#     define NDNBOOST_STATIC_ASSERT_MSG( ... ) static_assert(__VA_ARGS__)
#  else
#     define NDNBOOST_STATIC_ASSERT_MSG( B, Msg ) NDNBOOST_STATIC_ASSERT( B )
#  endif
#else
#     define NDNBOOST_STATIC_ASSERT_MSG( B, Msg ) NDNBOOST_STATIC_ASSERT( B )
#endif

#ifdef __BORLANDC__
//
// workaround for buggy integral-constant expression support:
#define NDNBOOST_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS
#endif

#if defined(__GNUC__) && (__GNUC__ == 3) && ((__GNUC_MINOR__ == 3) || (__GNUC_MINOR__ == 4))
// gcc 3.3 and 3.4 don't produce good error messages with the default version:
#  define NDNBOOST_SA_GCC_WORKAROUND
#endif

//
// If the compiler issues warnings about old C style casts,
// then enable this:
//
#if defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4)))
#  ifndef NDNBOOST_NO_CXX11_VARIADIC_MACROS
#     define NDNBOOST_STATIC_ASSERT_BOOL_CAST( ... ) ((__VA_ARGS__) == 0 ? false : true)
#  else
#     define NDNBOOST_STATIC_ASSERT_BOOL_CAST( x ) ((x) == 0 ? false : true)
#  endif
#else
#  ifndef NDNBOOST_NO_CXX11_VARIADIC_MACROS
#     define NDNBOOST_STATIC_ASSERT_BOOL_CAST( ... ) (bool)(__VA_ARGS__)
#  else
#     define NDNBOOST_STATIC_ASSERT_BOOL_CAST(x) (bool)(x)
#  endif
#endif
//
// If the compiler warns about unused typedefs then enable this:
//
#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
#  define NDNBOOST_STATIC_ASSERT_UNUSED_ATTRIBUTE __attribute__((unused))
#else
#  define NDNBOOST_STATIC_ASSERT_UNUSED_ATTRIBUTE
#endif

#ifndef NDNBOOST_NO_CXX11_STATIC_ASSERT
#  ifndef NDNBOOST_NO_CXX11_VARIADIC_MACROS
#     define NDNBOOST_STATIC_ASSERT( ... ) static_assert(__VA_ARGS__, #__VA_ARGS__)
#  else
#     define NDNBOOST_STATIC_ASSERT( B ) static_assert(B, #B)
#  endif
#else

namespace ndnboost{

// HP aCC cannot deal with missing names for template value parameters
template <bool x> struct STATIC_ASSERTION_FAILURE;

template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };

// HP aCC cannot deal with missing names for template value parameters
template<int x> struct static_assert_test{};

}

//
// Implicit instantiation requires that all member declarations be
// instantiated, but that the definitions are *not* instantiated.
//
// It's not particularly clear how this applies to enum's or typedefs;
// both are described as declarations [7.1.3] and [7.2] in the standard,
// however some compilers use "delayed evaluation" of one or more of
// these when implicitly instantiating templates.  We use typedef declarations
// by default, but try defining NDNBOOST_USE_ENUM_STATIC_ASSERT if the enum
// version gets better results from your compiler...
//
// Implementation:
// Both of these versions rely on sizeof(incomplete_type) generating an error
// message containing the name of the incomplete type.  We use
// "STATIC_ASSERTION_FAILURE" as the type name here to generate
// an eye catching error message.  The result of the sizeof expression is either
// used as an enum initialiser, or as a template argument depending which version
// is in use...
// Note that the argument to the assert is explicitly cast to bool using old-
// style casts: too many compilers currently have problems with static_cast
// when used inside integral constant expressions.
//
#if !defined(NDNBOOST_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS)

#if defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1300)
// __LINE__ macro broken when -ZI is used see Q199057
// fortunately MSVC ignores duplicate typedef's.
#define NDNBOOST_STATIC_ASSERT( B ) \
   typedef ::ndnboost::static_assert_test<\
      sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< (bool)( B ) >)\
      > boost_static_assert_typedef_
#elif defined(NDNBOOST_MSVC) && defined(NDNBOOST_NO_CXX11_VARIADIC_MACROS)
#define NDNBOOST_STATIC_ASSERT( B ) \
   typedef ::ndnboost::static_assert_test<\
      sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST ( B ) >)>\
         NDNBOOST_JOIN(boost_static_assert_typedef_, __COUNTER__)
#elif defined(NDNBOOST_MSVC)
#define NDNBOOST_STATIC_ASSERT(...) \
   typedef ::ndnboost::static_assert_test<\
      sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST (__VA_ARGS__) >)>\
         NDNBOOST_JOIN(boost_static_assert_typedef_, __COUNTER__)
#elif (defined(NDNBOOST_INTEL_CXX_VERSION) || defined(NDNBOOST_SA_GCC_WORKAROUND))  && defined(NDNBOOST_NO_CXX11_VARIADIC_MACROS)
// agurt 15/sep/02: a special care is needed to force Intel C++ issue an error 
// instead of warning in case of failure
# define NDNBOOST_STATIC_ASSERT( B ) \
    typedef char NDNBOOST_JOIN(boost_static_assert_typedef_, __LINE__) \
        [ ::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST( B ) >::value ]
#elif (defined(NDNBOOST_INTEL_CXX_VERSION) || defined(NDNBOOST_SA_GCC_WORKAROUND))  && !defined(NDNBOOST_NO_CXX11_VARIADIC_MACROS)
// agurt 15/sep/02: a special care is needed to force Intel C++ issue an error 
// instead of warning in case of failure
# define NDNBOOST_STATIC_ASSERT(...) \
    typedef char NDNBOOST_JOIN(boost_static_assert_typedef_, __LINE__) \
        [ ::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST( __VA_ARGS__ ) >::value ]
#elif defined(__sgi)
// special version for SGI MIPSpro compiler
#define NDNBOOST_STATIC_ASSERT( B ) \
   NDNBOOST_STATIC_CONSTANT(bool, \
     NDNBOOST_JOIN(boost_static_assert_test_, __LINE__) = ( B )); \
   typedef ::ndnboost::static_assert_test<\
     sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< \
       NDNBOOST_JOIN(boost_static_assert_test_, __LINE__) >)>\
         NDNBOOST_JOIN(boost_static_assert_typedef_, __LINE__)
#elif NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003)
// special version for CodeWarrior <= 8.x
#define NDNBOOST_STATIC_ASSERT( B ) \
   NDNBOOST_STATIC_CONSTANT(int, \
     NDNBOOST_JOIN(boost_static_assert_test_, __LINE__) = \
       sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST( B ) >) )
#else
// generic version
#  ifndef NDNBOOST_NO_CXX11_VARIADIC_MACROS
#     define NDNBOOST_STATIC_ASSERT( ... ) \
         typedef ::ndnboost::static_assert_test<\
            sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST( __VA_ARGS__ ) >)>\
               NDNBOOST_JOIN(boost_static_assert_typedef_, __LINE__) NDNBOOST_STATIC_ASSERT_UNUSED_ATTRIBUTE
#  else
#     define NDNBOOST_STATIC_ASSERT( B ) \
         typedef ::ndnboost::static_assert_test<\
            sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< NDNBOOST_STATIC_ASSERT_BOOL_CAST( B ) >)>\
               NDNBOOST_JOIN(boost_static_assert_typedef_, __LINE__) NDNBOOST_STATIC_ASSERT_UNUSED_ATTRIBUTE
#  endif
#endif

#else
// alternative enum based implementation:
#  ifndef NDNBOOST_NO_CXX11_VARIADIC_MACROS
#    define NDNBOOST_STATIC_ASSERT( ... ) \
         enum { NDNBOOST_JOIN(boost_static_assert_enum_, __LINE__) \
            = sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< (bool)( __VA_ARGS__ ) >) }
#  else
#    define NDNBOOST_STATIC_ASSERT(B) \
         enum { NDNBOOST_JOIN(boost_static_assert_enum_, __LINE__) \
            = sizeof(::ndnboost::STATIC_ASSERTION_FAILURE< (bool)( B ) >) }
#  endif
#endif
#endif // defined(NDNBOOST_NO_CXX11_STATIC_ASSERT)

#endif // NDNBOOST_STATIC_ASSERT_HPP


