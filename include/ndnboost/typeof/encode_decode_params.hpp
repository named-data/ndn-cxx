// Copyright (C) 2005 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_ENCODE_DECODE_PARAMS_HPP_INCLUDED
#define NDNBOOST_TYPEOF_ENCODE_DECODE_PARAMS_HPP_INCLUDED

#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/repetition/repeat.hpp>

// Assumes iter0 contains initial iterator

#define NDNBOOST_TYPEOF_DECODE_PARAM(z, n, text)   \
    typedef ndnboost::type_of::decode_type<iter##n> decode##n;     \
    typedef typename decode##n::type p##n;      \
    typedef typename decode##n::iter NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n));

#define NDNBOOST_TYPEOF_DECODE_PARAMS(n)\
    NDNBOOST_PP_REPEAT(n, NDNBOOST_TYPEOF_DECODE_PARAM, ~)

// The P0, P1, ... PN are encoded and added to V 

#define NDNBOOST_TYPEOF_ENCODE_PARAMS_BEGIN(z, n, text)\
    typename ndnboost::type_of::encode_type<

#define NDNBOOST_TYPEOF_ENCODE_PARAMS_END(z, n, text)\
    , NDNBOOST_PP_CAT(P, n)>::type

#define NDNBOOST_TYPEOF_ENCODE_PARAMS(n, ID)                   \
    NDNBOOST_PP_REPEAT(n, NDNBOOST_TYPEOF_ENCODE_PARAMS_BEGIN, ~) \
    typename ndnboost::type_of::push_back<V, ndnboost::mpl::size_t<ID> >::type      \
    NDNBOOST_PP_REPEAT(n, NDNBOOST_TYPEOF_ENCODE_PARAMS_END, ~)

#endif//NDNBOOST_TYPEOF_ENCODE_DECODE_PARAMS_HPP_INCLUDED
