// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <ndnboost/typeof/encode_decode_params.hpp>

#define n NDNBOOST_PP_ITERATION()

// function pointers

template<class V, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
struct encode_type_impl<V, R(*)(NDNBOOST_PP_ENUM_PARAMS(n, P))>
{
    typedef R NDNBOOST_PP_CAT(P, n);
    typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_INC(n), FUN_PTR_ID + n) type;
};

template<class V, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
struct encode_type_impl<V, R(*)(NDNBOOST_PP_ENUM_PARAMS(n, P) ...)>
{
    typedef R NDNBOOST_PP_CAT(P, n);
    typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_INC(n), FUN_VAR_PTR_ID + n) type;
};

template<class Iter>
struct decode_type_impl<ndnboost::mpl::size_t<FUN_PTR_ID + n>, Iter>
{
    typedef Iter iter0;
    NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_INC(n))
    typedef NDNBOOST_PP_CAT(p, n)(*type)(NDNBOOST_PP_ENUM_PARAMS(n, p));
    typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n)) iter;
};

template<class Iter>
struct decode_type_impl<ndnboost::mpl::size_t<FUN_VAR_PTR_ID + n>, Iter>
{
    typedef Iter iter0;
    NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_INC(n))
    typedef NDNBOOST_PP_CAT(p, n)(*type)(NDNBOOST_PP_ENUM_PARAMS(n, p) ...);
    typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n)) iter;
};

#ifndef NDNBOOST_TYPEOF_NO_FUNCTION_TYPES

    // function references

    template<class V, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(&)(NDNBOOST_PP_ENUM_PARAMS(n, P))>
    {
        typedef R NDNBOOST_PP_CAT(P, n);
        typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_INC(n), FUN_REF_ID + n) type;
    };

    template<class V, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(&)(NDNBOOST_PP_ENUM_PARAMS(n, P) ...)>
    {
        typedef R NDNBOOST_PP_CAT(P, n);
        typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_INC(n), FUN_VAR_REF_ID + n) type;
    };

    template<class Iter>
    struct decode_type_impl<ndnboost::mpl::size_t<FUN_REF_ID + n>, Iter>
    {
        typedef Iter iter0;
        NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_INC(n))
        typedef NDNBOOST_PP_CAT(p, n)(&type)(NDNBOOST_PP_ENUM_PARAMS(n, p));
        typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n)) iter;
    };

    template<class Iter>
    struct decode_type_impl<ndnboost::mpl::size_t<FUN_VAR_REF_ID + n>, Iter>
    {
        typedef Iter iter0;
        NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_INC(n))
        typedef NDNBOOST_PP_CAT(p, n)(&type)(NDNBOOST_PP_ENUM_PARAMS(n, p) ...);
        typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n)) iter;
    };

    // functions

    template<class V, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(NDNBOOST_PP_ENUM_PARAMS(n, P))>
    {
        typedef R NDNBOOST_PP_CAT(P, n);
        typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_INC(n), FUN_ID + n) type;
    };

    template<class V, class R NDNBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(NDNBOOST_PP_ENUM_PARAMS(n, P) ...)>
    {
        typedef R NDNBOOST_PP_CAT(P, n);
        typedef NDNBOOST_TYPEOF_ENCODE_PARAMS(NDNBOOST_PP_INC(n), FUN_VAR_ID + n) type;
    };

    template<class Iter>
    struct decode_type_impl<ndnboost::mpl::size_t<FUN_ID + n>, Iter>
    {
        typedef Iter iter0;
        NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_INC(n))
        typedef NDNBOOST_PP_CAT(p, n)(type)(NDNBOOST_PP_ENUM_PARAMS(n, p));
        typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n)) iter;
    };

    template<class Iter>
    struct decode_type_impl<ndnboost::mpl::size_t<FUN_VAR_ID + n>, Iter>
    {
        typedef Iter iter0;
        NDNBOOST_TYPEOF_DECODE_PARAMS(NDNBOOST_PP_INC(n))
        typedef NDNBOOST_PP_CAT(p, n)(type)(NDNBOOST_PP_ENUM_PARAMS(n, p) ...);
        typedef NDNBOOST_PP_CAT(iter, NDNBOOST_PP_INC(n)) iter;
    };

#endif//NDNBOOST_TYPEOF_NO_FUNCTION_TYPES

#ifndef NDNBOOST_TYPEOF_NO_MEMBER_FUNCTION_TYPES
// member functions

#define NDNBOOST_TYPEOF_qualifier
#define NDNBOOST_TYPEOF_id MEM_FUN_ID
#include <ndnboost/typeof/register_mem_functions.hpp>

#define NDNBOOST_TYPEOF_qualifier const
#define NDNBOOST_TYPEOF_id CONST_MEM_FUN_ID
#include <ndnboost/typeof/register_mem_functions.hpp>

#define NDNBOOST_TYPEOF_qualifier volatile
#define NDNBOOST_TYPEOF_id VOLATILE_MEM_FUN_ID
#include <ndnboost/typeof/register_mem_functions.hpp>

#define NDNBOOST_TYPEOF_qualifier volatile const
#define NDNBOOST_TYPEOF_id VOLATILE_CONST_MEM_FUN_ID
#include <ndnboost/typeof/register_mem_functions.hpp>

#undef n
#endif
