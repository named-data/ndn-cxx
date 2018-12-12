/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_IMPL_COMMON_PCH_HPP
#define NDN_IMPL_COMMON_PCH_HPP

// If the compiler supports precompiled headers, this header should be compiled
// and included before anything else

#include "ndn-cxx/detail/common.hpp"

// STL headers to precompile
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Boost headers to precompile
#include <boost/algorithm/string.hpp>
#include <boost/chrono.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>

#endif // NDN_IMPL_COMMON_PCH_HPP
