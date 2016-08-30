/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015-2016 Regents of the University of California.
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

#include "../common.hpp"

#ifndef NDN_CXX_HAVE_STD_TO_STRING
#include <boost/lexical_cast.hpp>
#endif

#include <algorithm>

namespace ndn {

#if __cpp_lib_make_unique
using std::make_unique;
#else
template<typename T, typename ...Args>
inline unique_ptr<T>
make_unique(Args&&... args)
{
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif // __cpp_lib_make_unique

#ifdef NDN_CXX_HAVE_STD_TO_STRING
using std::to_string;
#else
template<typename V>
inline std::string
to_string(const V& v)
{
  return boost::lexical_cast<std::string>(v);
}
#endif // NDN_CXX_HAVE_STD_TO_STRING

#if __cpp_lib_clamp >= 201603
using std::clamp;
#else
template<typename T, typename Compare>
constexpr const T&
clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<typename T>
constexpr const T&
clamp(const T& v, const T& lo, const T& hi)
{
  return (v < lo) ? lo : (hi < v) ? hi : v;
}
#endif // __cpp_lib_clamp

} // namespace ndn

#include "backports-optional.hpp"

#endif // NDN_UTIL_BACKPORTS_HPP
