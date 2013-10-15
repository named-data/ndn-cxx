
#ifndef NDNBOOST_MPL_AUX_ARITY_SPEC_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_ARITY_SPEC_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: arity_spec.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/int.hpp>
#include <ndnboost/mpl/limits/arity.hpp>
#include <ndnboost/mpl/aux_/config/dtp.hpp>
#include <ndnboost/mpl/aux_/preprocessor/params.hpp>
#include <ndnboost/mpl/aux_/arity.hpp>
#include <ndnboost/mpl/aux_/template_arity_fwd.hpp>
#include <ndnboost/mpl/aux_/config/ttp.hpp>
#include <ndnboost/mpl/aux_/config/lambda.hpp>
#include <ndnboost/mpl/aux_/config/static_constant.hpp>

#if defined(NDNBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define NDNBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(i,type,name) \
namespace aux { \
template< NDNBOOST_MPL_AUX_NTTP_DECL(int, N), NDNBOOST_MPL_PP_PARAMS(i,type T) > \
struct arity< \
      name< NDNBOOST_MPL_PP_PARAMS(i,T) > \
    , N \
    > \
{ \
    NDNBOOST_STATIC_CONSTANT(int \
        , value = NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        ); \
}; \
} \
/**/
#else
#   define NDNBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(i,type,name) /**/
#endif

#   define NDNBOOST_MPL_AUX_ARITY_SPEC(i,name) \
    NDNBOOST_MPL_AUX_NONTYPE_ARITY_SPEC(i,typename,name) \
/**/


#if defined(NDNBOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING) \
    && !defined(NDNBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   define NDNBOOST_MPL_AUX_TEMPLATE_ARITY_SPEC(i, name) \
namespace aux { \
template< NDNBOOST_MPL_PP_PARAMS(i,typename T) > \
struct template_arity< name<NDNBOOST_MPL_PP_PARAMS(i,T)> > \
    : int_<i> \
{ \
}; \
} \
/**/
#else
#   define NDNBOOST_MPL_AUX_TEMPLATE_ARITY_SPEC(i, name) /**/
#endif


#endif // NDNBOOST_MPL_AUX_ARITY_SPEC_HPP_INCLUDED
