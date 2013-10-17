/*
 * Distributed under the Boost Software License, Version 1.0.(See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)
 * 
 * See http://www.boost.org/libs/iostreams for documentation.
 *
 * File:        ndnboost/iostreams/detail/restrict.hpp
 * Date:        Sun Jan 06 12:57:30 MST 2008
 * Copyright:   2008 CodeRage, LLC
                2004-2007 Jonathan Turkanis
 * Author:      Jonathan Turkanis
 * Contact:     turkanis at coderage dot com
 *
 * Defines the class template ndnboost::iostreams::restriction and the 
 * overloaded function template ndnboost::iostreams::restrict
 */

#ifndef NDNBOOST_IOSTREAMS_RESTRICT_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_RESTRICT_HPP_INCLUDED

#include <ndnboost/iostreams/detail/restrict_impl.hpp>
#define NDNBOOST_IOSTREAMS_RESTRICT restrict
#include <ndnboost/iostreams/detail/restrict_impl.hpp>
#undef NDNBOOST_IOSTREAMS_RESTRICT

#endif // #ifndef NDNBOOST_IOSTREAMS_RESTRICT_HPP_INCLUDED
