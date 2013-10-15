
//  (C) Copyright John Maddock 2005.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_EXTENT_HPP_INCLUDED
#define NDNBOOST_TT_EXTENT_HPP_INCLUDED

// should be the last #include
#include <ndnboost/type_traits/detail/size_t_trait_def.hpp>

namespace ndnboost {

namespace detail{

#if defined( __CODEGEARC__ )
    // wrap the impl as main trait provides additional MPL lambda support
    template < typename T, std::size_t N >
    struct extent_imp {
        static const std::size_t value = __array_extent(T, N);
    };

#else

template <class T, std::size_t N>
struct extent_imp
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = 0);
};
#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <class T, std::size_t R, std::size_t N>
struct extent_imp<T[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct extent_imp<T const[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct extent_imp<T volatile[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct extent_imp<T const volatile[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};

template <class T, std::size_t R>
struct extent_imp<T[R],0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = R);
};

template <class T, std::size_t R>
struct extent_imp<T const[R], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = R);
};

template <class T, std::size_t R>
struct extent_imp<T volatile[R], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = R);
};

template <class T, std::size_t R>
struct extent_imp<T const volatile[R], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = R);
};

#if !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(__IBMCPP__) &&  !NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840)) && !defined(__MWERKS__)
template <class T, std::size_t N>
struct extent_imp<T[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};
template <class T, std::size_t N>
struct extent_imp<T const[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};
template <class T, std::size_t N>
struct extent_imp<T volatile[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};
template <class T, std::size_t N>
struct extent_imp<T const volatile[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::extent_imp<T, N-1>::value));
};
template <class T>
struct extent_imp<T[], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = 0);
};
template <class T>
struct extent_imp<T const[], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = 0);
};
template <class T>
struct extent_imp<T volatile[], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = 0);
};
template <class T>
struct extent_imp<T const volatile[], 0>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = 0);
};
#endif
#endif

#endif  // non-CodeGear implementation
}   // ::ndnboost::detail

template <class T, std::size_t N = 0>
struct extent
   : public ::ndnboost::integral_constant<std::size_t, ::ndnboost::detail::extent_imp<T,N>::value>
{
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300) 
   typedef ::ndnboost::integral_constant<std::size_t, ::ndnboost::detail::extent_imp<T,N>::value> base_; 
   using base_::value;
#endif
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(1,extent,(T))
};

} // namespace ndnboost

#include <ndnboost/type_traits/detail/size_t_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
