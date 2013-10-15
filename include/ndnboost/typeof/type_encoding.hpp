// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_TYPE_ENCODING_HPP_INCLUDED
#define NDNBOOST_TYPEOF_TYPE_ENCODING_HPP_INCLUDED

#define NDNBOOST_TYPEOF_REGISTER_TYPE_IMPL(T, Id)                          \
                                                                        \
    template<class V> struct encode_type_impl<V, T >                    \
        : ndnboost::type_of::push_back<V, ndnboost::mpl::size_t<Id> >         \
    {};                                                                 \
    template<class Iter> struct decode_type_impl<ndnboost::mpl::size_t<Id>, Iter> \
    {                                                                   \
        typedef T type;                                                 \
        typedef Iter iter;                                              \
    };

#define NDNBOOST_TYPEOF_REGISTER_TYPE_EXPLICIT_ID(Type, Id)                \
    NDNBOOST_TYPEOF_BEGIN_ENCODE_NS                                        \
    NDNBOOST_TYPEOF_REGISTER_TYPE_IMPL(Type, Id)                           \
    NDNBOOST_TYPEOF_END_ENCODE_NS

#define NDNBOOST_TYPEOF_REGISTER_TYPE(Type)                                \
    NDNBOOST_TYPEOF_REGISTER_TYPE_EXPLICIT_ID(Type, NDNBOOST_TYPEOF_UNIQUE_ID())

#endif//NDNBOOST_TYPEOF_TYPE_ENCODING_HPP_INCLUDED
