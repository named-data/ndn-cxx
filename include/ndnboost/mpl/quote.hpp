
#if !defined(NDNBOOST_PP_IS_ITERATING)

///// header body

#ifndef NDNBOOST_MPL_QUOTE_HPP_INCLUDED
#define NDNBOOST_MPL_QUOTE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2008
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: quote.hpp 49272 2008-10-11 06:50:46Z agurtovoy $
// $Date: 2008-10-10 23:50:46 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49272 $

#if !defined(NDNBOOST_MPL_PREPROCESSING_MODE)
#   include <ndnboost/mpl/void.hpp>
#   include <ndnboost/mpl/aux_/has_type.hpp>
#endif

#include <ndnboost/mpl/aux_/config/bcc.hpp>
#include <ndnboost/mpl/aux_/config/ttp.hpp>

#if defined(NDNBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS) \
    && !defined(NDNBOOST_MPL_CFG_BCC590_WORKAROUNDS)
#   define NDNBOOST_MPL_CFG_NO_QUOTE_TEMPLATE
#endif

#if !defined(NDNBOOST_MPL_CFG_NO_IMPLICIT_METAFUNCTIONS) \
    && defined(NDNBOOST_MPL_CFG_NO_HAS_XXX)
#   define NDNBOOST_MPL_CFG_NO_IMPLICIT_METAFUNCTIONS
#endif

#include <ndnboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
 && !defined(NDNBOOST_MPL_PREPROCESSING_MODE)

#   define NDNBOOST_MPL_PREPROCESSED_HEADER quote.hpp
#   include <ndnboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <ndnboost/mpl/limits/arity.hpp>
#   include <ndnboost/mpl/aux_/preprocessor/params.hpp>
#   include <ndnboost/mpl/aux_/config/ctps.hpp>
#   include <ndnboost/mpl/aux_/config/workaround.hpp>

#   include <ndnboost/preprocessor/iterate.hpp>
#   include <ndnboost/preprocessor/cat.hpp>

#if !defined(NDNBOOST_MPL_CFG_NO_QUOTE_TEMPLATE)

namespace ndnboost { namespace mpl {

#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< typename T, bool has_type_ >
struct quote_impl
// GCC has a problem with metafunction forwarding when T is a
// specialization of a template called 'type'.
# if NDNBOOST_WORKAROUND(__GNUC__, NDNBOOST_TESTED_AT(4)) \
    && NDNBOOST_WORKAROUND(__GNUC_MINOR__, NDNBOOST_TESTED_AT(0)) \
    && NDNBOOST_WORKAROUND(__GNUC_PATCHLEVEL__, NDNBOOST_TESTED_AT(2))
{
    typedef typename T::type type;
};
# else 
    : T
{
};
# endif 

template< typename T >
struct quote_impl<T,false>
{
    typedef T type;
};

#else // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

template< bool > struct quote_impl
{
    template< typename T > struct result_
        : T
    {
    };
};

template<> struct quote_impl<false>
{
    template< typename T > struct result_
    {
        typedef T type;
    };
};

#endif 

#define NDNBOOST_PP_ITERATION_PARAMS_1 \
    (3,(1, NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <ndnboost/mpl/quote.hpp>))
#include NDNBOOST_PP_ITERATE()

}}

#endif // NDNBOOST_MPL_CFG_NO_QUOTE_TEMPLATE

#endif // NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // NDNBOOST_MPL_QUOTE_HPP_INCLUDED

///// iteration

#else
#define i_ NDNBOOST_PP_FRAME_ITERATION(1)

template<
      template< NDNBOOST_MPL_PP_PARAMS(i_, typename P) > class F
    , typename Tag = void_
    >
struct NDNBOOST_PP_CAT(quote,i_)
{
    template< NDNBOOST_MPL_PP_PARAMS(i_, typename U) > struct apply
#if defined(NDNBOOST_MPL_CFG_BCC590_WORKAROUNDS)
    {
        typedef typename quote_impl<
              F< NDNBOOST_MPL_PP_PARAMS(i_, U) >
            , aux::has_type< F< NDNBOOST_MPL_PP_PARAMS(i_, U) > >::value
            >::type type;
    };
#elif !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
        : quote_impl<
              F< NDNBOOST_MPL_PP_PARAMS(i_, U) >
            , aux::has_type< F< NDNBOOST_MPL_PP_PARAMS(i_, U) > >::value
            >
    {
    };
#else
        : quote_impl< aux::has_type< F< NDNBOOST_MPL_PP_PARAMS(i_, U) > >::value >
            ::template result_< F< NDNBOOST_MPL_PP_PARAMS(i_, U) > >
    {
    };
#endif
};

#undef i_
#endif // NDNBOOST_PP_IS_ITERATING
