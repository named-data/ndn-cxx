
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


// Some fixes for is_array are based on a newsgroup posting by Jonathan Lundquist.


#ifndef NDNBOOST_TT_IS_ARRAY_HPP_INCLUDED
#define NDNBOOST_TT_IS_ARRAY_HPP_INCLUDED

#include <ndnboost/type_traits/config.hpp>

#ifdef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#   include <ndnboost/type_traits/detail/yes_no_type.hpp>
#   include <ndnboost/type_traits/detail/wrap.hpp>
#endif

#include <cstddef>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

#if defined( __CODEGEARC__ )
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_array,T,__is_array(T))
#elif !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_array,T,false)
#if !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,is_array,T[N],true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,is_array,T const[N],true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,is_array,T volatile[N],true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,is_array,T const volatile[N],true)
#if !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(__IBMCPP__) &&  !NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840))
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_array,T[],true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_array,T const[],true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_array,T volatile[],true)
NDNBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_array,T const volatile[],true)
#endif
#endif

#else // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

namespace detail {

using ::ndnboost::type_traits::yes_type;
using ::ndnboost::type_traits::no_type;
using ::ndnboost::type_traits::wrap;

template< typename T > T(* is_array_tester1(wrap<T>) )(wrap<T>);
char NDNBOOST_TT_DECL is_array_tester1(...);

template< typename T> no_type is_array_tester2(T(*)(wrap<T>));
yes_type NDNBOOST_TT_DECL is_array_tester2(...);

template< typename T >
struct is_array_impl
{ 
    NDNBOOST_STATIC_CONSTANT(bool, value = 
        sizeof(::ndnboost::detail::is_array_tester2(
            ::ndnboost::detail::is_array_tester1(
                ::ndnboost::type_traits::wrap<T>()
                )
        )) == 1
    );
};

#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_array,void,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_array,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_array,void volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_array,void const volatile,false)
#endif

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_array,T,::ndnboost::detail::is_array_impl<T>::value)

#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_ARRAY_HPP_INCLUDED
