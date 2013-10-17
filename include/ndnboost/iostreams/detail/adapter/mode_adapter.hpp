// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_MODE_ADAPTER_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_MODE_ADAPTER_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

// Contains the definition of the class template mode_adapter, which allows
// a filter or device to function as if it has a different i/o mode than that
// deduced by the metafunction mode_of.

#include <ndnboost/config.hpp>                // NDNBOOST_MSVC.
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/detail/ios.hpp>  // openmode, seekdir, int types. 
#include <ndnboost/iostreams/traits.hpp>
#include <ndnboost/iostreams/operations.hpp> 
#include <ndnboost/mpl/if.hpp> 

namespace ndnboost { namespace iostreams { namespace detail {

template<typename Mode, typename T>
class mode_adapter {
private:
    struct empty_base { };
public:
    typedef typename wrapped_type<T>::type  component_type;
    typedef typename char_type_of<T>::type  char_type;
    struct category 
        : Mode, 
          device_tag,
          mpl::if_<is_filter<T>, filter_tag, device_tag>,
          mpl::if_<is_filter<T>, multichar_tag, empty_base>,
          #if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
              closable_tag, // VC6 can't see member close()!
          #endif
          localizable_tag
        { };
    explicit mode_adapter(const component_type& t) : t_(t) { }

        // Device member functions.

    std::streamsize read(char_type* s, std::streamsize n);
    std::streamsize write(const char_type* s, std::streamsize n);
    std::streampos seek( stream_offset off, NDNBOOST_IOS::seekdir way,
                         NDNBOOST_IOS::openmode which = 
                             NDNBOOST_IOS::in | NDNBOOST_IOS::out );
#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
    void close();
    void close(NDNBOOST_IOS::openmode which);
#endif

        // Filter member functions.

    template<typename Source>
    std::streamsize read(Source& src, char_type* s, std::streamsize n)
    { return iostreams::read(t_, src, s, n); }

    template<typename Sink>
    std::streamsize write(Sink& snk, const char_type* s, std::streamsize n)
    { return iostreams::write(t_, snk, s, n); }

    template<typename Device>
    std::streampos seek(Device& dev, stream_offset off, NDNBOOST_IOS::seekdir way)
    { return iostreams::seek(t_, dev, off, way); }

    template<typename Device>
    std::streampos seek( Device& dev, stream_offset off, 
                         NDNBOOST_IOS::seekdir way, NDNBOOST_IOS::openmode which  )
    { return iostreams::seek(t_, dev, off, way, which); }

    template<typename Device>
    void close(Device& dev)
    { detail::close_all(t_, dev); }

    template<typename Device>
    void close(Device& dev, NDNBOOST_IOS::openmode which)
    { iostreams::close(t_, dev, which); }

    template<typename Locale>
    void imbue(const Locale& loc)
    { iostreams::imbue(t_, loc); }
private:
    component_type t_;
};
                    
//------------------Implementation of mode_adapter----------------------------//

template<typename Mode, typename T>
std::streamsize mode_adapter<Mode, T>::read
    (char_type* s, std::streamsize n)
{ return ndnboost::iostreams::read(t_, s, n); }

template<typename Mode, typename T>
std::streamsize mode_adapter<Mode, T>::write
    (const char_type* s, std::streamsize n)
{ return ndnboost::iostreams::write(t_, s, n); }

template<typename Mode, typename T>
std::streampos mode_adapter<Mode, T>::seek
    (stream_offset off, NDNBOOST_IOS::seekdir way, NDNBOOST_IOS::openmode which)
{ return ndnboost::iostreams::seek(t_, off, way, which); }

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
    template<typename Mode, typename T>
    void mode_adapter<Mode, T>::close() 
    { detail::close_all(t_); }

    template<typename Mode, typename T>
    void mode_adapter<Mode, T>::close(NDNBOOST_IOS::openmode which) 
    { iostreams::close(t_, which); }
#endif

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_MODE_ADAPTER_HPP_INCLUDED //-----//
