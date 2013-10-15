//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_INTRINSICS_HPP_INCLUDED
#define NDNBOOST_TT_INTRINSICS_HPP_INCLUDED

#ifndef NDNBOOST_TT_CONFIG_HPP_INCLUDED
#include <ndnboost/type_traits/config.hpp>
#endif

//
// Helper macros for builtin compiler support.
// If your compiler has builtin support for any of the following
// traits concepts, then redefine the appropriate macros to pick
// up on the compiler support:
//
// (these should largely ignore cv-qualifiers)
// NDNBOOST_IS_UNION(T) should evaluate to true if T is a union type
// NDNBOOST_IS_POD(T) should evaluate to true if T is a POD type
// NDNBOOST_IS_EMPTY(T) should evaluate to true if T is an empty class type (and not a union)
// NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) should evaluate to true if "T x;" has no effect
// NDNBOOST_HAS_TRIVIAL_COPY(T) should evaluate to true if T(t) <==> memcpy
// NDNBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) should evaluate to true if T(ndnboost::move(t)) <==> memcpy
// NDNBOOST_HAS_TRIVIAL_ASSIGN(T) should evaluate to true if t = u <==> memcpy
// NDNBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T) should evaluate to true if t = ndnboost::move(u) <==> memcpy
// NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) should evaluate to true if ~T() has no effect
// NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) should evaluate to true if "T x;" can not throw
// NDNBOOST_HAS_NOTHROW_COPY(T) should evaluate to true if T(t) can not throw
// NDNBOOST_HAS_NOTHROW_ASSIGN(T) should evaluate to true if t = u can not throw
// NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) should evaluate to true T has a virtual destructor
//
// The following can also be defined: when detected our implementation is greatly simplified.
//
// NDNBOOST_IS_ABSTRACT(T) true if T is an abstract type
// NDNBOOST_IS_BASE_OF(T,U) true if T is a base class of U
// NDNBOOST_IS_CLASS(T) true if T is a class type (and not a union)
// NDNBOOST_IS_CONVERTIBLE(T,U) true if T is convertible to U
// NDNBOOST_IS_ENUM(T) true is T is an enum
// NDNBOOST_IS_POLYMORPHIC(T) true if T is a polymorphic type
// NDNBOOST_ALIGNMENT_OF(T) should evaluate to the alignment requirements of type T.

#ifdef NDNBOOST_HAS_SGI_TYPE_TRAITS
    // Hook into SGI's __type_traits class, this will pick up user supplied
    // specializations as well as SGI - compiler supplied specializations.
#   include <ndnboost/type_traits/is_same.hpp>
#   ifdef __NetBSD__
      // There are two different versions of type_traits.h on NetBSD on Spark
      // use an implicit include via algorithm instead, to make sure we get
      // the same version as the std lib:
#     include <algorithm>
#   else
#    include <type_traits.h>
#   endif
#   define NDNBOOST_IS_POD(T) ::ndnboost::is_same< typename ::__type_traits<T>::is_POD_type, ::__true_type>::value
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) ::ndnboost::is_same< typename ::__type_traits<T>::has_trivial_default_constructor, ::__true_type>::value
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) ::ndnboost::is_same< typename ::__type_traits<T>::has_trivial_copy_constructor, ::__true_type>::value
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) ::ndnboost::is_same< typename ::__type_traits<T>::has_trivial_assignment_operator, ::__true_type>::value
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) ::ndnboost::is_same< typename ::__type_traits<T>::has_trivial_destructor, ::__true_type>::value

#   ifdef __sgi
#      define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#   endif
#endif

#if defined(__MSL_CPP__) && (__MSL_CPP__ >= 0x8000)
    // Metrowerks compiler is acquiring intrinsic type traits support
    // post version 8.  We hook into the published interface to pick up
    // user defined specializations as well as compiler intrinsics as 
    // and when they become available:
#   include <msl_utility>
#   define NDNBOOST_IS_UNION(T) NDNBOOST_STD_EXTENSION_NAMESPACE::is_union<T>::value
#   define NDNBOOST_IS_POD(T) NDNBOOST_STD_EXTENSION_NAMESPACE::is_POD<T>::value
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) NDNBOOST_STD_EXTENSION_NAMESPACE::has_trivial_default_ctor<T>::value
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) NDNBOOST_STD_EXTENSION_NAMESPACE::has_trivial_copy_ctor<T>::value
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) NDNBOOST_STD_EXTENSION_NAMESPACE::has_trivial_assignment<T>::value
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) NDNBOOST_STD_EXTENSION_NAMESPACE::has_trivial_dtor<T>::value
#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if (defined(NDNBOOST_MSVC) && defined(NDNBOOST_MSVC_FULL_VER) && (NDNBOOST_MSVC_FULL_VER >=140050215))\
         || (defined(NDNBOOST_INTEL) && defined(_MSC_VER) && (_MSC_VER >= 1500))
#   include <ndnboost/type_traits/is_same.hpp>
#   include <ndnboost/type_traits/is_function.hpp>

#   define NDNBOOST_IS_UNION(T) __is_union(T)
#   define NDNBOOST_IS_POD(T) (__is_pod(T) && __has_trivial_constructor(T))
#   define NDNBOOST_IS_EMPTY(T) __is_empty(T)
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T)|| ( ::ndnboost::is_pod<T>::value && !::ndnboost::is_volatile<T>::value))
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) || ( ::ndnboost::is_pod<T>::value && ! ::ndnboost::is_const<T>::value && !::ndnboost::is_volatile<T>::value))
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T) || ::ndnboost::is_pod<T>::value)
#   define NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_constructor(T) || ::ndnboost::has_trivial_constructor<T>::value)
#   define NDNBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T) || ::ndnboost::has_trivial_copy<T>::value)
#   define NDNBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) || ::ndnboost::has_trivial_assign<T>::value)
#   define NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define NDNBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define NDNBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   define NDNBOOST_IS_CLASS(T) __is_class(T)
#   define NDNBOOST_IS_CONVERTIBLE(T,U) ((__is_convertible_to(T,U) || (is_same<T,U>::value && !is_function<U>::value)) && !__is_abstract(U))
#   define NDNBOOST_IS_ENUM(T) __is_enum(T)
//  This one doesn't quite always do the right thing:
//  #   define NDNBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
//  This one fails if the default alignment has been changed with /Zp:
//  #   define NDNBOOST_ALIGNMENT_OF(T) __alignof(T)

#   if defined(_MSC_VER) && (_MSC_VER >= 1700)
#       define NDNBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) ((__has_trivial_move_constructor(T) || ::ndnboost::is_pod<T>::value) && !::ndnboost::is_volatile<T>::value)
#       define NDNBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T) ((__has_trivial_move_assign(T) || ::ndnboost::is_pod<T>::value) && ! ::ndnboost::is_const<T>::value && !::ndnboost::is_volatile<T>::value)
#   endif

#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(__DMC__) && (__DMC__ >= 0x848)
// For Digital Mars C++, www.digitalmars.com
#   define NDNBOOST_IS_UNION(T) (__typeinfo(T) & 0x400)
#   define NDNBOOST_IS_POD(T) (__typeinfo(T) & 0x800)
#   define NDNBOOST_IS_EMPTY(T) (__typeinfo(T) & 0x1000)
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) (__typeinfo(T) & 0x10)
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) (__typeinfo(T) & 0x20)
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) (__typeinfo(T) & 0x40)
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__typeinfo(T) & 0x8)
#   define NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__typeinfo(T) & 0x80)
#   define NDNBOOST_HAS_NOTHROW_COPY(T) (__typeinfo(T) & 0x100)
#   define NDNBOOST_HAS_NOTHROW_ASSIGN(T) (__typeinfo(T) & 0x200)
#   define NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) (__typeinfo(T) & 0x4)
#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(NDNBOOST_CLANG) && defined(__has_feature)
#   include <cstddef>
#   include <ndnboost/type_traits/is_same.hpp>
#   include <ndnboost/type_traits/is_reference.hpp>
#   include <ndnboost/type_traits/is_volatile.hpp>

#   if __has_feature(is_union)
#     define NDNBOOST_IS_UNION(T) __is_union(T)
#   endif
#   if (!defined(__GLIBCXX__) || (__GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519)) && __has_feature(is_pod)
#     define NDNBOOST_IS_POD(T) __is_pod(T)
#   endif
#   if (!defined(__GLIBCXX__) || (__GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519)) && __has_feature(is_empty)
#     define NDNBOOST_IS_EMPTY(T) __is_empty(T)
#   endif
#   if __has_feature(has_trivial_constructor)
#     define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   endif
#   if __has_feature(has_trivial_copy)
#     define NDNBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T) && !is_reference<T>::value && !is_volatile<T>::value)
#   endif
#   if __has_feature(has_trivial_assign)
#     define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) && !is_volatile<T>::value)
#   endif
#   if __has_feature(has_trivial_destructor)
#     define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   endif
#   if __has_feature(has_nothrow_constructor)
#     define NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) __has_nothrow_constructor(T)
#   endif
#   if __has_feature(has_nothrow_copy)
#     define NDNBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   endif
#   if __has_feature(has_nothrow_assign)
#     define NDNBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) && !is_volatile<T>::value)
#   endif
#   if __has_feature(has_virtual_destructor)
#     define NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)
#   endif
#   if __has_feature(is_abstract)
#     define NDNBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   endif
#   if __has_feature(is_base_of)
#     define NDNBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   endif
#   if __has_feature(is_class)
#     define NDNBOOST_IS_CLASS(T) __is_class(T)
#   endif
#   if __has_feature(is_convertible_to)
#     include <ndnboost/type_traits/is_abstract.hpp>
#     define NDNBOOST_IS_CONVERTIBLE(T,U) (__is_convertible_to(T,U) && !::ndnboost::is_abstract<U>::value)
#   endif
#   if __has_feature(is_enum)
#     define NDNBOOST_IS_ENUM(T) __is_enum(T)
#   endif
#   if __has_feature(is_polymorphic)
#     define NDNBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   endif
#   if __has_feature(has_trivial_move_constructor)
#     define NDNBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) __has_trivial_move_constructor(T)
#   endif
#   if __has_feature(has_trivial_move_assign)
#     define NDNBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T) __has_trivial_move_assign(T)
#   endif
#   define NDNBOOST_ALIGNMENT_OF(T) __alignof(T)

#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3) && !defined(__GCCXML__))) && !defined(NDNBOOST_CLANG)
#   include <ndnboost/type_traits/is_same.hpp>
#   include <ndnboost/type_traits/is_reference.hpp>
#   include <ndnboost/type_traits/is_volatile.hpp>

#ifdef NDNBOOST_INTEL
#  define NDNBOOST_INTEL_TT_OPTS || is_pod<T>::value
#else
#  define NDNBOOST_INTEL_TT_OPTS
#endif

#   define NDNBOOST_IS_UNION(T) __is_union(T)
#   define NDNBOOST_IS_POD(T) __is_pod(T)
#   define NDNBOOST_IS_EMPTY(T) __is_empty(T)
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) ((__has_trivial_constructor(T) NDNBOOST_INTEL_TT_OPTS) && ! ::ndnboost::is_volatile<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) ((__has_trivial_copy(T) NDNBOOST_INTEL_TT_OPTS) && !is_reference<T>::value && ! ::ndnboost::is_volatile<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) ((__has_trivial_assign(T) NDNBOOST_INTEL_TT_OPTS) && ! ::ndnboost::is_volatile<T>::value && ! ::ndnboost::is_const<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T) NDNBOOST_INTEL_TT_OPTS)
#   define NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_constructor(T) NDNBOOST_INTEL_TT_OPTS)
#   define NDNBOOST_HAS_NOTHROW_COPY(T) ((__has_nothrow_copy(T) NDNBOOST_INTEL_TT_OPTS) && !is_volatile<T>::value && !is_reference<T>::value)
#   define NDNBOOST_HAS_NOTHROW_ASSIGN(T) ((__has_nothrow_assign(T) NDNBOOST_INTEL_TT_OPTS) && !is_volatile<T>::value && !is_const<T>::value)
#   define NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define NDNBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define NDNBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   define NDNBOOST_IS_CLASS(T) __is_class(T)
#   define NDNBOOST_IS_ENUM(T) __is_enum(T)
#   define NDNBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   if (!defined(unix) && !defined(__unix__)) || defined(__LP64__)
      // GCC sometimes lies about alignment requirements
      // of type double on 32-bit unix platforms, use the
      // old implementation instead in that case:
#     define NDNBOOST_ALIGNMENT_OF(T) __alignof__(T)
#   endif

#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#if defined(__ghs__) && (__GHS_VERSION_NUMBER >= 600)
#   include <ndnboost/type_traits/is_same.hpp>
#   include <ndnboost/type_traits/is_reference.hpp>
#   include <ndnboost/type_traits/is_volatile.hpp>

#   define NDNBOOST_IS_UNION(T) __is_union(T)
#   define NDNBOOST_IS_POD(T) __is_pod(T)
#   define NDNBOOST_IS_EMPTY(T) __is_empty(T)
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T) && !is_reference<T>::value && !is_volatile<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) && !is_volatile<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   define NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) __has_nothrow_constructor(T)
#   define NDNBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   define NDNBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) && !is_volatile<T>::value)
#   define NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define NDNBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define NDNBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_same<T,U>::value)
#   define NDNBOOST_IS_CLASS(T) __is_class(T)
#   define NDNBOOST_IS_ENUM(T) __is_enum(T)
#   define NDNBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   define NDNBOOST_ALIGNMENT_OF(T) __alignof__(T)

#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

# if defined(__CODEGEARC__)
#   include <ndnboost/type_traits/is_same.hpp>
#   include <ndnboost/type_traits/is_reference.hpp>
#   include <ndnboost/type_traits/is_volatile.hpp>
#   include <ndnboost/type_traits/is_void.hpp>

#   define NDNBOOST_IS_UNION(T) __is_union(T)
#   define NDNBOOST_IS_POD(T) __is_pod(T)
#   define NDNBOOST_IS_EMPTY(T) __is_empty(T)
#   define NDNBOOST_HAS_TRIVIAL_CONSTRUCTOR(T) (__has_trivial_default_constructor(T))
#   define NDNBOOST_HAS_TRIVIAL_COPY(T) (__has_trivial_copy_constructor(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T) && !is_volatile<T>::value)
#   define NDNBOOST_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T))
#   define NDNBOOST_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_default_constructor(T))
#   define NDNBOOST_HAS_NOTHROW_COPY(T) (__has_nothrow_copy_constructor(T) && !is_volatile<T>::value && !is_reference<T>::value)
#   define NDNBOOST_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T) && !is_volatile<T>::value)
#   define NDNBOOST_HAS_VIRTUAL_DESTRUCTOR(T) __has_virtual_destructor(T)

#   define NDNBOOST_IS_ABSTRACT(T) __is_abstract(T)
#   define NDNBOOST_IS_BASE_OF(T,U) (__is_base_of(T,U) && !is_void<T>::value && !is_void<U>::value)
#   define NDNBOOST_IS_CLASS(T) __is_class(T)
#   define NDNBOOST_IS_CONVERTIBLE(T,U) (__is_convertible(T,U) || is_void<U>::value)
#   define NDNBOOST_IS_ENUM(T) __is_enum(T)
#   define NDNBOOST_IS_POLYMORPHIC(T) __is_polymorphic(T)
#   define NDNBOOST_ALIGNMENT_OF(T) alignof(T)

#   define NDNBOOST_HAS_TYPE_TRAITS_INTRINSICS
#endif

#endif // NDNBOOST_TT_INTRINSICS_HPP_INCLUDED







