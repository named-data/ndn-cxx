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

#include "nfd-constants.hpp"
#include <iostream>

namespace ndn {
namespace nfd {

std::ostream&
operator<<(std::ostream& os, FaceScope faceScope)
{
  switch (faceScope) {
  case FACE_SCOPE_NON_LOCAL:
    os << "non-local";
    break;
  case FACE_SCOPE_LOCAL:
    os << "local";
    break;
  default:
    os << "unknown";
    break;
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, FacePersistency facePersistency)
{
  switch (facePersistency) {
  case FACE_PERSISTENCY_PERSISTENT:
    os << "persistent";
    break;
  case FACE_PERSISTENCY_ON_DEMAND:
    os << "on-demand";
    break;
  case FACE_PERSISTENCY_PERMANENT:
    os << "permanent";
    break;
  default:
    os << "unknown";
    break;
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, LinkType linkType)
{
  switch (linkType) {
  case LINK_TYPE_POINT_TO_POINT:
    os << "point-to-point";
    break;
  case LINK_TYPE_MULTI_ACCESS:
    os << "multi-access";
    break;
  default:
    os << "unknown";
    break;
  }
  return os;
}

} // namespace nfd
} // namespace ndn
