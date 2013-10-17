// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_STREAM_BUFFER_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_STREAM_BUFFER_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <memory>            // allocator.
#include <ndnboost/config.hpp>  // NDNBOOST_DEDUCED_TYPENAME.
#include <ndnboost/iostreams/detail/char_traits.hpp>
#include <ndnboost/iostreams/detail/config/overload_resolution.hpp>
#include <ndnboost/iostreams/detail/forward.hpp>
#include <ndnboost/iostreams/detail/ios.hpp>  // failure, streamsize.
#include <ndnboost/iostreams/detail/streambuf/direct_streambuf.hpp>
#include <ndnboost/iostreams/detail/streambuf/indirect_streambuf.hpp>
#include <ndnboost/iostreams/traits.hpp>
#include <ndnboost/static_assert.hpp>
#include <ndnboost/throw_exception.hpp>
#include <ndnboost/type_traits/is_convertible.hpp>

// Must come last.
#include <ndnboost/iostreams/detail/config/disable_warnings.hpp>  // MSVC.

namespace ndnboost { namespace iostreams { namespace detail {

template<typename T, typename Tr, typename Alloc, typename Mode>
struct stream_buffer_traits {
    typedef typename
            mpl::if_<
                is_convertible<
                    NDNBOOST_DEDUCED_TYPENAME category_of<T>::type,
                    direct_tag
                >,
                direct_streambuf<T, Tr>,
                indirect_streambuf<T, Tr, Alloc, Mode>
            >::type type;
};

} } } // End namespaces detail, iostreams, boost

#ifdef NDNBOOST_IOSTREAMS_BROKEN_OVERLOAD_RESOLUTION
# include <ndnboost/iostreams/detail/broken_overload_resolution/stream_buffer.hpp>
#else

namespace ndnboost { namespace iostreams {

template< typename T,
          typename Tr =
              NDNBOOST_IOSTREAMS_CHAR_TRAITS(
                  NDNBOOST_DEDUCED_TYPENAME char_type_of<T>::type
              ),
          typename Alloc =
              std::allocator<
                  NDNBOOST_DEDUCED_TYPENAME char_type_of<T>::type
              >,
          typename Mode = NDNBOOST_DEDUCED_TYPENAME mode_of<T>::type >
class stream_buffer
    : public detail::stream_buffer_traits<T, Tr, Alloc, Mode>::type
{
private:
    NDNBOOST_STATIC_ASSERT((
        is_convertible<
            NDNBOOST_DEDUCED_TYPENAME iostreams::category_of<T>::type, Mode
        >::value
    ));
    typedef typename
            detail::stream_buffer_traits<
                T, Tr, Alloc, Mode
            >::type                           base_type;
public:
    typedef typename char_type_of<T>::type    char_type;
    struct category 
        : Mode,
          closable_tag,
          streambuf_tag
        { };
    NDNBOOST_IOSTREAMS_STREAMBUF_TYPEDEFS(Tr)
public:
    stream_buffer() { }
    ~stream_buffer()
    { 
        try { 
            if (this->is_open() && this->auto_close()) 
                this->close(); 
        } catch (...) { } 
    }
    NDNBOOST_IOSTREAMS_FORWARD( stream_buffer, open_impl, T,
                             NDNBOOST_IOSTREAMS_PUSH_PARAMS,
                             NDNBOOST_IOSTREAMS_PUSH_ARGS )
    T& operator*() { return *this->component(); }
    T* operator->() { return this->component(); }
private:
    void open_impl(const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS())
        {   // Used for forwarding.
            if (this->is_open())
                ndnboost::throw_exception(
                    NDNBOOST_IOSTREAMS_FAILURE("already open")
                );
            base_type::open(t NDNBOOST_IOSTREAMS_PUSH_ARGS());
        }
};

} } // End namespaces iostreams, boost.

#endif // #ifdef NDNBOOST_IOSTREAMS_BROKEN_OVERLOAD_RESOLUTION

#include <ndnboost/iostreams/detail/config/enable_warnings.hpp>  // MSVC.

#endif // #ifndef NDNBOOST_IOSTREAMS_STREAM_BUFFER_HPP_INCLUDED
