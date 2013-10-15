
#ifndef NDNBOOST_MPL_VECTOR_VECTOR20_C_HPP_INCLUDED
#define NDNBOOST_MPL_VECTOR_VECTOR20_C_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: vector20_c.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#if !defined(NDNBOOST_MPL_PREPROCESSING_MODE)
#   include <ndnboost/mpl/vector/vector10_c.hpp>
#   include <ndnboost/mpl/vector/vector20.hpp>
#endif

#include <ndnboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(NDNBOOST_MPL_PREPROCESSING_MODE)

#   define NDNBOOST_MPL_PREPROCESSED_HEADER vector20_c.hpp
#   include <ndnboost/mpl/vector/aux_/include_preprocessed.hpp>

#else

#   include <ndnboost/mpl/aux_/config/typeof.hpp>
#   include <ndnboost/mpl/aux_/config/ctps.hpp>
#   include <ndnboost/preprocessor/iterate.hpp>

namespace ndnboost { namespace mpl {

#   define NDNBOOST_PP_ITERATION_PARAMS_1 \
    (3,(11, 20, <ndnboost/mpl/vector/aux_/numbered_c.hpp>))
#   include NDNBOOST_PP_ITERATE()

}}

#endif // NDNBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#endif // NDNBOOST_MPL_VECTOR_VECTOR20_C_HPP_INCLUDED
