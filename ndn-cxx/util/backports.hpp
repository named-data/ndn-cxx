/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include <utility>

namespace ndn {

//
// https://wg21.link/P1682
// std::to_underlying() (C++23)
//
#if __cpp_lib_to_underlying >= 202102L
using std::to_underlying;
#else
template<typename T>
[[nodiscard]] constexpr std::underlying_type_t<T>
to_underlying(T val) noexcept
{
  // instantiating underlying_type with a non-enum type is UB before C++20
  static_assert(std::is_enum_v<T>, "");
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
