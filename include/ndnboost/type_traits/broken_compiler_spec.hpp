
//  Copyright 2001-2003 Aleksey Gurtovoy.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_BROKEN_COMPILER_SPEC_HPP_INCLUDED
#define NDNBOOST_TT_BROKEN_COMPILER_SPEC_HPP_INCLUDED

#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/config.hpp>

// these are needed regardless of NDNBOOST_TT_NO_BROKEN_COMPILER_SPEC 
#if defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
namespace ndnboost { namespace detail {
template< typename T > struct remove_const_impl     { typedef T type; };
template< typename T > struct remove_volatile_impl  { typedef T type; };
template< typename T > struct remove_pointer_impl   { typedef T type; };
template< typename T > struct remove_reference_impl { typedef T type; };
typedef int invoke_NDNBOOST_TT_BROKEN_COMPILER_SPEC_outside_all_namespaces;
}}
#endif

// agurt, 27/jun/03: disable the workaround if user defined 
// NDNBOOST_TT_NO_BROKEN_COMPILER_SPEC
#if    !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
    || defined(NDNBOOST_TT_NO_BROKEN_COMPILER_SPEC)

#   define NDNBOOST_TT_BROKEN_COMPILER_SPEC(T) /**/

#else

// same as NDNBOOST_TT_AUX_TYPE_TRAIT_IMPL_SPEC1 macro, except that it
// never gets #undef-ined
#   define NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(trait,spec,result) \
template<> struct trait##_impl<spec> \
{ \
    typedef result type; \
}; \
/**/

#   define NDNBOOST_TT_AUX_REMOVE_CONST_VOLATILE_RANK1_SPEC(T)                         \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_const,T const,T)                    \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_const,T const volatile,T volatile)  \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_volatile,T volatile,T)              \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_volatile,T const volatile,T const)  \
    /**/

#   define NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_1_SPEC(T)                               \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_pointer,T*,T)                       \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_pointer,T*const,T)                  \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_pointer,T*volatile,T)               \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_pointer,T*const volatile,T)         \
    NDNBOOST_TT_AUX_BROKEN_TYPE_TRAIT_SPEC1(remove_reference,T&,T)                     \
    /**/

#   define NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_2_SPEC(T)                               \
    NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_1_SPEC(T)                                      \
    NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_1_SPEC(T const)                                \
    NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_1_SPEC(T volatile)                             \
    NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_1_SPEC(T const volatile)                       \
    /**/

#   define NDNBOOST_TT_AUX_REMOVE_ALL_RANK_1_SPEC(T)                                   \
    NDNBOOST_TT_AUX_REMOVE_PTR_REF_RANK_2_SPEC(T)                                      \
    NDNBOOST_TT_AUX_REMOVE_CONST_VOLATILE_RANK1_SPEC(T)                                \
    /**/

#   define NDNBOOST_TT_AUX_REMOVE_ALL_RANK_2_SPEC(T)                                   \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_1_SPEC(T*)                                         \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_1_SPEC(T const*)                                   \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_1_SPEC(T volatile*)                                \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_1_SPEC(T const volatile*)                          \
    /**/

#   define NDNBOOST_TT_BROKEN_COMPILER_SPEC(T)                                         \
    namespace ndnboost { namespace detail {                                            \
    typedef invoke_NDNBOOST_TT_BROKEN_COMPILER_SPEC_outside_all_namespaces             \
      please_invoke_NDNBOOST_TT_BROKEN_COMPILER_SPEC_outside_all_namespaces;           \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_1_SPEC(T)                                          \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_2_SPEC(T)                                          \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_2_SPEC(T*)                                         \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_2_SPEC(T const*)                                   \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_2_SPEC(T volatile*)                                \
    NDNBOOST_TT_AUX_REMOVE_ALL_RANK_2_SPEC(T const volatile*)                          \
    }}                                                                              \
    /**/

#   include <ndnboost/type_traits/detail/type_trait_undef.hpp>

#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

NDNBOOST_TT_BROKEN_COMPILER_SPEC(bool)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(char)
#ifndef NDNBOOST_NO_INTRINSIC_WCHAR_T
NDNBOOST_TT_BROKEN_COMPILER_SPEC(wchar_t)
#endif
NDNBOOST_TT_BROKEN_COMPILER_SPEC(signed char)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(unsigned char)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(signed short)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(unsigned short)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(signed int)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(unsigned int)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(signed long)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(unsigned long)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(float)
NDNBOOST_TT_BROKEN_COMPILER_SPEC(double)
//NDNBOOST_TT_BROKEN_COMPILER_SPEC(long double)

// for backward compatibility
#define NDNBOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(T) \
    NDNBOOST_TT_BROKEN_COMPILER_SPEC(T) \
/**/

#endif // NDNBOOST_TT_BROKEN_COMPILER_SPEC_HPP_INCLUDED
