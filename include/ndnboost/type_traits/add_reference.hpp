
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef NDNBOOST_TT_ADD_REFERENCE_HPP_INCLUDED
#define NDNBOOST_TT_ADD_REFERENCE_HPP_INCLUDED

#include <ndnboost/type_traits/is_reference.hpp>
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/config.hpp>

// should be the last #include
#include <ndnboost/type_traits/detail/type_trait_def.hpp>

namespace ndnboost {

namespace detail {

#if defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && defined(NDNBOOST_MSVC6_MEMBER_TEMPLATES)

template <bool x>
struct reference_adder
{
    template <typename T> struct result_
    {
        typedef T& type;
    };
};

template <>
struct reference_adder<true>
{
    template <typename T> struct result_
    {
        typedef T type;
    };
};

template <typename T>
struct add_reference_impl
{
    typedef typename reference_adder<
          ::ndnboost::is_reference<T>::value
        >::template result_<T> result;

    typedef typename result::type type;
};

#else
//
// We can't filter out rvalue_references at the same level as
// references or we get ambiguities from msvc:
//

template <typename T>
struct add_reference_rvalue_layer
{
    typedef T& type;
};

#ifndef NDNBOOST_NO_CXX11_RVALUE_REFERENCES
template <typename T>
struct add_reference_rvalue_layer<T&&>
{
    typedef T&& type;
};
#endif

template <typename T>
struct add_reference_impl
{
    typedef typename add_reference_rvalue_layer<T>::type type;
};

#ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
NDNBOOST_TT_AUX_TYPE_TRAIT_IMPL_PARTIAL_SPEC1_1(typename T,add_reference,T&,T&)
#endif

#endif

// these full specialisations are always required:
NDNBOOST_TT_AUX_TYPE_TRAIT_IMPL_SPEC1(add_reference,void,void)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_TYPE_TRAIT_IMPL_SPEC1(add_reference,void const,void const)
NDNBOOST_TT_AUX_TYPE_TRAIT_IMPL_SPEC1(add_reference,void volatile,void volatile)
NDNBOOST_TT_AUX_TYPE_TRAIT_IMPL_SPEC1(add_reference,void const volatile,void const volatile)
#endif

} // namespace detail

NDNBOOST_TT_AUX_TYPE_TRAIT_DEF1(add_reference,T,typename ndnboost::detail::add_reference_impl<T>::type)

// agurt, 07/mar/03: workaround Borland's ill-formed sensitivity to an additional
// level of indirection, here
#if NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600)
NDNBOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,add_reference,T&,T&)
#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/type_trait_undef.hpp>

#endif // NDNBOOST_TT_ADD_REFERENCE_HPP_INCLUDED
