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

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("DEPRECATED not implemented")
#define DEPRECATED(func) func
#endif

#ifdef NDN_CXX_HAVE_CXX11

#if defined(__GNUC__)
#  if !defined(__GXX_EXPERIMENTAL_CXX0X__) && __cplusplus < 201103L
#    error "NDN-CXX library is configured and compiled in C++11 mode, but the current compiler is not C++11 enabled"
#  endif // !defined(__GXX_EXPERIMENTAL_CXX0X__) && __cplusplus < 201103L
#endif // defined(__GNUC__)

#if defined(__clang__) && __cplusplus < 201103L
#  error "NDN-CXX library is configured and compiled in C++11 mode, but the current compiler is not C++11 enabled"
#endif // defined(__clang__) && (__cplusplus < 201103L)


#include <memory>
#include <functional>

namespace ndn {

namespace ptr_lib = std;
namespace func_lib = std;

using std::shared_ptr;
using std::weak_ptr;
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


#else

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace ndn {

namespace ptr_lib = boost;
namespace func_lib = boost;

using boost::shared_ptr;
using boost::weak_ptr;
using boost::make_shared;
using boost::enable_shared_from_this;

using boost::static_pointer_cast;
using boost::dynamic_pointer_cast;
using boost::const_pointer_cast;

using boost::function;
using boost::bind;

using boost::ref;
using boost::cref;

} // namespace ndn

#endif // NDN_CXX_HAVE_CXX11

#include <boost/utility.hpp>

namespace ndn {

using boost::noncopyable;

}

#endif // NDN_COMMON_HPP
