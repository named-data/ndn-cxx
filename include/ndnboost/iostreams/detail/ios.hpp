// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_IOS_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_IOS_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              
                 
#include <ndnboost/config.hpp> // NDNBOOST_MSVC.
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/iostreams/detail/config/wide_streams.hpp>
#ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES
# if !NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003)
#  include <ios>
# else
#  include <istream>
#  include <ostream>
# endif
#else 
# include <exception>
# include <iosfwd>
#endif 

namespace ndnboost { namespace iostreams { namespace detail {

#ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES //--------------------------------//
# define NDNBOOST_IOSTREAMS_BASIC_IOS(ch, tr)  std::basic_ios< ch, tr >
# if !NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) && \
     !NDNBOOST_WORKAROUND(__BORLANDC__, < 0x600) && \
     !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300) \
     /**/

#define NDNBOOST_IOS                std::ios
#define NDNBOOST_IOSTREAMS_FAILURE  std::ios::failure

# else

#define NDNBOOST_IOS                std::ios_base
#define NDNBOOST_IOSTREAMS_FAILURE  std::ios_base::failure

# endif
#else // #ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES //-----------------------//

#define NDNBOOST_IOS                          std::ios
#define NDNBOOST_IOSTREAMS_BASIC_IOS(ch, tr)  std::ios
#define NDNBOOST_IOSTREAMS_FAILURE            ndnboost::iostreams::detail::failure

class failure : std::exception {    
public:
    explicit failure(const std::string& what_arg) : what_(what_arg) { }
    const char* what() const { return what_.c_str(); }
private:
    std::string what_;
};

#endif // #ifndef NDNBOOST_IOSTREAMS_NO_STREAM_TEMPLATES //----------------------//

} } } // End namespace failure, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_IOS_HPP_INCLUDED
