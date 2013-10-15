// Copyright (C) 2005 Peder Holt
// Copyright (C) 2005 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_TEMPLATE_TEMPLATE_PARAM_HPP_INCLUDED
#define NDNBOOST_TYPEOF_TEMPLATE_TEMPLATE_PARAM_HPP_INCLUDED

#include <ndnboost/preprocessor/logical/or.hpp>
#include <ndnboost/preprocessor/seq/fold_left.hpp>
#include <ndnboost/preprocessor/seq/enum.hpp>

#define NDNBOOST_TYPEOF_MAKE_OBJ_template(x)   NDNBOOST_TYPEOF_TEMPLATE_PARAM(x)
#define NDNBOOST_TYPEOF_TEMPLATE(X) template(X) NDNBOOST_TYPEOF_EAT
#define NDNBOOST_TYPEOF_template(X) (template(X))

#define NDNBOOST_TYPEOF_TEMPLATE_PARAM(Params)\
    (TEMPLATE_PARAM)\
    (Params)

#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(This)\
    NDNBOOST_TYPEOF_TOSEQ(NDNBOOST_PP_SEQ_ELEM(1, This))

//Encode / decode this
#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_ENCODE(This, n)\
   typedef typename ndnboost::type_of::encode_template<NDNBOOST_PP_CAT(V, n),\
       NDNBOOST_PP_CAT(P, n)<NDNBOOST_TYPEOF_SEQ_ENUM(NDNBOOST_TYPEOF_MAKE_OBJS(NDNBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(This)),NDNBOOST_TYPEOF_PLACEHOLDER) >\
   >::type NDNBOOST_PP_CAT(V, NDNBOOST_PP_INC(n));

#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_DECODE(This, n)\
   typedef ndnboost::type_of::decode_template< NDNBOOST_PP_CAT(iter, n) > NDNBOOST_PP_CAT(d, n);\
   typedef typename NDNBOOST_PP_CAT(d, n)::type NDNBOOST_PP_CAT(P, n);\
   typedef typename NDNBOOST_PP_CAT(d, n)::iter NDNBOOST_PP_CAT(iter,NDNBOOST_PP_INC(n));

// template<class, unsigned int, ...> class
#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_EXPANDTYPE(This) \
    template <NDNBOOST_PP_SEQ_ENUM(NDNBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(This)) > class

#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_PLACEHOLDER(Param)\
    Nested_Template_Template_Arguments_Not_Supported

//'template<class,int> class' is reduced to 'class'
#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_DECLARATION_TYPE(Param) class

// T3<int, (unsigned int)0, ...>
#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_PLACEHOLDER_TYPES(Param, n)\
    NDNBOOST_PP_CAT(T,n)<NDNBOOST_TYPEOF_SEQ_ENUM_1(NDNBOOST_TYPEOF_MAKE_OBJS(NDNBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(Param)),NDNBOOST_TYPEOF_PLACEHOLDER) >

#define NDNBOOST_TYPEOF_TEMPLATE_PARAM_ISTEMPLATE 1

////////////////////////////
// move to encode_decode?

NDNBOOST_TYPEOF_BEGIN_ENCODE_NS

template<class V, class Type_Not_Registered_With_Typeof_System> struct encode_template_impl;
template<class T, class Iter> struct decode_template_impl;

NDNBOOST_TYPEOF_END_ENCODE_NS

namespace ndnboost { namespace type_of {

    template<class V, class T> struct encode_template
        : NDNBOOST_TYPEOF_ENCODE_NS_QUALIFIER::encode_template_impl<V, T>
    {};

    template<class Iter> struct decode_template
        : NDNBOOST_TYPEOF_ENCODE_NS_QUALIFIER::decode_template_impl<typename Iter::type, typename Iter::next>
    {};
}}

////////////////////////////
// move to template_encoding.hpp?

//Template template registration
#define NDNBOOST_TYPEOF_REGISTER_TYPE_FOR_TEMPLATE_TEMPLATE(Name,Params,ID)\
    template<class V\
        NDNBOOST_TYPEOF_SEQ_ENUM_TRAILING(Params,NDNBOOST_TYPEOF_REGISTER_TEMPLATE_PARAM_PAIR)\
    >\
    struct encode_template_impl<V,Name<\
        NDNBOOST_PP_ENUM_PARAMS(\
        NDNBOOST_PP_SEQ_SIZE(Params),\
        P)> >\
        : ndnboost::type_of::push_back<V, ndnboost::mpl::size_t<ID> >\
    {\
    };\
    template<class Iter> struct decode_template_impl<ndnboost::mpl::size_t<ID>, Iter>\
    {\
        NDNBOOST_PP_REPEAT(NDNBOOST_PP_SEQ_SIZE(Params),NDNBOOST_TYPEOF_TYPEDEF_INT_PN,_)\
        typedef Name<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_PLACEHOLDER) > type;\
        typedef Iter iter;\
    };

#define NDNBOOST_TYPEOF_TYPEDEF_INT_PN(z,n,Params) typedef int NDNBOOST_PP_CAT(P,n);

#ifdef __BORLANDC__
#define NDNBOOST_TYPEOF_DECODE_NESTED_TEMPLATE_HELPER_NAME NDNBOOST_PP_CAT(\
        NDNBOOST_PP_CAT(\
            NDNBOOST_PP_CAT(\
                decode_nested_template_helper,\
                NDNBOOST_TYPEOF_REGISTRATION_GROUP\
            ),0x10000\
        ),__LINE__\
    )
#define NDNBOOST_TYPEOF_REGISTER_DECODE_NESTED_TEMPLATE_HELPER_IMPL(Name,Params,ID)\
    struct NDNBOOST_TYPEOF_DECODE_NESTED_TEMPLATE_HELPER_NAME {\
        template<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_REGISTER_DECLARE_DECODER_TYPE_PARAM_PAIR) >\
        struct decode_params;\
        template<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_REGISTER_DECODER_TYPE_PARAM_PAIR) >\
        struct decode_params<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_PLACEHOLDER_TYPES) >\
        {\
            typedef Name<NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_SEQ_SIZE(Params),T)> type;\
        };\
    };
//Template template param decoding
#define NDNBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TEMPLATE_TYPE(Name,Params)\
    typedef typename NDNBOOST_TYPEOF_DECODE_NESTED_TEMPLATE_HELPER_NAME::decode_params<NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_SEQ_SIZE(Params),P)>::type type;

#else
#define NDNBOOST_TYPEOF_REGISTER_DECODE_NESTED_TEMPLATE_HELPER_IMPL(Name,Params,ID)

//Template template param decoding
#define NDNBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TEMPLATE_TYPE(Name,Params)\
    template<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_REGISTER_DECLARE_DECODER_TYPE_PARAM_PAIR) >\
    struct decode_params;\
    template<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_REGISTER_DECODER_TYPE_PARAM_PAIR) >\
    struct decode_params<NDNBOOST_TYPEOF_SEQ_ENUM(Params,NDNBOOST_TYPEOF_PLACEHOLDER_TYPES) >\
    {\
        typedef Name<NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_SEQ_SIZE(Params),T)> type;\
    };\
    typedef typename decode_params<NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_SEQ_SIZE(Params),P)>::type type;
#endif
#define NDNBOOST_TYPEOF_REGISTER_DECLARE_DECODER_TYPE_PARAM_PAIR(z,n,elem) \
    NDNBOOST_TYPEOF_VIRTUAL(DECLARATION_TYPE, elem)(elem) NDNBOOST_PP_CAT(T, n)

// NDNBOOST_TYPEOF_HAS_TEMPLATES
#define NDNBOOST_TYPEOF_HAS_TEMPLATES(Params)\
    NDNBOOST_PP_SEQ_FOLD_LEFT(NDNBOOST_TYPEOF_HAS_TEMPLATES_OP, 0, Params)

#define NDNBOOST_TYPEOF_HAS_TEMPLATES_OP(s, state, elem)\
    NDNBOOST_PP_OR(state, NDNBOOST_TYPEOF_VIRTUAL(ISTEMPLATE, elem))

//Define template template arguments
#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE_TEMPLATE_IMPL(Name,Params,ID)\
    NDNBOOST_PP_IF(NDNBOOST_TYPEOF_HAS_TEMPLATES(Params),\
        NDNBOOST_TYPEOF_REGISTER_DECODE_NESTED_TEMPLATE_HELPER_IMPL,\
        NDNBOOST_TYPEOF_REGISTER_TYPE_FOR_TEMPLATE_TEMPLATE)(Name,Params,ID)

#endif //NDNBOOST_TYPEOF_TEMPLATE_TEMPLATE_PARAM_HPP_INCLUDED
