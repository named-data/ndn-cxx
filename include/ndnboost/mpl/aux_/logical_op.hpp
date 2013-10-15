
// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: logical_op.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION!

#if !defined(NDNBOOST_MPL_PREPROCESSING_MODE)
#   include <ndnboost/mpl/bool.hpp>
#   include <ndnboost/mpl/aux_/nested_type_wknd.hpp>
#   include <ndnboost/mpl/aux_/na_spec.hpp>
#   include <ndnboost/mpl/aux_/lambda_support.hpp>
#endif

#include <ndnboost/mpl/limits/arity.hpp>
#include <ndnboost/mpl/aux_/preprocessor/params.hpp>
#include <ndnboost/mpl/aux_/preprocessor/ext_params.hpp>
#include <ndnboost/mpl/aux_/preprocessor/def_params_tail.hpp>
#include <ndnboost/mpl/aux_/preprocessor/enum.hpp>
#include <ndnboost/mpl/aux_/preprocessor/sub.hpp>
#include <ndnboost/mpl/aux_/config/ctps.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

#include <ndnboost/preprocessor/dec.hpp>
#include <ndnboost/preprocessor/inc.hpp>
#include <ndnboost/preprocessor/cat.hpp>

namespace ndnboost { namespace mpl {

#   define AUX778076_PARAMS(param, sub) \
    NDNBOOST_MPL_PP_PARAMS( \
          NDNBOOST_MPL_PP_SUB(NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY, sub) \
        , param \
        ) \
    /**/

#   define AUX778076_SHIFTED_PARAMS(param, sub) \
    NDNBOOST_MPL_PP_EXT_PARAMS( \
          2, NDNBOOST_MPL_PP_SUB(NDNBOOST_PP_INC(NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY), sub) \
        , param \
        ) \
    /**/

#   define AUX778076_SPEC_PARAMS(param) \
    NDNBOOST_MPL_PP_ENUM( \
          NDNBOOST_PP_DEC(NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY) \
        , param \
        ) \
    /**/

namespace aux {

#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< bool C_, AUX778076_PARAMS(typename T, 1) >
struct NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)
    : NDNBOOST_PP_CAT(AUX778076_OP_VALUE1,_)
{
};

template< AUX778076_PARAMS(typename T, 1) >
struct NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)< AUX778076_OP_VALUE2,AUX778076_PARAMS(T, 1) >
    : NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          NDNBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        , AUX778076_SHIFTED_PARAMS(T, 1)
        , NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
        >
{
};

template<>
struct NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          AUX778076_OP_VALUE2
        , AUX778076_SPEC_PARAMS(NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_))
        >
    : NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
{
};

#else

template< bool C_ > struct NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)
{
    template< AUX778076_PARAMS(typename T, 1) > struct result_
        : NDNBOOST_PP_CAT(AUX778076_OP_VALUE1,_)
    {
    };
};

template<> struct NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)<AUX778076_OP_VALUE2>
{
    template< AUX778076_PARAMS(typename T, 1) > struct result_
        : NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)< 
              NDNBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
            >::template result_< AUX778076_SHIFTED_PARAMS(T,1),NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_) >
    {
    };

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, == 1300)
    template<> struct result_<AUX778076_SPEC_PARAMS(NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_))>
        : NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
    {
    };
};
#else
};

template<>
struct NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)<AUX778076_OP_VALUE2>
    ::result_< AUX778076_SPEC_PARAMS(NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_)) >
        : NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
{
};
#endif // NDNBOOST_MSVC == 1300

#endif // NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace aux

template<
      typename NDNBOOST_MPL_AUX_NA_PARAM(T1)
    , typename NDNBOOST_MPL_AUX_NA_PARAM(T2)
    NDNBOOST_MPL_PP_DEF_PARAMS_TAIL(2, typename T, NDNBOOST_PP_CAT(AUX778076_OP_VALUE2,_))
    >
struct AUX778076_OP_NAME
#if !defined(NDNBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    : aux::NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          NDNBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        , AUX778076_SHIFTED_PARAMS(T,0)
        >
#else
    : aux::NDNBOOST_PP_CAT(AUX778076_OP_NAME,impl)< 
          NDNBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        >::template result_< AUX778076_SHIFTED_PARAMS(T,0) >
#endif
{
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(
          NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY
        , AUX778076_OP_NAME
        , (AUX778076_PARAMS(T, 0))
        )
};

NDNBOOST_MPL_AUX_NA_SPEC2(
      2
    , NDNBOOST_MPL_LIMIT_METAFUNCTION_ARITY
    , AUX778076_OP_NAME
    )

}}

#undef AUX778076_SPEC_PARAMS
#undef AUX778076_SHIFTED_PARAMS
#undef AUX778076_PARAMS
#undef AUX778076_OP_NAME
#undef AUX778076_OP_VALUE1
#undef AUX778076_OP_VALUE2
