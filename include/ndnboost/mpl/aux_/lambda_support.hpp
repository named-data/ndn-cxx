
#ifndef NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: lambda_support.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/config/lambda.hpp>

#if !defined(NDNBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) /**/
#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(i,name,params) /**/

#else

#   include <ndnboost/mpl/int_fwd.hpp>
#   include <ndnboost/mpl/aux_/yes_no.hpp>
#   include <ndnboost/mpl/aux_/na_fwd.hpp>
#   include <ndnboost/mpl/aux_/preprocessor/params.hpp>
#   include <ndnboost/mpl/aux_/preprocessor/enum.hpp>
#   include <ndnboost/mpl/aux_/config/msvc.hpp>
#   include <ndnboost/mpl/aux_/config/workaround.hpp>

#   include <ndnboost/preprocessor/tuple/to_list.hpp>
#   include <ndnboost/preprocessor/list/for_each_i.hpp>
#   include <ndnboost/preprocessor/inc.hpp>
#   include <ndnboost/preprocessor/cat.hpp>

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC(R,typedef_,i,param) \
    typedef_ param NDNBOOST_PP_CAT(arg,NDNBOOST_PP_INC(i)); \
    /**/

// agurt, 07/mar/03: restore an old revision for the sake of SGI MIPSpro C++
#if NDNBOOST_WORKAROUND(__EDG_VERSION__, <= 238) 

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    typedef NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    NDNBOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , NDNBOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    struct rebind \
    { \
        template< NDNBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
            : name< NDNBOOST_MPL_PP_PARAMS(i,U) > \
        { \
        }; \
    }; \
    /**/

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    /**/

#elif NDNBOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(NDNBOOST_INTEL_CXX_VERSION)
// agurt, 18/jan/03: old EDG-based compilers actually enforce 11.4 para 9
// (in strict mode), so we have to provide an alternative to the 
// MSVC-optimized implementation

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    typedef NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    NDNBOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , NDNBOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    struct rebind; \
/**/

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
template< NDNBOOST_MPL_PP_PARAMS(i,typename T) > \
struct name<NDNBOOST_MPL_PP_PARAMS(i,T)>::rebind \
{ \
    template< NDNBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
        : name< NDNBOOST_MPL_PP_PARAMS(i,U) > \
    { \
    }; \
/**/

#else // __EDG_VERSION__

namespace ndnboost { namespace mpl { namespace aux {
template< typename T > struct has_rebind_tag;
}}}

#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    typedef NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    NDNBOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , NDNBOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    friend class NDNBOOST_PP_CAT(name,_rebind); \
    typedef NDNBOOST_PP_CAT(name,_rebind) rebind; \
/**/

#if NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x610))
#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
template< NDNBOOST_MPL_PP_PARAMS(i,typename T) > \
::ndnboost::mpl::aux::yes_tag operator|( \
      ::ndnboost::mpl::aux::has_rebind_tag<int> \
    , name<NDNBOOST_MPL_PP_PARAMS(i,T)>* \
    ); \
::ndnboost::mpl::aux::no_tag operator|( \
      ::ndnboost::mpl::aux::has_rebind_tag<int> \
    , name< NDNBOOST_MPL_PP_ENUM(i,::ndnboost::mpl::na) >* \
    ); \
/**/
#elif !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
template< NDNBOOST_MPL_PP_PARAMS(i,typename T) > \
::ndnboost::mpl::aux::yes_tag operator|( \
      ::ndnboost::mpl::aux::has_rebind_tag<int> \
    , ::ndnboost::mpl::aux::has_rebind_tag< name<NDNBOOST_MPL_PP_PARAMS(i,T)> >* \
    ); \
/**/
#else
#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) /**/
#endif

#   if !defined(__BORLANDC__)
#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
class NDNBOOST_PP_CAT(name,_rebind) \
{ \
 public: \
    template< NDNBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
        : name< NDNBOOST_MPL_PP_PARAMS(i,U) > \
    { \
    }; \
/**/
#   else
#   define NDNBOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
class NDNBOOST_PP_CAT(name,_rebind) \
{ \
 public: \
    template< NDNBOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
    { \
        typedef typename name< NDNBOOST_MPL_PP_PARAMS(i,U) >::type type; \
    }; \
/**/
#   endif // __BORLANDC__

#endif // __EDG_VERSION__

#endif // NDNBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#endif // NDNBOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED
