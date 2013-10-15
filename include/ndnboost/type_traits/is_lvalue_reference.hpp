
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, 
//      Howard Hinnant and John Maddock 2000. 
//  (C) Copyright Mat Marcus, Jesse Jones and Adobe Systems Inc 2001

//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

//    Fixed is_pointer, is_lvalue_reference, is_const, is_volatile, is_same, 
//    is_member_pointer based on the Simulated Partial Specialization work 
//    of Mat Marcus and Jesse Jones. See  http://opensource.adobe.com or 
//    http://groups.yahoo.com/group/boost/message/5441 
//    Some workarounds in here use ideas suggested from "Generic<Programming>: 
//    Mappings between Types and Values" 
//    by Andrei Alexandrescu (see http://www.cuj.com/experts/1810/alexandr.html).


#ifndef NDNBOOST_TT_IS_LVALUE_REFERENCE_HPP_INCLUDED
#define NDNBOOST_TT_IS_LVALUE_REFERENCE_HPP_INCLUDED

#include <ndnboost/type_traits/config.hpp>

#ifdef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#   include <ndnboost/type_traits/detail/yes_no_type.hpp>
#   include <ndnboost/type_traits/detail/wrap.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

#if defined( __CODEGEARC__ )
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_lvalue_reference,T,__is_reference(T))
#elif !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_lvalue_reference,T,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T&,true)

#if  defined(NDNBOOST_ILLEGAL_CV_REFERENCES)
// these are illegal specialisations; cv-qualifies applied to
// references have no effect according to [8.3.2p1],
// C++ Builder requires them though as it treats cv-qualified
// references as distinct types...
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T& const,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T& volatile,true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T& const volatile,true)
#endif

#if defined(__GNUC__) && (__GNUC__ < 3)
// these allow us to work around illegally cv-qualified reference
// types.
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T const ,::ndnboost::is_lvalue_reference<T>::value)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T volatile ,::ndnboost::is_lvalue_reference<T>::value)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_lvalue_reference,T const volatile ,::ndnboost::is_lvalue_reference<T>::value)
// However, the above specializations confuse gcc 2.96 unless we also
// supply these specializations for array types
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,unsigned long N,is_lvalue_reference,T[N],false)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,unsigned long N,is_lvalue_reference,const T[N],false)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,unsigned long N,is_lvalue_reference,volatile T[N],false)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,unsigned long N,is_lvalue_reference,const volatile T[N],false)
#endif

#else

#ifdef NDNBOOST_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4181 4097)
#endif

namespace detail {

using ::ndnboost::type_traits::yes_type;
using ::ndnboost::type_traits::no_type;
using ::ndnboost::type_traits::wrap;

template <class T> T&(* is_lvalue_reference_helper1(wrap<T>) )(wrap<T>);
char is_lvalue_reference_helper1(...);

template <class T> no_type is_lvalue_reference_helper2(T&(*)(wrap<T>));
yes_type is_lvalue_reference_helper2(...);

template <typename T>
struct is_lvalue_reference_impl
{
    NDNBOOST_STATIC_CONSTANT(
        bool, value = sizeof(
            ::ndnboost::detail::is_lvalue_reference_helper2(
                ::ndnboost::detail::is_lvalue_reference_helper1(::ndnboost::type_traits::wrap<T>()))) == 1
        );
};

NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_lvalue_reference,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_lvalue_reference,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_lvalue_reference,void volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_lvalue_reference,void const volatile,false)
#endif

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_lvalue_reference,T,::ndnboost::detail::is_lvalue_reference_impl<T>::value)

#ifdef NDNBOOST_MSVC
#   pragma warning(pop)
#endif

#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_REFERENCE_HPP_INCLUDED

