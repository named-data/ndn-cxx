// Copyright (C) 2004 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#include <ndnboost/typeof/encode_decode_params.hpp>

// member functions

template<class V, class T, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)> 
struct encode_type_impl<V, R(T::*)(NDNBOOST_PP_ENUM_PARAMS(n, P)) NDNBOOST_TYPEOF_qualifier>
{
    typedef R NDNBOOST_PP_CAT(P, n);
    typedef T NDNBOOST_PP_CAT(P, NDNBOOST_PP_INC(n));
    typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_ADD(n, 2), NDNBOOST_TYPEOF_id + n) type;
};

template<class Iter>
struct decode_type_impl<ndnboost::mpl::size_t<NDNBOOST_TYPEOF_id + n>, Iter>
{
    typedef Iter iter0;
    NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_ADD(n, 2))
    template<class T> struct workaround{
        typedef NDNBOOST_PP_CAT(p, n)(T::*type)(NDNBOOST_PP_ENUM_PARAMS(n, p)) NDNBOOST_TYPEOF_qualifier;
    };
    typedef typename workaround<NDNBOOST_PP_CAT(p, NDNBOOST_PP_INC(n))>::type type;
    typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_ADD(n, 2)) iter;
};

// undef parameters

#undef NDNBOOST_TYPEOF_id
#undef NDNBOOST_TYPEOF_qualifier
