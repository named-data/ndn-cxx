
// Copyright 2000 John Maddock (john@johnmaddock.co.uk)
// Copyright 2000 Jeremy Siek (jsiek@lsc.nd.edu)
// Copyright 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED
#define NDNBOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED

#include <ndnboost/type_traits/intrinsics.hpp>
#ifndef NDNBOOST_IS_CONVERTIBLE
#include <ndnboost/type_traits/detail/yes_no_type.hpp>
#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/type_traits/is_array.hpp>
#include <ndnboost/type_traits/ice.hpp>
#include <ndnboost/type_traits/is_arithmetic.hpp>
#include <ndnboost/type_traits/is_void.hpp>
#ifndef NDNBOOST_NO_IS_ABSTRACT
#include <ndnboost/type_traits/is_abstract.hpp>
#endif
#include <ndnboost/type_traits/add_lvalue_reference.hpp>
#include <ndnboost/type_traits/add_rvalue_reference.hpp>
#include <ndnboost/type_traits/is_function.hpp>

#if defined(__MWERKS__)
#include <ndnboost/type_traits/remove_reference.hpp>
#endif

#endif // NDNBOOST_IS_CONVERTIBLE

// should be always the last #include directive
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

#ifndef NDNBOOST_IS_CONVERTIBLE

// is one type convertible to another?
//
// there are multiple versions of the is_convertible
// template, almost every compiler seems to require its
// own version.
//
// Thanks to Andrei Alexandrescu for the original version of the
// conversion detection technique!
//

namespace detail {

// MS specific version:

#if defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC <= 1300)

// This workaround is necessary to handle when From is void
// which is normally taken care of by the partial specialization
// of the is_convertible typename.
using ::ndnboost::type_traits::yes_type;
using ::ndnboost::type_traits::no_type;

template< typename From >
struct does_conversion_exist
{
    template< typename To > struct result_
    {
        static no_type NDNBOOST_TT_DECL _m_check(...);
        static yes_type NDNBOOST_TT_DECL _m_check(To);
        static typename add_lvalue_reference<From>::type  _m_from;
        enum { value = sizeof( _m_check(_m_from) ) == sizeof(yes_type) };
    };
};

template<>
struct does_conversion_exist<void>
{
    template< typename To > struct result_
    {
        enum { value = ::ndnboost::is_void<To>::value };
    };
};

template <typename From, typename To>
struct is_convertible_basic_impl
    : public does_conversion_exist<From>::template result_<To>
{
};

#elif defined(__BORLANDC__) && (__BORLANDC__ < 0x560)
//
// special version for Borland compilers
// this version breaks when used for some
// UDT conversions:
//
template <typename From, typename To>
struct is_convertible_impl
{
#pragma option push -w-8074
    // This workaround for Borland breaks the EDG C++ frontend,
    // so we only use it for Borland.
    template <typename T> struct checker
    {
        static ::ndnboost::type_traits::no_type NDNBOOST_TT_DECL _m_check(...);
        static ::ndnboost::type_traits::yes_type NDNBOOST_TT_DECL _m_check(T);
    };

    static typename add_lvalue_reference<From>::type  _m_from;
    static bool const value = sizeof( checker<To>::_m_check(_m_from) )
        == sizeof(::ndnboost::type_traits::yes_type);
#pragma option pop
};

#elif defined(__GNUC__) || defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
// special version for gcc compiler + recent Borland versions
// note that this does not pass UDT's through (...)

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    template <typename T> any_conversion(T&);
};

template <typename T> struct checker
{
    static ndnboost::type_traits::no_type _m_check(any_conversion ...);
    static ndnboost::type_traits::yes_type _m_check(T, int);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;
#if !defined(NDNBOOST_NO_CXX11_RVALUE_REFERENCES) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 6)))
    static bool const value =
        sizeof( ndnboost::detail::checker<To>::_m_check(static_cast<rvalue_type>(_m_from), 0) )
        == sizeof(::ndnboost::type_traits::yes_type);
#else
    static bool const value =
        sizeof( ndnboost::detail::checker<To>::_m_check(_m_from, 0) )
        == sizeof(::ndnboost::type_traits::yes_type);
#endif
};

#elif (defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 245) && !defined(__ICL)) \
      || defined(__IBMCPP__) || defined(__HP_aCC)
//
// This is *almost* an ideal world implementation as it doesn't rely
// on undefined behaviour by passing UDT's through (...).
// Unfortunately it doesn't quite pass all the tests for most compilers (sigh...)
// Enable this for your compiler if is_convertible_test.cpp will compile it...
//
// Note we do not enable this for VC7.1, because even though it passes all the
// type_traits tests it is known to cause problems when instantiation occurs
// deep within the instantiation tree :-(
//
struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    // we need this constructor to catch references to functions
    // (which can not be cv-qualified):
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    static ::ndnboost::type_traits::no_type NDNBOOST_TT_DECL _m_check(any_conversion ...);
    static ::ndnboost::type_traits::yes_type NDNBOOST_TT_DECL _m_check(To, int);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type; 
    static lvalue_type _m_from;

#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from), 0) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#else
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from, 0) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#endif
};

#elif defined(__DMC__)

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    // we need this constructor to catch references to functions
    // (which can not be cv-qualified):
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    // Using '...' doesn't always work on Digital Mars. This version seems to.
    template <class T>
    static ::ndnboost::type_traits::no_type NDNBOOST_TT_DECL _m_check(any_conversion,  float, T);
    static ::ndnboost::type_traits::yes_type NDNBOOST_TT_DECL _m_check(To, int, int);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;

    // Static constants sometime cause the conversion of _m_from to To to be
    // called. This doesn't happen with an enum.
#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
    enum { value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from), 0, 0) ) == sizeof(::ndnboost::type_traits::yes_type)
        };
#else
    enum { value =
        sizeof( _m_check(_m_from, 0, 0) ) == sizeof(::ndnboost::type_traits::yes_type)
        };
#endif
};

#elif defined(__MWERKS__)
// 
// CW works with the technique implemented above for EDG, except when From
// is a function type (or a reference to such a type), in which case
// any_conversion won't be accepted as a valid conversion. We detect this
// exceptional situation and channel it through an alternative algorithm.
//

template <typename From, typename To,bool FromIsFunctionRef>
struct is_convertible_basic_impl_aux;

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl_aux<From,To,false /*FromIsFunctionRef*/>
{
    static ::ndnboost::type_traits::no_type NDNBOOST_TT_DECL _m_check(any_conversion ...);
    static ::ndnboost::type_traits::yes_type NDNBOOST_TT_DECL _m_check(To, int);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type; 
    static lvalue_type _m_from;

#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from), 0) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#else
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from, 0) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#endif
};

template <typename From, typename To>
struct is_convertible_basic_impl_aux<From,To,true /*FromIsFunctionRef*/>
{
    static ::ndnboost::type_traits::no_type NDNBOOST_TT_DECL _m_check(...);
    static ::ndnboost::type_traits::yes_type NDNBOOST_TT_DECL _m_check(To);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;
#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from)) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#else
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#endif
};

template <typename From, typename To>
struct is_convertible_basic_impl:
  is_convertible_basic_impl_aux<
    From,To,
    ::ndnboost::is_function<typename ::ndnboost::remove_reference<From>::type>::value
  >
{};

#else
//
// This version seems to work pretty well for a wide spectrum of compilers,
// however it does rely on undefined behaviour by passing UDT's through (...).
//
template <typename From, typename To>
struct is_convertible_basic_impl
{
    static ::ndnboost::type_traits::no_type NDNBOOST_TT_DECL _m_check(...);
    static ::ndnboost::type_traits::yes_type NDNBOOST_TT_DECL _m_check(To);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type; 
    static lvalue_type _m_from;
#ifdef NDNBOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4244)
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC_FULL_VER, >= 140050000)
#pragma warning(disable:6334)
#endif
#endif
#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from)) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#else
    NDNBOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from) ) == sizeof(::ndnboost::type_traits::yes_type)
        );
#endif
#ifdef NDNBOOST_MSVC
#pragma warning(pop)
#endif
};

#endif // is_convertible_impl

#if defined(__DMC__)
// As before, a static constant sometimes causes errors on Digital Mars.
template <typename From, typename To>
struct is_convertible_impl
{
    enum { value =
        (::ndnboost::type_traits::ice_and<
            ::ndnboost::type_traits::ice_or<
               ::ndnboost::detail::is_convertible_basic_impl<From,To>::value,
               ::ndnboost::is_void<To>::value
            >::value,
            ::ndnboost::type_traits::ice_not<
               ::ndnboost::is_array<To>::value
            >::value,
            ::ndnboost::type_traits::ice_not<
               ::ndnboost::is_function<To>::value
            >::value
        >::value) };
};
#elif !defined(__BORLANDC__) || __BORLANDC__ > 0x551
template <typename From, typename To>
struct is_convertible_impl
{
    NDNBOOST_STATIC_CONSTANT(bool, value =
        (::ndnboost::type_traits::ice_and<
            ::ndnboost::type_traits::ice_or<
               ::ndnboost::detail::is_convertible_basic_impl<From,To>::value,
               ::ndnboost::is_void<To>::value
            >::value,
            ::ndnboost::type_traits::ice_not<
               ::ndnboost::is_array<To>::value
            >::value,
            ::ndnboost::type_traits::ice_not<
               ::ndnboost::is_function<To>::value
            >::value
        >::value)
        );
};
#endif

template <bool trivial1, bool trivial2, bool abstract_target>
struct is_convertible_impl_select
{
   template <class From, class To>
   struct rebind
   {
      typedef is_convertible_impl<From, To> type;
   };
};

template <>
struct is_convertible_impl_select<true, true, false>
{
   template <class From, class To>
   struct rebind
   {
      typedef true_type type;
   };
};

template <>
struct is_convertible_impl_select<false, false, true>
{
   template <class From, class To>
   struct rebind
   {
      typedef false_type type;
   };
};

template <>
struct is_convertible_impl_select<true, false, true>
{
   template <class From, class To>
   struct rebind
   {
      typedef false_type type;
   };
};

template <typename From, typename To>
struct is_convertible_impl_dispatch_base
{
#if !NDNBOOST_WORKAROUND(__HP_aCC, < 60700)
   typedef is_convertible_impl_select< 
      ::ndnboost::is_arithmetic<From>::value, 
      ::ndnboost::is_arithmetic<To>::value,
#ifndef NDNBOOST_NO_IS_ABSTRACT
      ::ndnboost::is_abstract<To>::value
#else
      false
#endif
   > selector;
#else
   typedef is_convertible_impl_select<false, false, false> selector;
#endif
   typedef typename selector::template rebind<From, To> isc_binder;
   typedef typename isc_binder::type type;
};

template <typename From, typename To>
struct is_convertible_impl_dispatch 
   : public is_convertible_impl_dispatch_base<From, To>::type
{};

//
// Now add the full and partial specialisations
// for void types, these are common to all the
// implementation above:
//
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
#   define TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1,spec2,value) \
    NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2,value) \
    NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 const,value) \
    NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 volatile,value) \
    NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 const volatile,value) \
    /**/

#   define TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2(trait,spec1,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 const,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 volatile,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 const volatile,spec2,value) \
    /**/

    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2(is_convertible,void,void,true)

#   undef TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2
#   undef TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1

#else
    NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(is_convertible,void,void,true)
#endif // NDNBOOST_NO_CV_VOID_SPECIALIZATIONS

#ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void,To,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void const,To,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void volatile,To,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void const volatile,To,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void const volatile,false)
#endif
#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF2(is_convertible,From,To,(::ndnboost::detail::is_convertible_impl_dispatch<From,To>::value))

#else

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF2(is_convertible,From,To,NDNBOOST_IS_CONVERTIBLE(From,To))

#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED
