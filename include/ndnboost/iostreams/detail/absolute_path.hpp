/*
 * Distributed under the Boost Software License, Version 1.0.(See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)
 * 
 * See http://www.boost.org/libs/iostreams for documentation.

 * File:        ndnboost/iostreams/detail/execute.hpp
 * Date:        Thu Dec 06 13:21:54 MST 2007
 * Copyright:   2007-2008 CodeRage, LLC
 * Author:      Jonathan Turkanis
 * Contact:     turkanis at coderage dot com
 *
 * Defines the function ndnboost::iostreams::detail::absolute_path, used for 
 * debug output for mapped files.
 */

#ifndef NDNBOOST_IOSTREAMS_DETAIL_ABSOLUTE_PATH_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_ABSOLUTE_PATH_HPP_INCLUDED

#include <string>
#include <ndnboost/iostreams/detail/config/windows_posix.hpp>
#ifdef NDNBOOST_IOSTREAMS_WINDOWS
# include <cctype>
#endif
#include <ndnboost/iostreams/detail/current_directory.hpp>

namespace ndnboost { namespace iostreams { namespace detail {

// Resolves the given path relative to the current working directory
inline std::string absolute_path(const std::string& path)
{
#ifdef NDNBOOST_IOSTREAMS_WINDOWS
    return path.size() && (path[0] == '/' || path[0] == '\\') ||
           path.size() > 1 && std::isalpha(path[0]) && path[1] == ':' ?
               path :
               current_directory() + '\\' + path;
#else // #ifdef NDNBOOST_IOSTREAMS_WINDOWS
    return path.size() && (path[0] == '/') ?
        path :
        current_directory() + '/' + path;
#endif // #ifdef NDNBOOST_IOSTREAMS_WINDOWS
}

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_ABSOLUTE_PATH_HPP_INCLUDED
