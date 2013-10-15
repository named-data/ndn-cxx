// Copyright (C) 2004 Arkadiy Vertleyb

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// boostinspect:nounnamed

#ifndef NDNBOOST_TYPEOF_ENCODE_DECODE_HPP_INCLUDED
#define NDNBOOST_TYPEOF_ENCODE_DECODE_HPP_INCLUDED

#include <ndnboost/mpl/deref.hpp>
#include <ndnboost/mpl/next.hpp>

#ifndef NDNBOOST_TYPEOF_SUPPRESS_UNNAMED_NAMESPACE

#   define NDNBOOST_TYPEOF_BEGIN_ENCODE_NS namespace { namespace ndnboost_typeof {
#   define NDNBOOST_TYPEOF_END_ENCODE_NS }}
#   define NDNBOOST_TYPEOF_ENCODE_NS_QUALIFIER ndnboost_typeof

#else

#   define NDNBOOST_TYPEOF_BEGIN_ENCODE_NS namespace ndnboost { namespace type_of {
#   define NDNBOOST_TYPEOF_END_ENCODE_NS }}
#   define NDNBOOST_TYPEOF_ENCODE_NS_QUALIFIER ndnboost::type_of

#   define NDNBOOST_TYPEOF_TEXT "unnamed namespace is off"
#   include <ndnboost/typeof/message.hpp>

#endif

NDNBOOST_TYPEOF_BEGIN_ENCODE_NS

template<class V, class Type_Not_Registered_With_Typeof_System>
struct encode_type_impl;

template<class T, class Iter>
struct decode_type_impl
{
    typedef int type;  // MSVC ETI workaround
};

template<class T>
struct decode_nested_template_helper_impl;

NDNBOOST_TYPEOF_END_ENCODE_NS

namespace ndnboost { namespace type_of {

    template<class V, class T>
    struct encode_type : NDNBOOST_TYPEOF_ENCODE_NS_QUALIFIER::encode_type_impl<V, T>
    {};

    template<class Iter>
    struct decode_type : NDNBOOST_TYPEOF_ENCODE_NS_QUALIFIER::decode_type_impl<
        typename Iter::type,
        typename Iter::next
    >
    {};
}}

#endif//NDNBOOST_TYPEOF_ENCODE_DECODE_HPP_INCLUDED
