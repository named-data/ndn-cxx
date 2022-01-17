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

#ifndef NDN_CXX_UTIL_SPAN_HPP
#define NDN_CXX_UTIL_SPAN_HPP

#define span_CONFIG_SELECT_SPAN span_SPAN_NONSTD
#ifdef NDEBUG
#define span_CONFIG_CONTRACT_LEVEL_OFF 1
#else
#define span_CONFIG_CONTRACT_LEVEL_ON 1
#endif
#define span_FEATURE_WITH_INITIALIZER_LIST_P2447 1
#define span_FEATURE_MEMBER_BACK_FRONT 1
#define span_FEATURE_MAKE_SPAN 1
#include "ndn-cxx/detail/nonstd/span-lite.hpp"

namespace ndn {

using ::nonstd::span;
using ::nonstd::dynamic_extent;
using ::nonstd::make_span;

} // namespace ndn

#endif // NDN_CXX_UTIL_SPAN_HPP
