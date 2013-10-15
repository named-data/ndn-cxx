// Copyright (C) 2010 Peder Holt
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_UNSUPPORTED_HPP_INCLUDED
#define NDNBOOST_TYPEOF_UNSUPPORTED_HPP_INCLUDED

namespace ndnboost { namespace type_of {
    struct typeof_emulation_is_unsupported_on_this_compiler {};
}}

#define NDNBOOST_TYPEOF(expr) ndnboost::type_of::typeof_emulation_is_unsupported_on_this_compiler
#define NDNBOOST_TYPEOF_TPL NDNBOOST_TYPEOF

#define NDNBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
struct name {\
    typedef NDNBOOST_TYPEOF_TPL(expr) type;\
};

#define NDNBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
struct name {\
    typedef NDNBOOST_TYPEOF(expr) type;\
};


#define NDNBOOST_TYPEOF_REGISTER_TYPE(x)
#define NDNBOOST_TYPEOF_REGISTER_TEMPLATE(x, params)

#endif
