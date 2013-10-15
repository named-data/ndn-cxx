// Copyright (C) 2005 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_TYPE_TEMPLATE_PARAM_HPP_INCLUDED
#define NDNBOOST_TYPEOF_TYPE_TEMPLATE_PARAM_HPP_INCLUDED

#define NDNBOOST_TYPEOF_class_NDNBOOST_TYPEOF (class)
#define NDNBOOST_TYPEOF_typename_NDNBOOST_TYPEOF (typename)

#define NDNBOOST_TYPEOF_MAKE_OBJ_class NDNBOOST_TYPEOF_TYPE_PARAM
#define NDNBOOST_TYPEOF_MAKE_OBJ_typename NDNBOOST_TYPEOF_TYPE_PARAM

#define NDNBOOST_TYPEOF_TYPE_PARAM\
    (TYPE_PARAM)

#define NDNBOOST_TYPEOF_TYPE_PARAM_EXPANDTYPE(Param) class

// TYPE_PARAM "virtual functions" implementation

#define NDNBOOST_TYPEOF_TYPE_PARAM_ENCODE(This, n)\
    typedef typename ndnboost::type_of::encode_type<\
        NDNBOOST_PP_CAT(V, n),\
        NDNBOOST_PP_CAT(P, n)\
    >::type NDNBOOST_PP_CAT(V, NDNBOOST_PP_INC(n)); 

#define NDNBOOST_TYPEOF_TYPE_PARAM_DECODE(This, n)\
    typedef ndnboost::type_of::decode_type< NDNBOOST_PP_CAT(iter, n) > NDNBOOST_PP_CAT(d, n);\
    typedef typename NDNBOOST_PP_CAT(d, n)::type NDNBOOST_PP_CAT(P, n);\
    typedef typename NDNBOOST_PP_CAT(d, n)::iter NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n));

#define NDNBOOST_TYPEOF_TYPE_PARAM_PLACEHOLDER(Param) int
#define NDNBOOST_TYPEOF_TYPE_PARAM_DECLARATION_TYPE(Param) class
#define NDNBOOST_TYPEOF_TYPE_PARAM_PLACEHOLDER_TYPES(Param, n) NDNBOOST_PP_CAT(T,n)
#define NDNBOOST_TYPEOF_TYPE_PARAM_ISTEMPLATE 0

#endif//NDNBOOST_TYPEOF_TYPE_TEMPLATE_PARAM_HPP_INCLUDED
