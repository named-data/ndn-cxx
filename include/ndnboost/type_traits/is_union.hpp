
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_IS_UNION_HPP_INCLUDED
#define NDNBOOST_TT_IS_UNION_HPP_INCLUDED

#include <ndnboost/type_traits/remove_cv.hpp>
#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/type_traits/intrinsics.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail {
#ifndef __GNUC__
template <typename T> struct is_union_impl
{
   typedef typename remove_cv<T>::type cvt;
#ifdef NDNBOOST_IS_UNION
   NDNBOOST_STATIC_CONSTANT(bool, value = NDNBOOST_IS_UNION(cvt));
#else
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
#endif
};
#else
//
// using remove_cv here generates a whole load of needless
// warnings with gcc, since it doesn't do any good with gcc
// in any case (at least at present), just remove it:
//
template <typename T> struct is_union_impl
{
#ifdef NDNBOOST_IS_UNION
   NDNBOOST_STATIC_CONSTANT(bool, value = NDNBOOST_IS_UNION(T));
#else
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
#endif
};
#endif
} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_union,T,::ndnboost::detail::is_union_impl<T>::value)

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_UNION_HPP_INCLUDED
