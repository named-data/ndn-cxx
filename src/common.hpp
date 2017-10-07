/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

/** \file
 *  \brief import common constructs for ndn-cxx library internal use
 *  \warning This file is an implementation detail of ndn-cxx library.
 *           Aliases imported in this file MUST NOT be used outside of ndn-cxx.
 */

#ifndef NDN_COMMON_HPP
#define NDN_COMMON_HPP

#include "ndn-cxx-config.hpp"

// ndn-cxx specific macros declared in this and other headers must have NDN_CXX_ prefix
// to avoid conflicts with other projects that include ndn-cxx headers.
#ifdef NDN_CXX_HAVE_TESTS
#define NDN_CXX_VIRTUAL_WITH_TESTS virtual
#define NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED public
#define NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE public
#define NDN_CXX_PROTECTED_WITH_TESTS_ELSE_PRIVATE protected
#else
#define NDN_CXX_VIRTUAL_WITH_TESTS
#define NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED protected
#define NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE private
#define NDN_CXX_PROTECTED_WITH_TESTS_ELSE_PRIVATE private
#endif

// require C++11
#if __cplusplus < 201103L && !defined(__GXX_EXPERIMENTAL_CXX0X__)
#  error "ndn-cxx applications must be compiled using the C++11 standard (-std=c++11)"
#endif

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <iosfwd>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <utility>

namespace ndn {

using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::bad_weak_ptr;
using std::make_shared;
using std::enable_shared_from_this;

using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::const_pointer_cast;

using std::function;
using std::bind;
using std::ref;
using std::cref;

} // namespace ndn

using namespace std::placeholders;

/// \cond
// Bug 2109 workaround
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/is_placeholder.hpp>
namespace boost {
#define NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(N) \
  template<> \
  struct is_placeholder<typename std::remove_const<decltype(_##N)>::type> \
  { \
    enum _vt { \
      value = N \
    }; \
  };
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(1)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(2)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(3)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(4)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(5)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(6)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(7)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(8)
NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER(9)
#undef NDN_CXX_SPECIALIZE_BOOST_IS_PLACEHOLDER_FOR_STD_PLACEHOLDER
} // namespace boost
/// \endcond

#include <boost/assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/noncopyable.hpp>
#include <boost/throw_exception.hpp>

namespace ndn {
using boost::noncopyable;
} // namespace ndn

#include "util/backports.hpp"

#endif // NDN_COMMON_HPP
