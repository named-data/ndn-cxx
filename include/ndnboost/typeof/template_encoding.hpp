// Copyright (C) 2004 Arkadiy Vertleyb
// Copyright (C) 2005 Peder Holt
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_TEMPLATE_ENCODING_HPP_INCLUDED
#define NDNBOOST_TYPEOF_TEMPLATE_ENCODING_HPP_INCLUDED

#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/preprocessor/repetition/enum_trailing.hpp>
#include <ndnboost/preprocessor/control/iif.hpp>
#include <ndnboost/preprocessor/detail/is_unary.hpp>
#include <ndnboost/preprocessor/repetition/repeat.hpp>
#include <ndnboost/preprocessor/tuple/eat.hpp>
#include <ndnboost/preprocessor/seq/transform.hpp>
#include <ndnboost/preprocessor/seq/for_each_i.hpp>
#include <ndnboost/preprocessor/seq/cat.hpp>

#include <ndnboost/typeof/encode_decode.hpp>
#include <ndnboost/typeof/int_encoding.hpp>

#include <ndnboost/typeof/type_template_param.hpp>
#include <ndnboost/typeof/integral_template_param.hpp>
#include <ndnboost/typeof/template_template_param.hpp>

#ifdef __BORLANDC__
#define NDNBOOST_TYPEOF_QUALIFY(P) self_t::P
#else
#define NDNBOOST_TYPEOF_QUALIFY(P) P
#endif
// The template parameter description, entered by the user,
// is converted into a polymorphic "object"
// that is used to generate the code responsible for
// encoding/decoding the parameter, etc.

// make sure to cat the sequence first, and only then add the prefix.
#define NDNBOOST_TYPEOF_MAKE_OBJ(elem) NDNBOOST_PP_CAT(\
    NDNBOOST_TYPEOF_MAKE_OBJ,\
    NDNBOOST_PP_SEQ_CAT((_) NDNBOOST_TYPEOF_TO_SEQ(elem))\
    )

#define NDNBOOST_TYPEOF_TO_SEQ(tokens) NDNBOOST_TYPEOF_ ## tokens ## _NDNBOOST_TYPEOF

// NDNBOOST_TYPEOF_REGISTER_TEMPLATE

#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE_EXPLICIT_ID(Name, Params, Id)\
    NDNBOOST_TYPEOF_REGISTER_TEMPLATE_IMPL(\
        Name,\
        NDNBOOST_TYPEOF_MAKE_OBJS(NDNBOOST_TYPEOF_TOSEQ(Params)),\
        NDNBOOST_PP_SEQ_SIZE(NDNBOOST_TYPEOF_TOSEQ(Params)),\
        Id)

#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE(Name, Params)\
    NDNBOOST_TYPEOF_REGISTER_TEMPLATE_EXPLICIT_ID(Name, Params, NDNBOOST_TYPEOF_UNIQUE_ID())

#define NDNBOOST_TYPEOF_OBJECT_MAKER(s, data, elem)\
    NDNBOOST_TYPEOF_MAKE_OBJ(elem)

#define NDNBOOST_TYPEOF_MAKE_OBJS(Params)\
    NDNBOOST_PP_SEQ_TRANSFORM(NDNBOOST_TYPEOF_OBJECT_MAKER, ~, Params)

// As suggested by Paul Mensonides:

#define NDNBOOST_TYPEOF_TOSEQ(x)\
    NDNBOOST_PP_IIF(\
        NDNBOOST_PP_IS_UNARY(x),\
        x NDNBOOST_PP_TUPLE_EAT(3), NDNBOOST_PP_REPEAT\
    )(x, NDNBOOST_TYPEOF_TOSEQ_2, ~)

#define NDNBOOST_TYPEOF_TOSEQ_2(z, n, _) (class)

// NDNBOOST_TYPEOF_VIRTUAL

#define NDNBOOST_TYPEOF_CAT_4(a, b, c, d) NDNBOOST_TYPEOF_CAT_4_I(a, b, c, d)
#define NDNBOOST_TYPEOF_CAT_4_I(a, b, c, d) a ## b ## c ## d

#define NDNBOOST_TYPEOF_VIRTUAL(Fun, Obj)\
    NDNBOOST_TYPEOF_CAT_4(NDNBOOST_TYPEOF_, NDNBOOST_PP_SEQ_HEAD(Obj), _, Fun)

// NDNBOOST_TYPEOF_SEQ_ENUM[_TRAILING][_1]
// Two versions provided due to reentrancy issue

#define NDNBOOST_TYPEOF_SEQ_EXPAND_ELEMENT(z,n,seq)\
   NDNBOOST_PP_SEQ_ELEM(0,seq) (z,n,NDNBOOST_PP_SEQ_ELEM(n,NDNBOOST_PP_SEQ_ELEM(1,seq)))

#define NDNBOOST_TYPEOF_SEQ_ENUM(seq,macro)\
    NDNBOOST_PP_ENUM(NDNBOOST_PP_SEQ_SIZE(seq),NDNBOOST_TYPEOF_SEQ_EXPAND_ELEMENT,(macro)(seq))

#define NDNBOOST_TYPEOF_SEQ_ENUM_TRAILING(seq,macro)\
    NDNBOOST_PP_ENUM_TRAILING(NDNBOOST_PP_SEQ_SIZE(seq),NDNBOOST_TYPEOF_SEQ_EXPAND_ELEMENT,(macro)(seq))

#define NDNBOOST_TYPEOF_SEQ_EXPAND_ELEMENT_1(z,n,seq)\
    NDNBOOST_PP_SEQ_ELEM(0,seq) (z,n,NDNBOOST_PP_SEQ_ELEM(n,NDNBOOST_PP_SEQ_ELEM(1,seq)))

#define NDNBOOST_TYPEOF_SEQ_ENUM_1(seq,macro)\
    NDNBOOST_PP_ENUM(NDNBOOST_PP_SEQ_SIZE(seq),NDNBOOST_TYPEOF_SEQ_EXPAND_ELEMENT_1,(macro)(seq))

#define NDNBOOST_TYPEOF_SEQ_ENUM_TRAILING_1(seq,macro)\
    NDNBOOST_PP_ENUM_TRAILING(NDNBOOST_PP_SEQ_SIZE(seq),NDNBOOST_TYPEOF_SEQ_EXPAND_ELEMENT_1,(macro)(seq))

//

#define NDNBOOST_TYPEOF_PLACEHOLDER(z, n, elem)\
    NDNBOOST_TYPEOF_VIRTUAL(PLACEHOLDER, elem)(elem)

#define NDNBOOST_TYPEOF_PLACEHOLDER_TYPES(z, n, elem)\
    NDNBOOST_TYPEOF_VIRTUAL(PLACEHOLDER_TYPES, elem)(elem, n)

#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE_ENCODE_PARAM(r, data, n, elem)\
    NDNBOOST_TYPEOF_VIRTUAL(ENCODE, elem)(elem, n)

#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE_DECODE_PARAM(r, data, n, elem)\
    NDNBOOST_TYPEOF_VIRTUAL(DECODE, elem)(elem, n)

#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE_PARAM_PAIR(z, n, elem) \
    NDNBOOST_TYPEOF_VIRTUAL(EXPANDTYPE, elem)(elem) NDNBOOST_PP_CAT(P, n)

#define NDNBOOST_TYPEOF_REGISTER_DEFAULT_TEMPLATE_TYPE(Name,Params,ID)\
    Name< NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_SEQ_SIZE(Params), P) >

//Since we are creating an internal decode struct, we need to use different template names, T instead of P.
#define NDNBOOST_TYPEOF_REGISTER_DECODER_TYPE_PARAM_PAIR(z,n,elem) \
    NDNBOOST_TYPEOF_VIRTUAL(EXPANDTYPE, elem)(elem) NDNBOOST_PP_CAT(T, n)

//Default template param decoding

#define NDNBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TYPE(Name,Params)\
    typedef Name<NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_SEQ_SIZE(Params),NDNBOOST_TYPEOF_QUALIFY(P))> type;

//Branch the decoding
#define NDNBOOST_TYPEOF_TYPEDEF_DECODED_TYPE(Name,Params)\
    NDNBOOST_PP_IF(NDNBOOST_TYPEOF_HAS_TEMPLATES(Params),\
        NDNBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TEMPLATE_TYPE,\
        NDNBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TYPE)(Name,Params)

#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE_IMPL(Name, Params, Size, ID)\
    NDNBOOST_TYPEOF_BEGIN_ENCODE_NS\
    NDNBOOST_TYPEOF_REGISTER_TEMPLATE_TEMPLATE_IMPL(Name, Params, ID)\
    template<class V\
        NDNBOOST_TYPEOF_SEQ_ENUM_TRAILING(Params, NDNBOOST_TYPEOF_REGISTER_TEMPLATE_PARAM_PAIR)\
    >\
    struct encode_type_impl<V, Name<NDNBOOST_PP_ENUM_PARAMS(Size, P)> >\
    {\
        typedef typename ndnboost::type_of::push_back<V, ndnboost::mpl::size_t<ID> >::type V0;\
        NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_TYPEOF_REGISTER_TEMPLATE_ENCODE_PARAM, ~, Params)\
        typedef NDNBOOST_PP_CAT(V, Size) type;\
    };\
    template<class Iter>\
    struct decode_type_impl<ndnboost::mpl::size_t<ID>, Iter>\
    {\
        typedef decode_type_impl<ndnboost::mpl::size_t<ID>, Iter> self_t;\
        typedef ndnboost::mpl::size_t<ID> self_id;\
        typedef Iter iter0;\
        NDNBOOST_PP_SEQ_FOR_EACH_I(NDNBOOST_TYPEOF_REGISTER_TEMPLATE_DECODE_PARAM, ~, Params)\
        NDNBOOST_TYPEOF_TYPEDEF_DECODED_TYPE(Name, Params)\
        typedef NDNBOOST_PP_CAT(iter, Size) iter;\
    };\
    NDNBOOST_TYPEOF_END_ENCODE_NS

#endif//NDNBOOST_TYPEOF_TEMPLATE_ENCODING_HPP_INCLUDED
