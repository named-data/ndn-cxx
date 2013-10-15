
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: numbered_c.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#if defined(NDNBOOST_PP_IS_ITERATING)

#include <ndnboost/preprocessor/enum_params.hpp>
#include <ndnboost/preprocessor/enum_shifted_params.hpp>
#include <ndnboost/preprocessor/dec.hpp>
#include <ndnboost/preprocessor/cat.hpp>

#define i NDNBOOST_PP_FRAME_ITERATION(1)

#if i == 1

template<
      typename T
    , NDNBOOST_PP_ENUM_PARAMS(i, T C)
    >
struct list1_c
    : l_item<
          long_<1>
        , integral_c<T,C0>
        , l_end
        >
{
    typedef list1_c type;
    typedef T value_type;
};

#else

#   define MPL_AUX_LIST_C_TAIL(list, i, C) \
    NDNBOOST_PP_CAT(NDNBOOST_PP_CAT(list,NDNBOOST_PP_DEC(i)),_c)<T, \
      NDNBOOST_PP_ENUM_SHIFTED_PARAMS(i, C) \
    > \
    /**/
    
template<
      typename T
    , NDNBOOST_PP_ENUM_PARAMS(i, T C)
    >
struct NDNBOOST_PP_CAT(NDNBOOST_PP_CAT(list,i),_c)
    : l_item<
          long_<i>
        , integral_c<T,C0>
        , MPL_AUX_LIST_C_TAIL(list,i,C)
        >
{
    typedef NDNBOOST_PP_CAT(NDNBOOST_PP_CAT(list,i),_c) type;
    typedef T value_type;
};

#   undef MPL_AUX_LIST_C_TAIL

#endif // i == 1

#undef i

#endif // NDNBOOST_PP_IS_ITERATING
