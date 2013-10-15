
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// $Source$
// $Date: 2011-04-25 05:26:48 -0700 (Mon, 25 Apr 2011) $
// $Revision: 71481 $

#include <ndnboost/type_traits/detail/template_arity_spec.hpp>
#include <ndnboost/type_traits/integral_constant.hpp>
#include <ndnboost/mpl/aux_/lambda_support.hpp>
#include <ndnboost/mpl/size_t.hpp>

#include <cstddef>

#if !defined(NDNBOOST_MSVC) || NDNBOOST_MSVC >= 1300
#   define NDNBOOST_TT_AUX_SIZE_T_BASE(C) public ::ndnboost::integral_constant<std::size_t,C>
#   define NDNBOOST_TT_AUX_SIZE_T_TRAIT_VALUE_DECL(C) /**/
#else
#   define NDNBOOST_TT_AUX_SIZE_T_BASE(C) public ::ndnboost::mpl::size_t<C>
#   define NDNBOOST_TT_AUX_SIZE_T_TRAIT_VALUE_DECL(C) \
    typedef ::ndnboost::mpl::size_t<C> base_; \
    using base_::value; \
    /**/
#endif


#define NDNBOOST_TT_AUX_SIZE_T_TRAIT_DEF1(trait,T,C) \
template< typename T > struct trait \
    : NDNBOOST_TT_AUX_SIZE_T_BASE(C) \
{ \
public:\
    NDNBOOST_TT_AUX_SIZE_T_TRAIT_VALUE_DECL(C) \
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(1,trait,(T)) \
}; \
\
NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1,trait) \
/**/

#define NDNBOOST_TT_AUX_SIZE_T_TRAIT_SPEC1(trait,spec,C) \
template<> struct trait<spec> \
    : NDNBOOST_TT_AUX_SIZE_T_BASE(C) \
{ \
public:\
    NDNBOOST_TT_AUX_SIZE_T_TRAIT_VALUE_DECL(C) \
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(1,trait,(spec)) \
}; \
/**/

#define NDNBOOST_TT_AUX_SIZE_T_TRAIT_PARTIAL_SPEC1_1(param,trait,spec,C) \
template< param > struct trait<spec> \
    : NDNBOOST_TT_AUX_SIZE_T_BASE(C) \
{ \
}; \
/**/
