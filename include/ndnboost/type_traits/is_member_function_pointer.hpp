
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
#define NDNBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED

#include <ndnboost/type_traits/config.hpp>
#include <ndnboost/detail/workaround.hpp>

#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
   && !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(NDNBOOST_TT_TEST_MS_FUNC_SIGS)
   //
   // Note: we use the "workaround" version for MSVC because it works for 
   // __stdcall etc function types, where as the partial specialisation
   // version does not do so.
   //
#   include <ndnboost/type_traits/detail/is_mem_fun_pointer_impl.hpp>
#   include <ndnboost/type_traits/remove_cv.hpp>
#else
#   include <ndnboost/type_traits/is_reference.hpp>
#   include <ndnboost/type_traits/is_array.hpp>
#   include <ndnboost/type_traits/detail/yes_no_type.hpp>
#   include <ndnboost/type_traits/detail/false_result.hpp>
#   include <ndnboost/type_traits/detail/ice_or.hpp>
#   include <ndnboost/type_traits/detail/is_mem_fun_pointer_tester.hpp>
#endif

// should be the last #include
#include <ndnboost/type_traits/detail/bool_trait_def.hpp>

namespace ndnboost {

#if defined( __CODEGEARC__ )
NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_member_function_pointer,T,__is_member_function_pointer( T ))
#elif !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(NDNBOOST_TT_TEST_MS_FUNC_SIGS)

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(
      is_member_function_pointer
    , T
    , ::ndnboost::type_traits::is_mem_fun_pointer_impl<typename remove_cv<T>::type>::value
    )

#else

namespace detail {

#ifndef __BORLANDC__

template <bool>
struct is_mem_fun_pointer_select
    : public ::ndnboost::type_traits::false_result
{
};

template <>
struct is_mem_fun_pointer_select<false>
{
    template <typename T> struct result_
    {
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC_FULL_VER, >= 140050000)
#pragma warning(push)
#pragma warning(disable:6334)
#endif
        static T* make_t;
        typedef result_<T> self_type;

        NDNBOOST_STATIC_CONSTANT(
            bool, value = (
                1 == sizeof(::ndnboost::type_traits::is_mem_fun_pointer_tester(self_type::make_t))
            ));
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC_FULL_VER, >= 140050000)
#pragma warning(pop)
#endif
    };
};

template <typename T>
struct is_member_function_pointer_impl
    : public is_mem_fun_pointer_select<
          ::ndnboost::type_traits::ice_or<
              ::ndnboost::is_reference<T>::value
            , ::ndnboost::is_array<T>::value
            >::value
        >::template result_<T>
{
};

#ifndef NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <typename T>
struct is_member_function_pointer_impl<T&> : public false_type{};
#endif

#else // Borland C++

template <typename T>
struct is_member_function_pointer_impl
{
   static T* m_t;
   NDNBOOST_STATIC_CONSTANT(
              bool, value =
               (1 == sizeof(type_traits::is_mem_fun_pointer_tester(m_t))) );
};

template <typename T>
struct is_member_function_pointer_impl<T&>
{
   NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

#endif

NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_member_function_pointer,void,false)
#ifndef NDNBOOST_NO_CV_VOID_SPECIALIZATIONS
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_member_function_pointer,void const,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_member_function_pointer,void volatile,false)
NDNBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_member_function_pointer,void const volatile,false)
#endif

} // namespace detail

NDNBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_member_function_pointer,T,::ndnboost::detail::is_member_function_pointer_impl<T>::value)

#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace ndnboost

#include <ndnboost/type_traits/detail/bool_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
