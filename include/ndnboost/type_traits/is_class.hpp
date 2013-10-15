//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000-2003.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_IS_CLASS_HPP_INCLUDED
#define NDNBOOST_TT_IS_CLASS_HPP_INCLUDED

#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/type_traits/intrinsics.hpp>
#ifndef NDNBOOST_IS_CLASS
#   include <ndnboost/type_traits/is_union.hpp>
#   include <ndnboost/type_traits/detail/ice_and.hpp>
#   include <ndnboost/type_traits/detail/ice_not.hpp>

#ifdef NDNBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION
#   include <ndnboost/type_traits/detail/yes_no_type.hpp>
#else
#   include <ndnboost/type_traits/is_scalar.hpp>
#   include <ndnboost/type_traits/is_array.hpp>
#   include <ndnboost/type_traits/is_reference.hpp>
#   include <ndnboost/type_traits/is_void.hpp>
#   include <ndnboost/type_traits/is_function.hpp>
#endif

#endif // NDNBOOST_IS_CLASS

#ifdef __EDG_VERSION__
#   include <ndnboost/type_traits/remove_cv.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

namespace detail {

#ifndef NDNBOOST_IS_CLASS
#ifdef NDNBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION

// This is actually the conforming implementation which works with
// abstract classes.  However, enough compilers have trouble with
// it that most will use the one in
// ndnboost/type_traits/object_traits.hpp. This implementation
// actually works with VC7.0, but other interactions seem to fail
// when we use it.

// is_class<> metafunction due to Paul Mensonides
// (leavings@attbi.com). For more details:
// http://groups.google.com/groups?hl=en&selm=000001c1cc83%24e154d5e0%247772e50c%40c161550a&rnum=1
#if defined(__GNUC__)  && !defined(__EDG_VERSION__)

template <class U> ::ndnboost::type_traits::yes_type is_class_tester(void(U::*)(void));
template <class U> ::ndnboost::type_traits::no_type is_class_tester(...);

template <typename T>
struct is_class_impl
{

    NDNBOOST_STATIC_CONSTANT(bool, value =
        (::ndnboost::type_traits::ice_and<
            sizeof(is_class_tester<T>(0)) == sizeof(::ndnboost::type_traits::yes_type),
            ::ndnboost::type_traits::ice_not< ::ndnboost::is_union<T>::value >::value
        >::value)
        );
};

#else

template <typename T>
struct is_class_impl
{
    template <class U> static ::ndnboost::type_traits::yes_type is_class_tester(void(U::*)(void));
    template <class U> static ::ndnboost::type_traits::no_type is_class_tester(...);

    NDNBOOST_STATIC_CONSTANT(bool, value =
        (::ndnboost::type_traits::ice_and<
            sizeof(is_class_tester<T>(0)) == sizeof(::ndnboost::type_traits::yes_type),
            ::ndnboost::type_traits::ice_not< ::ndnboost::is_union<T>::value >::value
        >::value)
        );
};

#endif

#else

template <typename T>
struct is_class_impl
{
#   ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    NDNBOOST_STATIC_CONSTANT(bool, value =
    (::ndnboost::type_traits::ice_and<
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_union<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_scalar<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_array<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_reference<T>::value>::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_void<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_function<T>::value >::value
        >::value));
#   else
    NDNBOOST_STATIC_CONSTANT(bool, value =
    (::ndnboost::type_traits::ice_and<
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_union<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_scalar<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_array<T>::value >::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_reference<T>::value>::value,
        ::ndnboost::type_traits::ice_not< ::ndnboost::is_void<T>::value >::value
        >::value));
#   endif
};

# endif // NDNBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION
# else // NDNBOOST_IS_CLASS
template <typename T>
struct is_class_impl
{
    NDNBOOST_STATIC_CONSTANT(bool, value = NDNBOOST_IS_CLASS(T));
};
# endif // NDNBOOST_IS_CLASS

} // namespace detail

# ifdef __EDG_VERSION__
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(
   is_class,T, ndnboost::detail::is_class_impl<typename ndnboost::remove_cv<T>::type>::value)
# else 
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_class,T,::ndnboost::detail::is_class_impl<T>::value)
# endif
    
} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_CLASS_HPP_INCLUDED
