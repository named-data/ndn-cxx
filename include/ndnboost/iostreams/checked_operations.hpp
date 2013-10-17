// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2005-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Contains implementations of get, read, put, write and seek which
// check a device's mode at runtime instead of compile time.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_CHECKED_OPERATIONS_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_CHECKED_OPERATIONS_HPP_INCLUDED

#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/detail/dispatch.hpp>
#include <ndnboost/iostreams/detail/error.hpp>
#include <ndnboost/iostreams/detail/config/unreachable_return.hpp>
#include <ndnboost/iostreams/get.hpp>
#include <ndnboost/iostreams/put.hpp>
#include <ndnboost/iostreams/read.hpp>
#include <ndnboost/iostreams/seek.hpp>
#include <ndnboost/iostreams/traits.hpp>
#include <ndnboost/iostreams/write.hpp>
#include <ndnboost/throw_exception.hpp>

// Must come last.
#include <ndnboost/iostreams/detail/config/disable_warnings.hpp>  // MSVC.

namespace ndnboost { namespace iostreams {

namespace detail {

template<typename T> 
struct read_write_if_impl;

template<typename T> 
struct seek_if_impl;

} // End namespace detail.

template<typename T>
typename int_type_of<T>::type get_if(T& t)
{ 
    typedef typename detail::dispatch<T, input, output>::type tag;
    return detail::read_write_if_impl<tag>::get(t);
}

template<typename T>
inline std::streamsize
read_if(T& t, typename char_type_of<T>::type* s, std::streamsize n)
{ 
    typedef typename detail::dispatch<T, input, output>::type tag;
    return detail::read_write_if_impl<tag>::read(t, s, n);
}

template<typename T>
bool put_if(T& t, typename char_type_of<T>::type c)
{ 
    typedef typename detail::dispatch<T, output, input>::type tag;
    return detail::read_write_if_impl<tag>::put(t, c);
}

template<typename T>
inline std::streamsize write_if
    (T& t, const typename char_type_of<T>::type* s, std::streamsize n)
{ 
    typedef typename detail::dispatch<T, output, input>::type tag;
    return detail::read_write_if_impl<tag>::write(t, s, n);
}

template<typename T>
inline std::streampos
seek_if( T& t, stream_offset off, NDNBOOST_IOS::seekdir way, 
         NDNBOOST_IOS::openmode which = NDNBOOST_IOS::in | NDNBOOST_IOS::out )
{ 
    using namespace detail;
    typedef typename dispatch<T, random_access, any_tag>::type tag;
    return seek_if_impl<tag>::seek(t, off, way, which);
}

namespace detail {

//------------------Specializations of read_write_if_impl---------------------//

template<>
struct read_write_if_impl<input> {
    template<typename T>
    static typename int_type_of<T>::type get(T& t)
    { return iostreams::get(t); }

    template<typename T>
    static std::streamsize
    read(T& t, typename char_type_of<T>::type* s, std::streamsize n)
    { return iostreams::read(t, s, n); }

    template<typename T>
    static bool put(T&, typename char_type_of<T>::type)
    { ndnboost::throw_exception(cant_write());
      NDNBOOST_IOSTREAMS_UNREACHABLE_RETURN(false) }

    template<typename T>
    static std::streamsize 
    write(T&, const typename char_type_of<T>::type*, std::streamsize)
    { ndnboost::throw_exception(cant_write());
      NDNBOOST_IOSTREAMS_UNREACHABLE_RETURN(0) }
};

template<>
struct read_write_if_impl<output> {
    template<typename T>
    static typename int_type_of<T>::type get(T&)
    { ndnboost::throw_exception(cant_read());
      NDNBOOST_IOSTREAMS_UNREACHABLE_RETURN(0) }

    template<typename T>
    static std::streamsize
    read(T&, typename char_type_of<T>::type*, std::streamsize)
    { ndnboost::throw_exception(cant_read());
      NDNBOOST_IOSTREAMS_UNREACHABLE_RETURN(0) }

    template<typename T>
    static bool put(T& t, typename char_type_of<T>::type c)
    { return iostreams::put(t, c); }

    template<typename T>
    static std::streamsize 
    write( T& t, const typename char_type_of<T>::type* s, 
           std::streamsize n )
    { return iostreams::write(t, s, n); }
};

//------------------Specializations of seek_if_impl---------------------------//

template<>
struct seek_if_impl<random_access> {
    template<typename T>
    static std::streampos 
    seek( T& t, stream_offset off, NDNBOOST_IOS::seekdir way, 
          NDNBOOST_IOS::openmode which )
    { return iostreams::seek(t, off, way, which); }
};

template<>
struct seek_if_impl<any_tag> {
    template<typename T>
    static std::streampos 
    seek(T&, stream_offset, NDNBOOST_IOS::seekdir, NDNBOOST_IOS::openmode)
    { ndnboost::throw_exception(cant_seek());
      NDNBOOST_IOSTREAMS_UNREACHABLE_RETURN(std::streampos()) }
};

} // End namespace detail.

} } // End namespaces iostreams, boost.

#include <ndnboost/iostreams/detail/config/enable_warnings.hpp>  // MSVC.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_CHECKED_OPERATIONS_HPP_INCLUDED
