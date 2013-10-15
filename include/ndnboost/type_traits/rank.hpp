
//  (C) Copyright John Maddock 2005.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef NDNBOOST_TT_RANK_HPP_INCLUDED
#define NDNBOOST_TT_RANK_HPP_INCLUDED

// should be the last #include
#include <ndnboost/type_traits/detail/size_t_trait_def.hpp>

namespace ndnboost {

#if !defined( __CODEGEARC__ )

namespace detail{

template <class T, std::size_t N>
struct rank_imp
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = N);
};
#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(NDNBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <class T, std::size_t R, std::size_t N>
struct rank_imp<T[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct rank_imp<T const[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct rank_imp<T volatile[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct rank_imp<T const volatile[R], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};

#if !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(__IBMCPP__) &&  !NDNBOOST_WORKAROUND(__DMC__, NDNBOOST_TESTED_AT(0x840))
template <class T, std::size_t N>
struct rank_imp<T[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};
template <class T, std::size_t N>
struct rank_imp<T const[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};
template <class T, std::size_t N>
struct rank_imp<T volatile[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};
template <class T, std::size_t N>
struct rank_imp<T const volatile[], N>
{
   NDNBOOST_STATIC_CONSTANT(std::size_t, value = (::ndnboost::detail::rank_imp<T, N+1>::value));
};
#endif
#endif
}

#endif // !defined( __CODEGEARC__ )

#if defined( __CODEGEARC__ )
NDNBOOST_TT_AUX_SIZE_T_TRAIT_DEF1(rank,T,__array_rank(T))
#else
NDNBOOST_TT_AUX_SIZE_T_TRAIT_DEF1(rank,T,(::ndnboost::detail::rank_imp<T,0>::value))
#endif

} // namespace ndnboost

#include <ndnboost/type_traits/detail/size_t_trait_undef.hpp>

#endif // NDNBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
