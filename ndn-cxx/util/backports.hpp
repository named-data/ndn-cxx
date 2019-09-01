/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_UTIL_BACKPORTS_HPP
#define NDN_UTIL_BACKPORTS_HPP

#include "ndn-cxx/detail/common.hpp"

#include <boost/predef/compiler/clang.h>
#include <boost/predef/compiler/gcc.h>
#include <boost/predef/compiler/visualc.h>

#ifdef __has_cpp_attribute
#  define NDN_CXX_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define NDN_CXX_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifdef __has_include
#  define NDN_CXX_HAS_INCLUDE(x) __has_include(x)
#else
#  define NDN_CXX_HAS_INCLUDE(x) 0
#endif

//
// http://wg21.link/P0188
// [[fallthrough]] attribute (C++17)
//
#if (__cplusplus > 201402L) && NDN_CXX_HAS_CPP_ATTRIBUTE(fallthrough)
#  define NDN_CXX_FALLTHROUGH [[fallthrough]]
#elif NDN_CXX_HAS_CPP_ATTRIBUTE(clang::fallthrough)
#  define NDN_CXX_FALLTHROUGH [[clang::fallthrough]]
#elif NDN_CXX_HAS_CPP_ATTRIBUTE(gnu::fallthrough)
#  define NDN_CXX_FALLTHROUGH [[gnu::fallthrough]]
#elif BOOST_COMP_GNUC >= BOOST_VERSION_NUMBER(7,0,0)
#  define NDN_CXX_FALLTHROUGH __attribute__((fallthrough))
#else
#  define NDN_CXX_FALLTHROUGH ((void)0)
#endif

//
// http://wg21.link/P0189
// [[nodiscard]] attribute (C++17)
//
#if (__cplusplus > 201402L) && NDN_CXX_HAS_CPP_ATTRIBUTE(nodiscard)
#  define NDN_CXX_NODISCARD [[nodiscard]]
#elif NDN_CXX_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)
#  define NDN_CXX_NODISCARD [[gnu::warn_unused_result]]
#else
#  define NDN_CXX_NODISCARD
#endif

#ifndef NDEBUG
#  define NDN_CXX_UNREACHABLE BOOST_ASSERT(false)
#elif BOOST_COMP_GNUC || BOOST_COMP_CLANG
#  define NDN_CXX_UNREACHABLE __builtin_unreachable()
#elif BOOST_COMP_MSVC
#  define NDN_CXX_UNREACHABLE __assume(0)
#else
#  include <cstdlib>
#  define NDN_CXX_UNREACHABLE std::abort()
#endif

#include "ndn-cxx/util/nonstd/any.hpp"
#include "ndn-cxx/util/nonstd/optional.hpp"
#include "ndn-cxx/util/nonstd/variant.hpp"
#include "ndn-cxx/util/ostream-joiner.hpp"

#ifndef NDN_CXX_HAVE_STD_TO_STRING
#include <boost/lexical_cast.hpp>
#endif

namespace ndn {

//
// https://redmine.named-data.net/issues/2743
// std::to_string() (C++11)
//
#ifdef NDN_CXX_HAVE_STD_TO_STRING
using std::to_string;
#else
template<typename T>
inline std::string
to_string(const T& val)
{
  return boost::lexical_cast<std::string>(val);
}
#endif // NDN_CXX_HAVE_STD_TO_STRING

//
// https://wg21.link/P0025
// std::clamp() (C++17)
//
#if __cpp_lib_clamp >= 201603L
using std::clamp;
#else
template<typename T, typename Compare>
constexpr const T&
clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
  BOOST_ASSERT(!comp(hi, lo));
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<typename T>
constexpr const T&
clamp(const T& v, const T& lo, const T& hi)
{
  BOOST_ASSERT(!(hi < lo));
  return (v < lo) ? lo : (hi < v) ? hi : v;
}
#endif // __cpp_lib_clamp

//
// https://wg21.link/P1682
// std::to_underlying() (approved for LWG as of July 2019)
//
#if __cpp_lib_to_underlying >= 202002L
using std::to_underlying;
#else
template<typename T>
constexpr std::underlying_type_t<T>
to_underlying(T val) noexcept
{
  static_assert(std::is_enum<T>::value, "");
  return static_cast<std::underlying_type_t<T>>(val);
}
#endif // __cpp_lib_to_underlying

using ::nonstd::any;
using ::nonstd::any_cast;
using ::nonstd::bad_any_cast;
using ::nonstd::make_any;

using ::nonstd::optional;
using ::nonstd::bad_optional_access;
using ::nonstd::nullopt;
using ::nonstd::nullopt_t;
using ::nonstd::in_place;
using ::nonstd::in_place_t;
using ::nonstd::make_optional;

using ::nonstd::variant;
using ::nonstd::bad_variant_access;
using ::nonstd::monostate;
using ::nonstd::variant_npos;
using ::nonstd::get;
using ::nonstd::get_if;
using ::nonstd::holds_alternative;
using ::nonstd::visit;

} // namespace ndn

#endif // NDN_UTIL_BACKPORTS_HPP
