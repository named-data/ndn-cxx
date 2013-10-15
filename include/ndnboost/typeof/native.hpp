// Copyright (C) 2006 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_NATIVE_HPP_INCLUDED
#define NDNBOOST_TYPEOF_NATIVE_HPP_INCLUDED

#ifndef MSVC_TYPEOF_HACK

#ifdef NDNBOOST_NO_SFINAE

namespace ndnboost { namespace type_of {

    template<class T> 
        T& ensure_obj(const T&);

}}

#else

#include <ndnboost/type_traits/is_function.hpp> 
#include <ndnboost/utility/enable_if.hpp>

namespace ndnboost { namespace type_of {
# ifdef NDNBOOST_NO_SFINAE
    template<class T> 
    T& ensure_obj(const T&);
# else
    template<typename T>
        typename enable_if<is_function<T>, T&>::type
        ensure_obj(T&);

    template<typename T>
        typename disable_if<is_function<T>, T&>::type
        ensure_obj(const T&);
# endif
}}

#endif//NDNBOOST_NO_SFINAE

#define NDNBOOST_TYPEOF(expr) NDNBOOST_TYPEOF_KEYWORD(ndnboost::type_of::ensure_obj(expr))
#define NDNBOOST_TYPEOF_TPL NDNBOOST_TYPEOF

#define NDNBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
    struct name {\
        typedef NDNBOOST_TYPEOF_TPL(expr) type;\
    };

#define NDNBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
    struct name {\
        typedef NDNBOOST_TYPEOF(expr) type;\
    };

#endif//MSVC_TYPEOF_HACK

#define NDNBOOST_TYPEOF_REGISTER_TYPE(x)
#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE(x, params)

#endif//NDNBOOST_TYPEOF_NATIVE_HPP_INCLUDED

