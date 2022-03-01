/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_UTIL_BACKPORTS_HPP
#define NDN_CXX_UTIL_BACKPORTS_HPP

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
// https://wg21.link/P0188
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
// https://wg21.link/P0189
// [[nodiscard]] attribute (C++17)
//
#if (__cplusplus > 201402L) && NDN_CXX_HAS_CPP_ATTRIBUTE(nodiscard)
#  define NDN_CXX_NODISCARD [[nodiscard]]
#elif NDN_CXX_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)
#  define NDN_CXX_NODISCARD [[gnu::warn_unused_result]]
#else
#  define NDN_CXX_NODISCARD
#endif

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
std::string
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
// std::to_underlying() (C++23)
//
#if __cpp_lib_to_underlying >= 202102L
using std::to_underlying;
#else
template<typename T>
NDN_CXX_NODISCARD constexpr std::underlying_type_t<T>
to_underlying(T val) noexcept
{
  // instantiating underlying_type with a non-enum type is UB before C++20
  static_assert(std::is_enum<T>::value, "");
  return static_cast<std::underlying_type_t<T>>(val);
}
#endif // __cpp_lib_to_underlying

//
// https://wg21.link/P0627
// std::unreachable() (C++23)
//
#ifndef NDEBUG
#  define NDN_CXX_UNREACHABLE BOOST_ASSERT(false)
#elif __cpp_lib_unreachable >= 202202L
#  define NDN_CXX_UNREACHABLE std::unreachable()
#else
#  define NDN_CXX_UNREACHABLE ::ndn::detail::unreachable()
namespace detail {
[[noreturn]] inline void
unreachable()
{
#if BOOST_COMP_GNUC || BOOST_COMP_CLANG
  __builtin_unreachable();
#elif BOOST_COMP_MSVC
  __assume(0);
#endif
} // unreachable()
} // namespace detail
#endif

} // namespace ndn

#endif // NDN_CXX_UTIL_BACKPORTS_HPP
