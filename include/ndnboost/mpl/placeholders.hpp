
#if !defined(NDNBOOST_PP_IS_ITERATING)

///// header body

#ifndef NDNBOOST_MPL_PLACEHOLDERS_HPP_INCLUDED
#define NDNBOOST_MPL_PLACEHOLDERS_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
// Copyright Peter Dimov 2001-2003
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: placeholders.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $


#if !defined(NDNBOOST_MPL_PREPROCESSING_MODE)
#   include <ndnboost/mpl/arg.hpp>
#   include <ndnboost/mpl/aux_/adl_barrier.hpp>

#   if !defined(NDNBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE)
#       define NDNBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(type) \
        using ::NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::type; \
        /**/
#   else
#       define NDNBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(type) /**/
#   endif

#endif

#include <ndnboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
 && !defined(NDNBOOST_MPL_PREPROCESSING_MODE)

#   define NDNBOOST_MPL_PREPROCESSED_HEADER placeholders.hpp
#   include <ndnboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <ndnboost/mpl/aux_/nttp_decl.hpp>
#   include <ndnboost/mpl/limits/arity.hpp>
#   include <ndnboost/preprocessor/iterate.hpp>
#   include <ndnboost/preprocessor/cat.hpp>

// watch out for GNU gettext users, who #define _(x)
#if !defined(_) || defined(NDNBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)
NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN
typedef arg<-1> _;
NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

namespace ndnboost { namespace mpl { 

NDNBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(_)

namespace placeholders {
using NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::_;
}

}}
#endif

/// agurt, 17/mar/02: one more placeholder for the last 'apply#' 
/// specialization
#define NDNBOOST_PP_ITERATION_PARAMS_1 \
    (3,(1, NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY + 1, <ndnboost/mpl/placeholders.hpp>))
#include NDNBOOST_PP_ITERATE()

#endif // NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // NDNBOOST_MPL_PLACEHOLDERS_HPP_INCLUDED

///// iteration

#else
#define i_ NDNBOOST_PP_FRAME_ITERATION(1)

NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

typedef arg<i_> NDNBOOST_PP_CAT(_,i_);

NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

namespace ndnboost { namespace mpl { 

NDNBOOST_MPL_AUX_ARG_ADL_BARRIER_DECL(NDNBOOST_PP_CAT(_,i_))

namespace placeholders {
using NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::NDNBOOST_PP_CAT(_,i_);
}

}}

#undef i_
#endif // NDNBOOST_PP_IS_ITERATING
