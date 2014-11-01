/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
#  error "ndn-cxx applications must be compiled using the C++11 standard"
#endif

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unistd.h>

#if defined(__GNUC__) || defined(__clang__)
#  define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#  define DEPRECATED(func) __declspec(deprecated) func
#else
#  pragma message("DEPRECATED not implemented")
#  define DEPRECATED(func) func
#endif

namespace ndn {

const size_t MAX_NDN_PACKET_SIZE = 8800;

/** \brief the namespace contains smart pointers
 *  \deprecated use std:: directly
 */
namespace ptr_lib = std;

/** \brief the namespace contains function and bind
 *  \deprecated use std:: directly
 */
namespace func_lib = std;

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
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;
using std::placeholders::_6;
using std::placeholders::_7;
using std::placeholders::_8;
using std::placeholders::_9;
using std::ref;
using std::cref;

} // namespace ndn

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>

namespace ndn {
using boost::noncopyable;
}

#endif // NDN_COMMON_HPP
