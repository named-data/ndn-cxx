// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_VALUE_TYPE_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_VALUE_TYPE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <ndnboost/iostreams/traits.hpp>
#include <ndnboost/mpl/if.hpp>

namespace ndnboost { namespace iostreams { namespace detail {

template<typename T>
struct param_type {
    typedef typename mpl::if_<is_std_io<T>, T&, const T&>::type type;
};

template<typename T>
struct value_type {
    typedef typename mpl::if_<is_std_io<T>, T&, T>::type type;
};

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_VALUE_TYPE_HPP_INCLUDED //-----------//
