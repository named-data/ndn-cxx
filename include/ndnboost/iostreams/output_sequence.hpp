// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_OUTPUT_SEQUENCE_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_OUTPUT_SEQUENCE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <utility>           // pair.
#include <ndnboost/config.hpp>  // DEDUCED_TYPENAME, MSVC.
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/iostreams/detail/wrap_unwrap.hpp>
#include <ndnboost/iostreams/operations_fwd.hpp>  // is_custom 
#include <ndnboost/iostreams/traits.hpp>
#include <ndnboost/mpl/if.hpp>

// Must come last.
#include <ndnboost/iostreams/detail/config/disable_warnings.hpp>

namespace ndnboost { namespace iostreams {

namespace detail {

template<typename T>
struct output_sequence_impl;

} // End namespace detail.

template<typename T>
inline std::pair<
    NDNBOOST_DEDUCED_TYPENAME char_type_of<T>::type*,
    NDNBOOST_DEDUCED_TYPENAME char_type_of<T>::type*
>
output_sequence(T& t)
{ return detail::output_sequence_impl<T>::output_sequence(t); }

namespace detail {

//------------------Definition of output_sequence_impl------------------------//

template<typename T>
struct output_sequence_impl
    : mpl::if_<
          detail::is_custom<T>,
          operations<T>,
          output_sequence_impl<direct_tag>
      >::type
    { };

template<>
struct output_sequence_impl<direct_tag> {
    template<typename U>
    static std::pair<
        NDNBOOST_DEDUCED_TYPENAME char_type_of<U>::type*,
        NDNBOOST_DEDUCED_TYPENAME char_type_of<U>::type*
    >
    output_sequence(U& u) { return u.output_sequence(); }
};

} // End namespace detail.

} } // End namespaces iostreams, boost.

#include <ndnboost/iostreams/detail/config/enable_warnings.hpp>

#endif // #ifndef NDNBOOST_IOSTREAMS_OUTPUT_SEQUENCE_HPP_INCLUDED
