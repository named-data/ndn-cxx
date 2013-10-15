
#if !defined(NDNBOOST_PP_IS_ITERATING)

///// header body

#ifndef NDNBOOST_MPL_AUX_ADVANCE_FORWARD_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_ADVANCE_FORWARD_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: advance_forward.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#if !defined(NDNBOOST_MPL_PREPROCESSING_MODE)
#   include <ndnboost/mpl/next.hpp>
#   include <ndnboost/mpl/apply_wrap.hpp>
#endif

#include <ndnboost/mpl/aux_/config/use_preprocessed.hpp>

#if    !defined(NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(NDNBOOST_MPL_PREPROCESSING_MODE)

#   define NDNBOOST_MPL_PREPROCESSED_HEADER advance_forward.hpp
#   include <ndnboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <ndnboost/mpl/limits/unrolling.hpp>
#   include <ndnboost/mpl/aux_/nttp_decl.hpp>
#   include <ndnboost/mpl/aux_/config/eti.hpp>

#   include <ndnboost/preprocessor/iterate.hpp>
#   include <ndnboost/preprocessor/cat.hpp>
#   include <ndnboost/preprocessor/inc.hpp>

namespace ndnboost { namespace mpl { namespace aux {

// forward declaration
template< NDNBOOST_MPL_AUX_NTTP_DECL(long, N) > struct advance_forward;

#   define NDNBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, NDNBOOST_MPL_LIMIT_UNROLLING, <ndnboost/mpl/aux_/advance_forward.hpp>))
#   include NDNBOOST_PP_ITERATE()

// implementation for N that exceeds NDNBOOST_MPL_LIMIT_UNROLLING
template< NDNBOOST_MPL_AUX_NTTP_DECL(long, N) > 
struct advance_forward
{
    template< typename Iterator > struct apply
    {
        typedef typename apply_wrap1<
              advance_forward<NDNBOOST_MPL_LIMIT_UNROLLING>
            , Iterator
            >::type chunk_result_;

        typedef typename apply_wrap1<
              advance_forward<(
                (N - NDNBOOST_MPL_LIMIT_UNROLLING) < 0
                    ? 0
                    : N - NDNBOOST_MPL_LIMIT_UNROLLING
                    )>
            , chunk_result_
            >::type type;
    };
};

}}}

#endif // NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // NDNBOOST_MPL_AUX_ADVANCE_FORWARD_HPP_INCLUDED

///// iteration, depth == 1

// For gcc 4.4 compatability, we must include the
// NDNBOOST_PP_ITERATION_DEPTH test inside an #else clause.
#else // NDNBOOST_PP_IS_ITERATING
#if NDNBOOST_PP_ITERATION_DEPTH() == 1
#define i_ NDNBOOST_PP_FRAME_ITERATION(1)

template<>
struct advance_forward< NDNBOOST_PP_FRAME_ITERATION(1) >
{
    template< typename Iterator > struct apply
    {
        typedef Iterator iter0;

#if i_ > 0
#   define NDNBOOST_PP_ITERATION_PARAMS_2 \
    (3,(1, i_, <ndnboost/mpl/aux_/advance_forward.hpp>))
#   include NDNBOOST_PP_ITERATE()
#endif
        typedef NDNBOOST_PP_CAT(iter,i_) type;
    };

#if defined(NDNBOOST_MPL_CFG_MSVC_60_ETI_BUG)
    /// ETI workaround
    template<> struct apply<int>
    {
        typedef int type;
    };
#endif
};

#undef i_

///// iteration, depth == 2

#elif NDNBOOST_PP_ITERATION_DEPTH() == 2

#   define AUX778076_ITER_0 NDNBOOST_PP_CAT(iter,NDNBOOST_PP_DEC(NDNBOOST_PP_FRAME_ITERATION(2)))
#   define AUX778076_ITER_1 NDNBOOST_PP_CAT(iter,NDNBOOST_PP_FRAME_ITERATION(2))

        typedef typename next<AUX778076_ITER_0>::type AUX778076_ITER_1;
        
#   undef AUX778076_ITER_1
#   undef AUX778076_ITER_0

#endif // NDNBOOST_PP_ITERATION_DEPTH()
#endif // NDNBOOST_PP_IS_ITERATING
