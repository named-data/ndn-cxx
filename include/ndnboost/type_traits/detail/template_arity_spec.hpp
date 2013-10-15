
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

#include <ndnboost/mpl/int.hpp>
#include <ndnboost/mpl/aux_/template_arity_fwd.hpp>
#include <ndnboost/mpl/aux_/preprocessor/params.hpp>
#include <ndnboost/mpl/aux_/config/lambda.hpp>
#include <ndnboost/mpl/aux_/config/overload_resolution.hpp>

#if defined(NDNBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT) \
    && defined(NDNBOOST_MPL_CFG_BROKEN_OVERLOAD_RESOLUTION)
#   define NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(i, name) \
namespace mpl { namespace aux { \
template< NDNBOOST_MPL_PP_PARAMS(i, typename T) > \
struct template_arity< \
          name< NDNBOOST_MPL_PP_PARAMS(i, T) > \
        > \
    : int_<i> \
{ \
}; \
}} \
/**/
#else
#   define NDNBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(i, name) /**/
#endif
