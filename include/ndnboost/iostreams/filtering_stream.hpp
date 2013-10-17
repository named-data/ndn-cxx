// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2004-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_FILTER_STREAM_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_FILTER_STREAM_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <memory>                                     // allocator.
#include <ndnboost/iostreams/detail/access_control.hpp>
#include <ndnboost/iostreams/detail/char_traits.hpp>
#include <ndnboost/iostreams/detail/iostream.hpp>        // standard streams.
#include <ndnboost/iostreams/detail/push.hpp>
#include <ndnboost/iostreams/detail/select.hpp>
#include <ndnboost/iostreams/detail/streambuf.hpp>       // pubsync.
#include <ndnboost/iostreams/filtering_streambuf.hpp>
#include <ndnboost/mpl/and.hpp>
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/static_assert.hpp>
#include <ndnboost/type_traits/is_convertible.hpp>

// Must come last.
#include <ndnboost/iostreams/detail/config/disable_warnings.hpp>  // MSVC.

namespace ndnboost { namespace iostreams {

//--------------Definition of filtered_istream--------------------------------//

namespace detail {

template<typename Mode, typename Ch, typename Tr>
struct filtering_stream_traits {
    typedef typename 
            iostreams::select<  // Disambiguation for Tru64  
                mpl::and_< 
                    is_convertible<Mode, input>, 
                    is_convertible<Mode, output> 
                >,          
                NDNBOOST_IOSTREAMS_BASIC_IOSTREAM(Ch, Tr),
                is_convertible<Mode, input>, 
                NDNBOOST_IOSTREAMS_BASIC_ISTREAM(Ch, Tr),
                else_,        
                NDNBOOST_IOSTREAMS_BASIC_OSTREAM(Ch, Tr)
            >::type stream_type;
    typedef typename
            iostreams::select< // Dismbiguation required for Tru64.
                mpl::and_<
                    is_convertible<Mode, input>,
                    is_convertible<Mode, output>
                >,
                iostream_tag,
                is_convertible<Mode, input>,
                istream_tag,
                else_,
                ostream_tag
            >::type stream_tag;
};

template<typename Chain, typename Access>
class filtering_stream_base 
    : public access_control<
                 ndnboost::iostreams::detail::chain_client<Chain>,
                 Access
             >,
      public filtering_stream_traits<
                 typename Chain::mode, 
                 typename Chain::char_type, 
                 typename Chain::traits_type
             >::stream_type
{
public:
    typedef Chain                                         chain_type;
    typedef access_control<
                 ndnboost::iostreams::detail::chain_client<Chain>,
                 Access
             >                                            client_type;
protected:
    typedef typename 
            filtering_stream_traits<
                 typename Chain::mode, 
                 typename Chain::char_type, 
                 typename Chain::traits_type
            >::stream_type                                stream_type;
    filtering_stream_base() : stream_type(0) { this->set_chain(&chain_); }
private:
    void notify() { this->rdbuf(chain_.empty() ? 0 : &chain_.front()); }
    Chain chain_;
};

} // End namespace detail.

//
// Macro: NDNBOOST_IOSTREAMS_DEFINE_FILTER_STREAM(name_, chain_type_, default_char_)
// Description: Defines a template derived from std::basic_streambuf which uses
//      a chain to perform i/o. The template has the following parameters:
//      Mode - the i/o mode.
//      Ch - The character type.
//      Tr - The character traits type.
//      Alloc - The allocator type.
//      Access - Indicates accessibility of the chain interface; must be either
//          public_ or protected_; defaults to public_.
// Macro parameters:
//      name_ - The name of the template to be defined.
//      chain_type_ - The name of the chain template.
//      default_char_ - The default value for the char template parameter.
//
#define NDNBOOST_IOSTREAMS_DEFINE_FILTER_STREAM(name_, chain_type_, default_char_) \
    template< typename Mode, \
              typename Ch = default_char_, \
              typename Tr = NDNBOOST_IOSTREAMS_CHAR_TRAITS(Ch), \
              typename Alloc = std::allocator<Ch>, \
              typename Access = public_ > \
    class name_ \
        : public ndnboost::iostreams::detail::filtering_stream_base< \
                     chain_type_<Mode, Ch, Tr, Alloc>, Access \
                 > \
    { \
    public: \
        typedef Ch                                char_type; \
        struct category \
            : Mode, \
              closable_tag, \
              detail::filtering_stream_traits<Mode, Ch, Tr>::stream_tag \
            { }; \
        NDNBOOST_IOSTREAMS_STREAMBUF_TYPEDEFS(Tr) \
        typedef Mode                              mode; \
        typedef chain_type_<Mode, Ch, Tr, Alloc>  chain_type; \
        name_() { } \
        NDNBOOST_IOSTREAMS_DEFINE_PUSH_CONSTRUCTOR(name_, mode, Ch, push_impl) \
        ~name_() { \
            if (this->is_complete()) \
                 this->rdbuf()->NDNBOOST_IOSTREAMS_PUBSYNC(); \
        } \
    private: \
        typedef access_control< \
                    ndnboost::iostreams::detail::chain_client< \
                        chain_type_<Mode, Ch, Tr, Alloc> \
                    >, \
                    Access \
                > client_type; \
        template<typename T> \
        void push_impl(const T& t NDNBOOST_IOSTREAMS_PUSH_PARAMS()) \
        { client_type::push(t NDNBOOST_IOSTREAMS_PUSH_ARGS()); } \
    }; \
    /**/    
NDNBOOST_IOSTREAMS_DEFINE_FILTER_STREAM(filtering_stream, ndnboost::iostreams::chain, char)
NDNBOOST_IOSTREAMS_DEFINE_FILTER_STREAM(wfiltering_stream, ndnboost::iostreams::chain, wchar_t)

typedef filtering_stream<input>    filtering_istream;
typedef filtering_stream<output>   filtering_ostream;
typedef wfiltering_stream<input>   filtering_wistream;
typedef wfiltering_stream<output>  filtering_wostream;

//----------------------------------------------------------------------------//

} } // End namespace iostreams, boost

#include <ndnboost/iostreams/detail/config/enable_warnings.hpp> // MSVC

#endif // #ifndef NDNBOOST_IOSTREAMS_FILTER_STREAM_HPP_INCLUDED
