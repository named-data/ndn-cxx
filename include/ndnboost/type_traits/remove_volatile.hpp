
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_REMOVE_VOLATILE_HPP_INCLUDED
#define NDNBOOST_TT_REMOVE_VOLATILE_HPP_INCLUDED

#include <ndnboost/type_traits/is_const.hpp>
#include <ndnboost/type_traits/broken_compiler_spec.hpp>
#include <ndnboost/type_traits/detail/cv_traits_impl.hpp>
#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>

#include <cstddef>

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC,<=1300)
#include <ndnboost/type_traits/msvc/remove_volatile.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/type_trait_def.hpp>

namespace ndnboost {

#ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

namespace detail {

template <typename T, bool is_const>
struct remove_volatile_helper
{
    typedef T type;
};

template <typename T>
struct remove_volatile_helper<T,true>
{
    typedef T const type;
};

template <typename T>
struct remove_volatile_impl
{
    typedef typename remove_volatile_helper<
          typename cv_traits_imp<T*>::unqualified_type
        , ::ndnboost::is_const<T>::value
        >::type type;
};

//
// We can't filter out rvalue_references at the same level as
// references or we get ambiguities from msvc:
//
#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
template <typename T>
struct remove_volatile_impl<T&&>
{
    typedef T&& type;
};
#endif
} // namespace detail

// * convert a type T to a non-volatile type - remove_volatile<T>

NDNBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_volatile,T,typename ndnboost::detail::remove_volatile_impl<T>::type)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_volatile,T&,T&)
#if !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_volatile,T volatile[N],T type[N])
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_volatile,T const volatile[N],T const type[N])
#endif

#elif !NDNBOOST_WORKAROUND(NDNBOOST_MSVC,<=1300)

NDNBOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_volatile,T,typename ndnboost::detail::remove_volatile_impl<T>::type)

#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace ndnboost

#include <ndnboost/type_traits/detail/type_trait_undef.hpp>

#endif // NDNBOOST_TT_REMOVE_VOLATILE_HPP_INCLUDED
