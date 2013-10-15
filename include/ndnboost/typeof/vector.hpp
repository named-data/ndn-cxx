// Copyright (C) 2005 Arkadiy Vertleyb
// Copyright (C) 2005 Peder Holt
//
// Copyright (C) 2006 Tobias Schwinger
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_VECTOR_HPP_INCLUDED

#include <ndnboost/mpl/int.hpp>
#include <ndnboost/preprocessor/iteration/self.hpp>

#ifndef NDNBOOST_TYPEOF_LIMIT_SIZE
#   define NDNBOOST_TYPEOF_LIMIT_SIZE 50
#endif

//
// To recreate the preprocessed versions of this file preprocess and run
//
//   $(NDNBOOST_ROOT)/libs/typeof/tools/preprocess.pl
//

#if defined(NDNBOOST_TYPEOF_PP_INCLUDE_EXTERNAL)

#   undef NDNBOOST_TYPEOF_PP_INCLUDE_EXTERNAL

#elif !defined(NDNBOOST_TYPEOF_PREPROCESSING_MODE) && !NDNBOOST_PP_IS_SELFISH

#   define NDNBOOST_PP_INDIRECT_SELF <ndnboost/typeof/vector.hpp>
#   if   NDNBOOST_TYPEOF_LIMIT_SIZE < 50
#     include NDNBOOST_PP_INCLUDE_SELF()
#   elif NDNBOOST_TYPEOF_LIMIT_SIZE < 100
#     include <ndnboost/typeof/vector50.hpp>
#     define  NDNBOOST_TYPEOF_PP_START_SIZE 51
#     include NDNBOOST_PP_INCLUDE_SELF()
#   elif NDNBOOST_TYPEOF_LIMIT_SIZE < 150
#     include <ndnboost/typeof/vector100.hpp>
#     define  NDNBOOST_TYPEOF_PP_START_SIZE 101
#     include NDNBOOST_PP_INCLUDE_SELF()
#   elif NDNBOOST_TYPEOF_LIMIT_SIZE < 200
#     include <ndnboost/typeof/vector150.hpp>
#     define  NDNBOOST_TYPEOF_PP_START_SIZE 151
#     include NDNBOOST_PP_INCLUDE_SELF()
#   elif NDNBOOST_TYPEOF_LIMIT_SIZE <= 250
#     include <ndnboost/typeof/vector200.hpp>
#     define  NDNBOOST_TYPEOF_PP_START_SIZE 201
#     include NDNBOOST_PP_INCLUDE_SELF()
#   else
#     error "NDNBOOST_TYPEOF_LIMIT_SIZE too high"
#   endif

#else// defined(NDNBOOST_TYPEOF_PREPROCESSING_MODE) || NDNBOOST_PP_IS_SELFISH

#   ifndef NDNBOOST_TYPEOF_PP_NEXT_SIZE
#     define NDNBOOST_TYPEOF_PP_NEXT_SIZE NDNBOOST_TYPEOF_LIMIT_SIZE
#   endif
#   ifndef NDNBOOST_TYPEOF_PP_START_SIZE
#     define NDNBOOST_TYPEOF_PP_START_SIZE 0
#   endif

#   if NDNBOOST_TYPEOF_PP_START_SIZE <= NDNBOOST_TYPEOF_LIMIT_SIZE

#     include <ndnboost/preprocessor/enum_params.hpp>
#     include <ndnboost/preprocessor/repeat.hpp>
#     include <ndnboost/preprocessor/repeat_from_to.hpp>
#     include <ndnboost/preprocessor/cat.hpp>
#     include <ndnboost/preprocessor/inc.hpp>
#     include <ndnboost/preprocessor/dec.hpp>
#     include <ndnboost/preprocessor/comma_if.hpp>
#     include <ndnboost/preprocessor/iteration/local.hpp>
#     include <ndnboost/preprocessor/control/expr_iif.hpp>
#     include <ndnboost/preprocessor/logical/not.hpp>

// iterator

#     define NDNBOOST_TYPEOF_spec_iter(n)\
        template<class V>\
        struct v_iter<V, mpl::int_<n> >\
        {\
            typedef typename V::item ## n type;\
            typedef v_iter<V, mpl::int_<n + 1> > next;\
        };

namespace ndnboost { namespace type_of {

    template<class V, class Increase_NDNBOOST_TYPEOF_LIMIT_SIZE> struct v_iter; // not defined
#     define  NDNBOOST_PP_LOCAL_MACRO  NDNBOOST_TYPEOF_spec_iter
#     define  NDNBOOST_PP_LOCAL_LIMITS \
        (NDNBOOST_PP_DEC(NDNBOOST_TYPEOF_PP_START_SIZE), \
         NDNBOOST_PP_DEC(NDNBOOST_TYPEOF_LIMIT_SIZE))
#     include NDNBOOST_PP_LOCAL_ITERATE()

}}

#     undef NDNBOOST_TYPEOF_spec_iter

// vector

#     define NDNBOOST_TYPEOF_typedef_item(z, n, _)\
        typedef P ## n item ## n;

#     define NDNBOOST_TYPEOF_typedef_fake_item(z, n, _)\
        typedef mpl::int_<1> item ## n;

#     define NDNBOOST_TYPEOF_define_vector(n)\
        template<NDNBOOST_PP_ENUM_PARAMS(n, class P) NDNBOOST_PP_EXPR_IIF(NDNBOOST_PP_NOT(n), class T = void)>\
        struct vector ## n\
        {\
            typedef v_iter<vector ## n<NDNBOOST_PP_ENUM_PARAMS(n,P)>, ndnboost::mpl::int_<0> > begin;\
            NDNBOOST_PP_REPEAT(n, NDNBOOST_TYPEOF_typedef_item, ~)\
            NDNBOOST_PP_REPEAT_FROM_TO(n, NDNBOOST_TYPEOF_PP_NEXT_SIZE, NDNBOOST_TYPEOF_typedef_fake_item, ~)\
        };

namespace ndnboost { namespace type_of {

#     define  NDNBOOST_PP_LOCAL_MACRO  NDNBOOST_TYPEOF_define_vector
#     define  NDNBOOST_PP_LOCAL_LIMITS \
        (NDNBOOST_TYPEOF_PP_START_SIZE,NDNBOOST_TYPEOF_LIMIT_SIZE)
#     include NDNBOOST_PP_LOCAL_ITERATE()

}}

#     undef NDNBOOST_TYPEOF_typedef_item
#     undef NDNBOOST_TYPEOF_typedef_fake_item
#     undef NDNBOOST_TYPEOF_define_vector

// push_back

#     define NDNBOOST_TYPEOF_spec_push_back(n)\
        template<NDNBOOST_PP_ENUM_PARAMS(n, class P) NDNBOOST_PP_COMMA_IF(n) class T>\
        struct push_back<NDNBOOST_PP_CAT(ndnboost::type_of::vector, n)<NDNBOOST_PP_ENUM_PARAMS(n, P)>, T>\
        {\
            typedef NDNBOOST_PP_CAT(ndnboost::type_of::vector, NDNBOOST_PP_INC(n))<\
                NDNBOOST_PP_ENUM_PARAMS(n, P) NDNBOOST_PP_COMMA_IF(n) T\
            > type;\
        };

namespace ndnboost { namespace type_of {

#   if   NDNBOOST_TYPEOF_LIMIT_SIZE < 50
    template<class V, class T> struct push_back {
        typedef V type;
    };
#   endif
    //default behaviour is to let push_back ignore T, and return the input vector.
    //This is to let NDNBOOST_TYPEOF_NESTED_TYPEDEF work properly with the default vector.
#     define  NDNBOOST_PP_LOCAL_MACRO  NDNBOOST_TYPEOF_spec_push_back
#     define  NDNBOOST_PP_LOCAL_LIMITS \
        (NDNBOOST_PP_DEC(NDNBOOST_TYPEOF_PP_START_SIZE), \
         NDNBOOST_PP_DEC(NDNBOOST_TYPEOF_LIMIT_SIZE))
#     include NDNBOOST_PP_LOCAL_ITERATE()

}}

#     undef NDNBOOST_TYPEOF_spec_push_back

#   endif//NDNBOOST_TYPEOF_PP_START_SIZE<=NDNBOOST_TYPEOF_LIMIT_SIZE
#   undef  NDNBOOST_TYPEOF_PP_START_SIZE
#   undef  NDNBOOST_TYPEOF_PP_NEXT_SIZE

#endif//NDNBOOST_TYPEOF_PREPROCESSING_MODE || NDNBOOST_PP_IS_SELFISH

#define NDNBOOST_TYPEOF_VECTOR_HPP_INCLUDED
#endif//NDNBOOST_TYPEOF_VECTOR_HPP_INCLUDED

