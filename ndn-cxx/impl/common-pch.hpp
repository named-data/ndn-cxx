/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#ifndef NDN_CXX_IMPL_COMMON_PCH_HPP
#define NDN_CXX_IMPL_COMMON_PCH_HPP

// If the compiler supports precompiled headers, this header should be compiled
// and included before anything else

#include "ndn-cxx/detail/common.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"

// STL headers to precompile
#include <atomic>
#include <iterator>
#include <list>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Boost headers to precompile
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/bind/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/system/error_code.hpp>

#endif // NDN_CXX_IMPL_COMMON_PCH_HPP
