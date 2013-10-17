// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_FILE_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_FILE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/iostreams/detail/config/wide_streams.hpp>
#ifndef NDNBOOST_IOSTREAMS_NO_LOCALE
# include <locale>
#endif
#include <string>                               // pathnames, char_traits.
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/detail/ios.hpp>       // openmode, seekdir, int types.
#include <ndnboost/iostreams/detail/fstream.hpp>
#include <ndnboost/iostreams/operations.hpp>       // seek.
#include <ndnboost/shared_ptr.hpp>      

// Must come last.
#include <ndnboost/iostreams/detail/config/disable_warnings.hpp>  // MSVC.

namespace ndnboost { namespace iostreams {

template<typename Ch>
class basic_file {
public:
    typedef Ch char_type;
    struct category
        : public seekable_device_tag,
          public closable_tag,
          public localizable_tag,
          public flushable_tag
        { };
    basic_file( const std::string& path,
                NDNBOOST_IOS::openmode mode =
                    NDNBOOST_IOS::in | NDNBOOST_IOS::out,
                NDNBOOST_IOS::openmode base_mode =
                    NDNBOOST_IOS::in | NDNBOOST_IOS::out );
    std::streamsize read(char_type* s, std::streamsize n);
    bool putback(char_type c);
    std::streamsize write(const char_type* s, std::streamsize n);
    std::streampos seek( stream_offset off, NDNBOOST_IOS::seekdir way, 
                         NDNBOOST_IOS::openmode which = 
                             NDNBOOST_IOS::in | NDNBOOST_IOS::out );
    void open( const std::string& path,
               NDNBOOST_IOS::openmode mode =
                   NDNBOOST_IOS::in | NDNBOOST_IOS::out,
               NDNBOOST_IOS::openmode base_mode =
                   NDNBOOST_IOS::in | NDNBOOST_IOS::out );
    bool is_open() const;
    void close();
    bool flush();
#ifndef NDNBOOST_IOSTREAMS_NO_LOCALE
    void imbue(const std::locale& loc) { pimpl_->file_.pubimbue(loc);  }
#endif
private:
    struct impl {
        impl(const std::string& path, NDNBOOST_IOS::openmode mode)
            { file_.open(path.c_str(), mode); }
        ~impl() { if (file_.is_open()) file_.close(); }
        NDNBOOST_IOSTREAMS_BASIC_FILEBUF(Ch) file_;
    };
    shared_ptr<impl> pimpl_;
};

typedef basic_file<char>     file;
typedef basic_file<wchar_t>  wfile;

template<typename Ch>
struct basic_file_source : private basic_file<Ch> {
    typedef Ch char_type;
    struct category
        : input_seekable,
          device_tag,
          closable_tag
        { };
    using basic_file<Ch>::read;
    using basic_file<Ch>::putback;
    using basic_file<Ch>::seek;
    using basic_file<Ch>::is_open;
    using basic_file<Ch>::close;
    basic_file_source( const std::string& path,
                       NDNBOOST_IOS::openmode mode = 
                           NDNBOOST_IOS::in )
        : basic_file<Ch>(path, mode & ~NDNBOOST_IOS::out, NDNBOOST_IOS::in)
        { }
    void open( const std::string& path,
               NDNBOOST_IOS::openmode mode = NDNBOOST_IOS::in )
    {
        basic_file<Ch>::open(path, mode & ~NDNBOOST_IOS::out, NDNBOOST_IOS::in);
    }
};

typedef basic_file_source<char>     file_source;
typedef basic_file_source<wchar_t>  wfile_source;

template<typename Ch>
struct basic_file_sink : private basic_file<Ch> {
    typedef Ch char_type;
    struct category
        : output_seekable,
          device_tag,
          closable_tag,
          flushable_tag
        { };
    using basic_file<Ch>::write;
    using basic_file<Ch>::seek;
    using basic_file<Ch>::is_open;
    using basic_file<Ch>::close;
    using basic_file<Ch>::flush;
    basic_file_sink( const std::string& path,
                     NDNBOOST_IOS::openmode mode = NDNBOOST_IOS::out )
        : basic_file<Ch>(path, mode & ~NDNBOOST_IOS::in, NDNBOOST_IOS::out)
        { }
    void open( const std::string& path,
               NDNBOOST_IOS::openmode mode = NDNBOOST_IOS::out )
    {
        basic_file<Ch>::open(path, mode & ~NDNBOOST_IOS::in, NDNBOOST_IOS::out);
    }
};

typedef basic_file_sink<char>     file_sink;
typedef basic_file_sink<wchar_t>  wfile_sink;
                                 
//------------------Implementation of basic_file------------------------------//

template<typename Ch>
basic_file<Ch>::basic_file
    ( const std::string& path, NDNBOOST_IOS::openmode mode, 
      NDNBOOST_IOS::openmode base_mode )
{ 
    open(path, mode, base_mode);
}

template<typename Ch>
inline std::streamsize basic_file<Ch>::read
    (char_type* s, std::streamsize n)
{ 
    std::streamsize result = pimpl_->file_.sgetn(s, n); 
    return result != 0 ? result : -1;
}

template<typename Ch>
inline bool basic_file<Ch>::putback(char_type c)
{ 
    return !!pimpl_->file_.sputbackc(c); 
}

template<typename Ch>
inline std::streamsize basic_file<Ch>::write
    (const char_type* s, std::streamsize n)
{ return pimpl_->file_.sputn(s, n); }

template<typename Ch>
std::streampos basic_file<Ch>::seek
    ( stream_offset off, NDNBOOST_IOS::seekdir way, 
      NDNBOOST_IOS::openmode )
{ return iostreams::seek(pimpl_->file_, off, way); }

template<typename Ch>
void basic_file<Ch>::open
    ( const std::string& path, NDNBOOST_IOS::openmode mode, 
      NDNBOOST_IOS::openmode base_mode )
{ 
    pimpl_.reset(new impl(path, mode | base_mode));
}

template<typename Ch>
bool basic_file<Ch>::is_open() const { return pimpl_->file_.is_open(); }

template<typename Ch>
void basic_file<Ch>::close() { pimpl_->file_.close(); }

template<typename Ch>
bool basic_file<Ch>::flush()
{ return pimpl_->file_.NDNBOOST_IOSTREAMS_PUBSYNC() == 0; }

//----------------------------------------------------------------------------//

} } // End namespaces iostreams, boost.

#include <ndnboost/iostreams/detail/config/enable_warnings.hpp> // MSVC

#endif // #ifndef NDNBOOST_IOSTREAMS_FILE_HPP_INCLUDED
