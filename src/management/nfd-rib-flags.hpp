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

#ifndef NDN_MANAGEMENT_NFD_RIB_FLAGS_HPP
#define NDN_MANAGEMENT_NFD_RIB_FLAGS_HPP

#include "../encoding/nfd-constants.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief implements getters to each RIB flag
 *
 * \tparam T class containing a RibFlags field and implements
 *           `RibFlags getFlags() const` method
 */
template<typename T>
class RibFlagsTraits
{
public:
  bool
  isChildInherit() const
  {
    return static_cast<const T*>(this)->getFlags() & ROUTE_FLAG_CHILD_INHERIT;
  }

  bool
  isRibCapture() const
  {
    return static_cast<const T*>(this)->getFlags() & ROUTE_FLAG_CAPTURE;
  }
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_RIB_FLAGS_HPP
