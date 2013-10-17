// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).

// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_FILESYSTEM_UTF8_CODECVT_FACET_HPP
#define NDNBOOST_FILESYSTEM_UTF8_CODECVT_FACET_HPP

#include <ndnboost/filesystem/config.hpp>

#define NDNBOOST_UTF8_BEGIN_NAMESPACE \
     namespace ndnboost { namespace filesystem { namespace detail {

#define NDNBOOST_UTF8_END_NAMESPACE }}}
#define NDNBOOST_UTF8_DECL NDNBOOST_FILESYSTEM_DECL

#include <ndnboost/detail/utf8_codecvt_facet.hpp>

#undef NDNBOOST_UTF8_BEGIN_NAMESPACE
#undef NDNBOOST_UTF8_END_NAMESPACE
#undef NDNBOOST_UTF8_DECL

#endif
