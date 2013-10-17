// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Borrowed from <ndnboost/archive/add_facet.hpp>

#ifndef NDNBOOST_IOSTREAMS_DETAIL_ADD_FACET_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_ADD_FACET_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/config.hpp>  // NDNBOOST_DINKUMWARE_STDLIB.
#include <ndnboost/detail/workaround.hpp>

//------------------Definition of add_facet-----------------------------------//

// Does STLport uses old Dinkumware locale?
#if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && \
    defined(_STLP_NO_OWN_IOSTREAMS) \
    /**/
#  if (defined(_YVALS) && !defined(__IBMCPP__)) || defined(_CPPLIB_VER)
#    define NDNBOOST_IOSTREMS_STLPORT_WITH_OLD_DINKUMWARE
#  endif
#endif

namespace ndnboost { namespace iostreams { namespace detail {

template<class Facet>
inline std::locale add_facet(const std::locale &l, Facet * f)
{
    return
        #if NDNBOOST_WORKAROUND(NDNBOOST_DINKUMWARE_STDLIB, == 1) || \
            defined(NDNBOOST_IOSTREMS_STLPORT_WITH_OLD_DINKUMWARE) \
            /**/
            std::locale(std::_Addfac(l, f));
        #else
            // standard compatible
            std::locale(l, f);
        #endif
}

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_ADD_FACET_HPP_INCLUDED
