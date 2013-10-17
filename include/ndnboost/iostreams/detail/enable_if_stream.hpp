// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_ENABLE_IF_STREAM_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_ENABLE_IF_STREAM_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <ndnboost/config.hpp>                // NDNBOOST_NO_SFINAE.
#include <ndnboost/utility/enable_if.hpp>                  
#include <ndnboost/iostreams/traits_fwd.hpp>  // is_std_io.

#if !defined(NDNBOOST_NO_SFINAE) && \
    !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x592))
# define NDNBOOST_IOSTREAMS_ENABLE_IF_STREAM(T) \
    , typename ndnboost::enable_if< ndnboost::iostreams::is_std_io<T> >::type* = 0 \
    /**/
# define NDNBOOST_IOSTREAMS_DISABLE_IF_STREAM(T) \
    , typename ndnboost::disable_if< ndnboost::iostreams::is_std_io<T> >::type* = 0 \
    /**/
#else 
# define NDNBOOST_IOSTREAMS_ENABLE_IF_STREAM(T)
# define NDNBOOST_IOSTREAMS_DISABLE_IF_STREAM(T)
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_ENABLE_IF_STREAM_HPP_INCLUDED
