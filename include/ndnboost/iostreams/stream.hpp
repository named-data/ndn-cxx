// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_STREAM_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_STREAM_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/iostreams/constants.hpp>
#include <ndnboost/iostreams/detail/char_traits.hpp>
#include <ndnboost/iostreams/detail/config/overload_resolution.hpp>
#include <ndnboost/iostreams/detail/forward.hpp>
#include <ndnboost/iostreams/detail/iostream.hpp>  // standard streams.
#include <ndnboost/iostreams/detail/select.hpp>
#include <ndnboost/iostreams/stream_buffer.hpp>
#include <ndnboost/mpl/and.hpp>
#include <ndnboost/type_traits/is_convertible.hpp>
#include <ndnboost/utility/base_from_member.hpp>

namespace ndnboost { namespace iostreams { namespace detail {

template<typename Device, typename Tr>
struct stream_traits {
    typedef typename char_type_of<Device>::type                char_type;
    typedef Tr                                                 traits_type;
    typedef typename category_of<Device>::type                 mode;
    typedef typename
            iostreams::select< // Disambiguation required for Tru64.
                mpl::and_<
                    is_convertible<mode, input>,
                    is_convertible<mode, output>
                >,
                NDNBOOST_IOSTREAMS_BASIC_IOSTREAM(char_type, traits_type),
                is_convertible<mode, input>,
                NDNBOOST_IOSTREAMS_BASIC_ISTREAM(char_type, traits_type),
                else_,
                NDNBOOST_IOSTREAMS_BASIC_OSTREAM(char_type, traits_type)
            >::type stream_type;
    typedef typename
            iostreams::select< // Disambiguation required for Tru64.
                mpl::and_<
                    is_convertible<mode, input>,
                    is_convertible<mode, output>
                >,
                iostream_tag,
                is_convertible<mode, input>,
                istream_tag,
                else_,
                ostream_tag
            >::type stream_tag;
};

// By encapsulating initialization in a base, we can define the macro
// NDNBOOST_IOSTREAMS_DEFINE_FORWARDING_FUNCTIONS to generate constructors
// without base member initializer lists.
template< typename Device,
          typename Tr =
              NDNBOOST_IOSTREAMS_CHAR_TRAITS(
                  NDNBOOST_DEDUCED_TYPENAME char_type_of<Device>::type
              ),
          typename Alloc =
              std::allocator<
                  NDNBOOST_DEDUCED_TYPENAME char_type_of<Device>::type
              >,
          typename Base = // VC6 Workaround.
              NDNBOOST_DEDUCED_TYPENAME
              detail::stream_traits<Device, Tr>::stream_type >
class stream_base
    : protected base_from_member< stream_buffer<Device, Tr, Alloc> >,
      public Base
{
private:
    typedef base_from_member< stream_buffer<Device, Tr, Alloc> >  pbase_type;
    typedef typename stream_traits<Device, Tr>::stream_type       stream_type;
protected:
    using pbase_type::member; // Avoid warning about 'this' in initializer list.
public:
    stream_base() : pbase_type(), stream_type(&member) { }
};

} } } // End namespaces detail, iostreams, boost.

#ifdef NDNBOOST_IOSTREAMS_BROKEN_OVERLOAD_RESOLUTION
# include <ndnboost/iostreams/detail/broken_overload_resolution/stream.hpp>
#else

namespace ndnboost { namespace iostreams {

//
// Template name: stream.
// Description: A iostream which reads from and writes to an instance of a
//      designated device type.
// Template parameters:
//      Device - A device type.
//      Alloc - The allocator type.
//
template< typename Device,
          typename Tr =
              NDNBOOST_IOSTREAMS_CHAR_TRAITS(
                  NDNBOOST_DEDUCED_TYPENAME char_type_of<Device>::type
              ),
          typename Alloc =
              std::allocator<
                  NDNBOOST_DEDUCED_TYPENAME char_type_of<Device>::type
              > >
struct stream : detail::stream_base<Device, Tr, Alloc> {
public:
    typedef typename char_type_of<Device>::type  char_type;
    struct category 
        : mode_of<Device>::type,
          closable_tag,
          detail::stream_traits<Device, Tr>::stream_tag
        { };
    NDNBOOST_IOSTREAMS_STREAMBUF_TYPEDEFS(Tr)
private:
    typedef typename
            detail::stream_traits<
                Device, Tr
            >::stream_type                       stream_type;
public:
    stream() { }
    NDNBOOST_IOSTREAMS_FORWARD( stream, open_impl, Device,
                             NDNBOOST_IOSTREAMS_PUSH_PARAMS,
                             NDNBOOST_IOSTREAMS_PUSH_ARGS )
    bool is_open() const { return this->member.is_open(); }
    void close() { this->member.close(); }
    bool auto_close() const { return this->member.auto_close(); }
    void set_auto_close(bool close) { this->member.set_auto_close(close); }
    bool strict_sync() { return this->member.strict_sync(); }
    Device& operator*() { return *this->member; }
    Device* operator->() { return &*this->member; }
    Device* component() { return this->member.component(); }
private:
    void open_impl(const Device& dev NDNBOOST_IOSTREAMS_PUSH_PARAMS()) // For forwarding.
    { 
        this->clear(); 
        this->member.open(dev NDNBOOST_IOSTREAMS_PUSH_ARGS()); 
    }
};

} } // End namespaces iostreams, boost.

#endif // #ifdef NDNBOOST_IOSTREAMS_BROKEN_OVERLOAD_RESOLUTION

#endif // #ifndef NDNBOOST_IOSTREAMS_stream_HPP_INCLUDED
