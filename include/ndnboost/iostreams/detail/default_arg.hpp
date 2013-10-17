// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2005-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_DEFAULT_ARG_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_DEFAULT_ARG_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif            

#include <ndnboost/config.hpp>
#include <ndnboost/detail/workaround.hpp>

#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
# include <ndnboost/mpl/identity.hpp>
# define NDNBOOST_IOSTREAMS_DEFAULT_ARG(arg) mpl::identity< arg >::type
#else
# define NDNBOOST_IOSTREAMS_DEFAULT_ARG(arg) arg
#endif

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_DEFAULT_ARG_HPP_INCLUDED
