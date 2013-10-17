// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Adapted from <ndnboost/config/auto_link.hpp> and from
// http://www.boost.org/more/separate_compilation.html, by John Maddock.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_AUTO_LINK_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_AUTO_LINK_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#if defined(NDNBOOST_EXTERNAL_LIB_NAME)
# if defined(NDNBOOST_MSVC) \
     || defined(__BORLANDC__) \
     || (defined(__MWERKS__) && defined(_WIN32) && (__MWERKS__ >= 0x3000)) \
     || (defined(__ICL) && defined(_MSC_EXTENSIONS) && (_MSC_VER >= 1200)) \
     /**/
#  pragma comment(lib, NDNBOOST_EXTERNAL_LIB_NAME)
# endif
# undef NDNBOOST_EXTERNAL_LIB_NAME
#endif

//------------------Enable automatic library variant selection----------------// 

#if !defined(NDNBOOST_IOSTREAMS_SOURCE) && \
    !defined(NDNBOOST_ALL_NO_LIB) && \
    !defined(NDNBOOST_IOSTREAMS_NO_LIB) \
    /**/

// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it.
# define NDNBOOST_LIB_NAME ndnboost_iostreams

// If we're importing code from a dll, then tell auto_link.hpp about it.
# if defined(NDNBOOST_ALL_DYN_LINK) || defined(NDNBOOST_IOSTREAMS_DYN_LINK)
#  define NDNBOOST_DYN_LINK
# endif

// And include the header that does the work.
# include <ndnboost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_AUTO_LINK_HPP_INCLUDED
