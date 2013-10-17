// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2004-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_ARRAY_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_ARRAY_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/config.hpp>         // NDNBOOST_MSVC, make sure size_t is in std.
#include <ndnboost/detail/workaround.hpp>
#include <cstddef>                  // std::size_t.
#include <utility>                  // pair.
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/preprocessor/cat.hpp>
#include <ndnboost/static_assert.hpp>
#include <ndnboost/type_traits/is_convertible.hpp>
#include <ndnboost/type_traits/is_same.hpp>

namespace ndnboost { namespace iostreams {

namespace detail {

template<typename Mode, typename Ch>
class array_adapter {
public:
    typedef Ch                                 char_type;
    typedef std::pair<char_type*, char_type*>  pair_type;
    struct category
        : public Mode,
          public device_tag,
          public direct_tag
        { };
    array_adapter(char_type* begin, char_type* end);
    array_adapter(char_type* begin, std::size_t length);
    array_adapter(const char_type* begin, const char_type* end);
    array_adapter(const char_type* begin, std::size_t length);
#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
    template<int N>
    array_adapter(char_type (&ar)[N])
        : begin_(ar), end_(ar + N) 
        { }
#endif
    pair_type input_sequence();
    pair_type output_sequence();
private:
    char_type* begin_;
    char_type* end_;
};

} // End namespace detail.

// Local macros, #undef'd below.
#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300)
# define NDNBOOST_IOSTREAMS_ARRAY_CTOR(name, ch) \
    template<int N> \
    NDNBOOST_PP_CAT(basic_, name)(ch (&ar)[N]) \
        : base_type(ar) { } \
    /**/
#else
# define NDNBOOST_IOSTREAMS_ARRAY_CTOR(name, ch)
#endif
#define NDNBOOST_IOSTREAMS_ARRAY(name, mode) \
    template<typename Ch> \
    struct NDNBOOST_PP_CAT(basic_, name) : detail::array_adapter<mode, Ch> { \
    private: \
        typedef detail::array_adapter<mode, Ch>  base_type; \
    public: \
        typedef typename base_type::char_type    char_type; \
        typedef typename base_type::category     category; \
        NDNBOOST_PP_CAT(basic_, name)(char_type* begin, char_type* end) \
            : base_type(begin, end) { } \
        NDNBOOST_PP_CAT(basic_, name)(char_type* begin, std::size_t length) \
            : base_type(begin, length) { } \
        NDNBOOST_PP_CAT(basic_, name)(const char_type* begin, const char_type* end) \
            : base_type(begin, end) { } \
        NDNBOOST_PP_CAT(basic_, name)(const char_type* begin, std::size_t length) \
            : base_type(begin, length) { } \
        NDNBOOST_IOSTREAMS_ARRAY_CTOR(name, Ch) \
    }; \
    typedef NDNBOOST_PP_CAT(basic_, name)<char>     name; \
    typedef NDNBOOST_PP_CAT(basic_, name)<wchar_t>  NDNBOOST_PP_CAT(w, name); \
    /**/
NDNBOOST_IOSTREAMS_ARRAY(array_source, input_seekable)
NDNBOOST_IOSTREAMS_ARRAY(array_sink, output_seekable)
NDNBOOST_IOSTREAMS_ARRAY(array, seekable)
#undef NDNBOOST_IOSTREAMS_ARRAY_CTOR
#undef NDNBOOST_IOSTREAMS_ARRAY


//------------------Implementation of array_adapter---------------------------//

namespace detail {

template<typename Mode, typename Ch>
array_adapter<Mode, Ch>::array_adapter
    (char_type* begin, char_type* end) 
    : begin_(begin), end_(end) 
    { }

template<typename Mode, typename Ch>
array_adapter<Mode, Ch>::array_adapter
    (char_type* begin, std::size_t length) 
    : begin_(begin), end_(begin + length) 
    { }

template<typename Mode, typename Ch>
array_adapter<Mode, Ch>::array_adapter
    (const char_type* begin, const char_type* end) 
    : begin_(const_cast<char_type*>(begin)),  // Treated as read-only.
      end_(const_cast<char_type*>(end))       // Treated as read-only.
{ NDNBOOST_STATIC_ASSERT((!is_convertible<Mode, output>::value)); }

template<typename Mode, typename Ch>
array_adapter<Mode, Ch>::array_adapter
    (const char_type* begin, std::size_t length) 
    : begin_(const_cast<char_type*>(begin)),       // Treated as read-only.
      end_(const_cast<char_type*>(begin) + length) // Treated as read-only.
{ NDNBOOST_STATIC_ASSERT((!is_convertible<Mode, output>::value)); }

template<typename Mode, typename Ch>
typename array_adapter<Mode, Ch>::pair_type
array_adapter<Mode, Ch>::input_sequence()
{ NDNBOOST_STATIC_ASSERT((is_convertible<Mode, input>::value));
  return pair_type(begin_, end_); }

template<typename Mode, typename Ch>
typename array_adapter<Mode, Ch>::pair_type
array_adapter<Mode, Ch>::output_sequence()
{ NDNBOOST_STATIC_ASSERT((is_convertible<Mode, output>::value));
  return pair_type(begin_, end_); }

} // End namespace detail.

//----------------------------------------------------------------------------//

} } // End namespaces iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_ARRAY_HPP_INCLUDED
