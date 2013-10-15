
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_IS_ARITHMETIC_HPP_INCLUDED
#define NDNBOOST_TT_IS_ARITHMETIC_HPP_INCLUDED

#if !defined( __CODEGEARC__ )
#include <ndnboost/type_traits/is_integral.hpp>
#include <ndnboost/type_traits/is_float.hpp>
#include <ndnboost/type_traits/detail/ice_or.hpp>
#include <ndnboost/config.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

#if !defined(__CODEGEARC__)
namespace detail {

template< typename T >
struct is_arithmetic_impl
{ 
    NDNBOOST_STATIC_CONSTANT(bool, value = 
        (::ndnboost::type_traits::ice_or< 
            ::ndnboost::is_integral<T>::value,
            ::ndnboost::is_float<T>::value
        >::value)); 
};

} // namespace detail
#endif

//* is a type T an arithmetic type described in the standard (3.9.1p8)
#if defined(__CODEGEARC__)
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_arithmetic,T,__is_arithmetic(T))
#else
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_arithmetic,T,::ndnboost::detail::is_arithmetic_impl<T>::value)
#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_ARITHMETIC_HPP_INCLUDED
