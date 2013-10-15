
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_IS_STATELESS_HPP_INCLUDED
#define NDNBOOST_TT_IS_STATELESS_HPP_INCLUDED

#include <ndnboost/type_traits/has_trivial_constructor.hpp>
#include <ndnboost/type_traits/has_trivial_copy.hpp>
#include <ndnboost/type_traits/has_trivial_destructor.hpp>
#include <ndnboost/type_traits/is_class.hpp>
#include <ndnboost/type_traits/is_empty.hpp>
#include <ndnboost/type_traits/detail/ice_and.hpp>
#include <ndnboost/config.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail {

template <typename T>
struct is_stateless_impl
{
  NDNBOOST_STATIC_CONSTANT(bool, value = 
    (::ndnboost::type_traits::ice_and<
       ::ndnboost::has_trivial_constructor<T>::value,
       ::ndnboost::has_trivial_copy<T>::value,
       ::ndnboost::has_trivial_destructor<T>::value,
       ::ndnboost::is_class<T>::value,
       ::ndnboost::is_empty<T>::value
     >::value));
};

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_stateless,T,::ndnboost::detail::is_stateless_impl<T>::value)

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_STATELESS_HPP_INCLUDED
