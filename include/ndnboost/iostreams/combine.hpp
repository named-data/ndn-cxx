// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// To do: add support for random-access.

#ifndef NDNBOOST_IOSTREAMS_COMBINE_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_COMBINE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <ndnboost/config.hpp> // NO_STD_LOCALE, DEDUCED_TYPENAME.
#ifndef NDNBOOST_NO_STD_LOCALE
# include <locale>
#endif
#include <ndnboost/iostreams/detail/ios.hpp>   
#include <ndnboost/iostreams/detail/wrap_unwrap.hpp>       
#include <ndnboost/iostreams/traits.hpp>         
#include <ndnboost/iostreams/operations.hpp>        
#include <ndnboost/mpl/if.hpp>    
#include <ndnboost/static_assert.hpp>  
#include <ndnboost/type_traits/is_convertible.hpp>
#include <ndnboost/type_traits/is_same.hpp> 

// Must come last.
#include <ndnboost/iostreams/detail/config/disable_warnings.hpp>

namespace ndnboost { namespace iostreams {

namespace detail {

//
// Template name: combined_device.
// Description: Model of Device defined in terms of a Source/Sink pair.
// Template parameters:
//      Source - A model of Source, with the same char_type and traits_type
//          as Sink.
//      Sink - A model of Sink, with the same char_type and traits_type
//          as Source.
//
template<typename Source, typename Sink>
class combined_device {
private:
    typedef typename category_of<Source>::type  in_category;
    typedef typename category_of<Sink>::type    out_category;
    typedef typename char_type_of<Sink>::type   sink_char_type;
public:
    typedef typename char_type_of<Source>::type char_type;
    struct category
        : bidirectional, 
          device_tag, 
          closable_tag, 
          localizable_tag
        { };
    NDNBOOST_STATIC_ASSERT(is_device<Source>::value);
    NDNBOOST_STATIC_ASSERT(is_device<Sink>::value);
    NDNBOOST_STATIC_ASSERT((is_convertible<in_category, input>::value));
    NDNBOOST_STATIC_ASSERT((is_convertible<out_category, output>::value));
    NDNBOOST_STATIC_ASSERT((is_same<char_type, sink_char_type>::value));
    combined_device(const Source& src, const Sink& snk);
    std::streamsize read(char_type* s, std::streamsize n);
    std::streamsize write(const char_type* s, std::streamsize n);
    void close(NDNBOOST_IOS::openmode);
    #ifndef NDNBOOST_NO_STD_LOCALE
        void imbue(const std::locale& loc);
    #endif
private:
    Source  src_;
    Sink    sink_;
};

//
// Template name: combined_filter.
// Description: Model of Device defined in terms of a Source/Sink pair.
// Template parameters:
//      InputFilter - A model of InputFilter, with the same char_type as 
//          OutputFilter.
//      OutputFilter - A model of OutputFilter, with the same char_type as 
//          InputFilter.
//
template<typename InputFilter, typename OutputFilter>
class combined_filter {
private:
    typedef typename category_of<InputFilter>::type    in_category;
    typedef typename category_of<OutputFilter>::type   out_category;
    typedef typename char_type_of<OutputFilter>::type  output_char_type;
public:
    typedef typename char_type_of<InputFilter>::type   char_type;
    struct category 
        : multichar_bidirectional_filter_tag,
          closable_tag, 
          localizable_tag
        { };
    NDNBOOST_STATIC_ASSERT(is_filter<InputFilter>::value);
    NDNBOOST_STATIC_ASSERT(is_filter<OutputFilter>::value);
    NDNBOOST_STATIC_ASSERT((is_convertible<in_category, input>::value));
    NDNBOOST_STATIC_ASSERT((is_convertible<out_category, output>::value));
    NDNBOOST_STATIC_ASSERT((is_same<char_type, output_char_type>::value));
    combined_filter(const InputFilter& in, const OutputFilter& out);

    template<typename Source>
    std::streamsize read(Source& src, char_type* s, std::streamsize n)
    { return ndnboost::iostreams::read(in_, src, s, n); }

    template<typename Sink>
    std::streamsize write(Sink& snk, const char_type* s, std::streamsize n)
    { return ndnboost::iostreams::write(out_, snk, s, n); }

    template<typename Sink>
    void close(Sink& snk, NDNBOOST_IOS::openmode which)
    {
        if (which == NDNBOOST_IOS::in) {
            if (is_convertible<in_category, dual_use>::value) {
                iostreams::close(in_, snk, NDNBOOST_IOS::in);
            } else {
                detail::close_all(in_, snk);
            }
        }
        if (which == NDNBOOST_IOS::out) {
            if (is_convertible<out_category, dual_use>::value) {
                iostreams::close(out_, snk, NDNBOOST_IOS::out);
            } else {
                detail::close_all(out_, snk);
            }
        }
    }
    #ifndef NDNBOOST_NO_STD_LOCALE
        void imbue(const std::locale& loc);
    #endif
private:
    InputFilter   in_;
    OutputFilter  out_;
};

template<typename In, typename Out>
struct combination_traits 
    : mpl::if_<
          is_device<In>,
          combined_device<
              typename wrapped_type<In>::type,
              typename wrapped_type<Out>::type
          >,
          combined_filter<
              typename wrapped_type<In>::type,
              typename wrapped_type<Out>::type
          >
      >
    { };

} // End namespace detail.

template<typename In, typename Out>
struct combination : detail::combination_traits<In, Out>::type {
    typedef typename detail::combination_traits<In, Out>::type  base_type;
    typedef typename detail::wrapped_type<In>::type          in_type;
    typedef typename detail::wrapped_type<Out>::type         out_type;
    combination(const in_type& in, const out_type& out)
        : base_type(in, out) { }
};

namespace detail {

// Workaround for VC6 ETI bug.
template<typename In, typename Out>
struct combine_traits {
    typedef combination<
                NDNBOOST_DEDUCED_TYPENAME detail::unwrapped_type<In>::type, 
                NDNBOOST_DEDUCED_TYPENAME detail::unwrapped_type<Out>::type
            > type;
};

} // End namespace detail.

//
// Template name: combine.
// Description: Takes a Source/Sink pair or InputFilter/OutputFilter pair and
//      returns a Source or Filter which performs input using the first member
//      of the pair and output using the second member of the pair.
// Template parameters:
//      In - A model of Source or InputFilter, with the same char_type as Out.
//      Out - A model of Sink or OutputFilter, with the same char_type as In.
//
template<typename In, typename Out>
typename detail::combine_traits<In, Out>::type
combine(const In& in, const Out& out) 
{ 
    typedef typename detail::combine_traits<In, Out>::type return_type;
    return return_type(in, out); 
}

//----------------------------------------------------------------------------//

namespace detail {

//--------------Implementation of combined_device-----------------------------//

template<typename Source, typename Sink>
inline combined_device<Source, Sink>::combined_device
    (const Source& src, const Sink& snk)
    : src_(src), sink_(snk) { }

template<typename Source, typename Sink>
inline std::streamsize
combined_device<Source, Sink>::read(char_type* s, std::streamsize n)
{ return iostreams::read(src_, s, n); }

template<typename Source, typename Sink>
inline std::streamsize
combined_device<Source, Sink>::write(const char_type* s, std::streamsize n)
{ return iostreams::write(sink_, s, n); }

template<typename Source, typename Sink>
inline void
combined_device<Source, Sink>::close(NDNBOOST_IOS::openmode which)
{ 
    if (which == NDNBOOST_IOS::in)
        detail::close_all(src_); 
    if (which == NDNBOOST_IOS::out)
        detail::close_all(sink_); 
}

#ifndef NDNBOOST_NO_STD_LOCALE
    template<typename Source, typename Sink>
    void combined_device<Source, Sink>::imbue(const std::locale& loc)
    {
        iostreams::imbue(src_, loc);
        iostreams::imbue(sink_, loc);
    }
#endif

//--------------Implementation of filter_pair---------------------------------//

template<typename InputFilter, typename OutputFilter>
inline combined_filter<InputFilter, OutputFilter>::combined_filter
    (const InputFilter& in, const OutputFilter& out) : in_(in), out_(out)
    { }

#ifndef NDNBOOST_NO_STD_LOCALE
    template<typename InputFilter, typename OutputFilter>
    void combined_filter<InputFilter, OutputFilter>::imbue
        (const std::locale& loc)
    {
        iostreams::imbue(in_, loc);
        iostreams::imbue(out_, loc);
    }
#endif


} // End namespace detail.

} } // End namespaces iostreams, boost.

#include <ndnboost/iostreams/detail/config/enable_warnings.hpp>

#endif // #ifndef NDNBOOST_IOSTREAMS_COMBINE_HPP_INCLUDED
