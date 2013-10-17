// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// To do: handle bidirection streams and output-seekable components.

#ifndef NDNBOOST_IOSTREAMS_SKIP_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_SKIP_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/iostreams/char_traits.hpp>
#include <ndnboost/iostreams/detail/ios.hpp>  // failure.
#include <ndnboost/iostreams/operations.hpp>
#include <ndnboost/iostreams/seek.hpp>
#include <ndnboost/iostreams/traits.hpp>
#include <ndnboost/mpl/and.hpp>
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/mpl/or.hpp>
#include <ndnboost/throw_exception.hpp>
#include <ndnboost/type_traits/is_convertible.hpp>

namespace ndnboost { namespace iostreams {

namespace detail {

template<typename Device>
void skip(Device& dev, stream_offset off, mpl::true_)
{ iostreams::seek(dev, off, NDNBOOST_IOS::cur); }

template<typename Device>
void skip(Device& dev, stream_offset off, mpl::false_)
{   // gcc 2.95 needs namespace qualification for char_traits.
    typedef typename char_type_of<Device>::type  char_type;
    typedef iostreams::char_traits<char_type>    traits_type;
    for (stream_offset z = 0; z < off; ) {
        typename traits_type::int_type c;
        if (traits_type::is_eof(c = iostreams::get(dev)))
            ndnboost::throw_exception(NDNBOOST_IOSTREAMS_FAILURE("bad skip offset"));
        if (!traits_type::would_block(c))
            ++z;
    }
}

template<typename Filter, typename Device>
void skip( Filter& flt, Device& dev, stream_offset off,
           NDNBOOST_IOS::openmode which, mpl::true_ )
{ ndnboost::iostreams::seek(flt, dev, off, NDNBOOST_IOS::cur, which); }

template<typename Filter, typename Device>
void skip( Filter& flt, Device& dev, stream_offset off,
           NDNBOOST_IOS::openmode, mpl::false_ )
{ 
    typedef typename char_type_of<Device>::type char_type;
    char_type c;
    for (stream_offset z = 0; z < off; ) {
        std::streamsize amt;
        if ((amt = iostreams::read(flt, dev, &c, 1)) == -1)
            ndnboost::throw_exception(NDNBOOST_IOSTREAMS_FAILURE("bad skip offset"));
        if (amt == 1)
            ++z;
    }
}

} // End namespace detail.

template<typename Device>
void skip(Device& dev, stream_offset off)
{ 
    typedef typename mode_of<Device>::type     mode;
    typedef mpl::or_<
        is_convertible<mode, input_seekable>,
        is_convertible<mode, output_seekable>
    >                                          can_seek;
    NDNBOOST_STATIC_ASSERT(
        (can_seek::value || is_convertible<mode, input>::value)
    );
    detail::skip(dev, off, can_seek());
}

template<typename Filter, typename Device>
void skip( Filter& flt, Device& dev, stream_offset off, 
           NDNBOOST_IOS::openmode which = NDNBOOST_IOS::in | NDNBOOST_IOS::out )
{ 
    typedef typename mode_of<Filter>::type                 filter_mode;
    typedef typename mode_of<Device>::type                 device_mode;
    typedef mpl::or_<
        mpl::and_<
            is_convertible<filter_mode, input_seekable>,
            is_convertible<device_mode, input_seekable>
        >,
        mpl::and_<
            is_convertible<filter_mode, output_seekable>,
            is_convertible<device_mode, output_seekable>
        >
    >                                                      can_seek;
    NDNBOOST_STATIC_ASSERT(
        ( can_seek::value || 
          (is_convertible<filter_mode, input>::value &&
          is_convertible<device_mode, input>::value) )
    );
    detail::skip(flt, dev, off, which, can_seek());
}

} } // End namespaces iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_SKIP_HPP_INCLUDED //------------------------//
