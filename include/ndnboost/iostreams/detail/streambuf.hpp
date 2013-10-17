// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_STREAMBUF_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_STREAMBUF_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              
                 
#include <ndnboost/iostreams/detail/config/wide_streams.hpp>
#ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES
# include <streambuf>
#else 
# include <streambuf.h>
#endif 

#ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES
# define NDNBOOST_IOSTREAMS_BASIC_STREAMBUF(ch, tr) std::basic_streambuf< ch, tr >
# define NDNBOOST_IOSTREAMS_PUBSYNC pubsync
# define NDNBOOST_IOSTREAMS_PUBSEEKOFF pubseekoff
# define NDNBOOST_IOSTREAMS_PUBSEEKPOS pubseekpos
#else
# define NDNBOOST_IOSTREAMS_BASIC_STREAMBUF(ch, tr) std::streambuf
# define NDNBOOST_IOSTREAMS_PUBSYNC sync
# define NDNBOOST_IOSTREAMS_PUBSEEKOFF seekoff
# define NDNBOOST_IOSTREAMS_PUBSEEKPOS seekpos
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_STREAMBUF_HPP_INCLUDED
