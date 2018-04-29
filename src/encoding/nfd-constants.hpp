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

#ifndef NDN_ENCODING_NFD_CONSTANTS_HPP
#define NDN_ENCODING_NFD_CONSTANTS_HPP

#include "../common.hpp"

namespace ndn {
namespace nfd {

const uint64_t INVALID_FACE_ID = 0;

/** \ingroup management
 */
enum FaceScope : uint8_t {
  FACE_SCOPE_NONE      = std::numeric_limits<uint8_t>::max(),
  FACE_SCOPE_NON_LOCAL = 0, ///< face is non-local
  FACE_SCOPE_LOCAL     = 1, ///< face is local
};

std::ostream&
operator<<(std::ostream& os, FaceScope faceScope);

/** \ingroup management
 */
enum FacePersistency : uint8_t {
  FACE_PERSISTENCY_NONE       = std::numeric_limits<uint8_t>::max(),
  FACE_PERSISTENCY_PERSISTENT = 0, ///< face is persistent
  FACE_PERSISTENCY_ON_DEMAND  = 1, ///< face is on-demand
  FACE_PERSISTENCY_PERMANENT  = 2, ///< face is permanent
};

std::ostream&
operator<<(std::ostream& os, FacePersistency facePersistency);

/** \ingroup management
 */
enum LinkType : uint8_t {
  LINK_TYPE_NONE           = std::numeric_limits<uint8_t>::max(),
  LINK_TYPE_POINT_TO_POINT = 0, ///< link is point-to-point
  LINK_TYPE_MULTI_ACCESS   = 1, ///< link is multi-access
  LINK_TYPE_AD_HOC         = 2, ///< link is ad hoc
};

std::ostream&
operator<<(std::ostream& os, LinkType linkType);

/** \ingroup management
 */
enum FaceFlagBit {
  BIT_LOCAL_FIELDS_ENABLED = 0, ///< whether local fields are enabled on a face
  BIT_LP_RELIABILITY_ENABLED = 1, ///< whether the link reliability feature is enabled on a face
  BIT_CONGESTION_MARKING_ENABLED = 2, ///< whether congestion detection and marking is enabled on a face
};

/** \ingroup management
 */
enum FaceEventKind : uint8_t {
  FACE_EVENT_NONE      = 0,
  FACE_EVENT_CREATED   = 1, ///< face was created
  FACE_EVENT_DESTROYED = 2, ///< face was destroyed
  FACE_EVENT_UP        = 3, ///< face went UP (from DOWN state)
  FACE_EVENT_DOWN      = 4, ///< face went DOWN (from UP state)
};

std::ostream&
operator<<(std::ostream& os, FaceEventKind faceEventKind);

/** \ingroup management
 *  \brief CS enablement flags
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#Update-config
 */
enum CsFlagBit {
  BIT_CS_ENABLE_ADMIT = 0, ///< enables the CS to admit new Data
  BIT_CS_ENABLE_SERVE = 1, ///< enables the CS to satisfy Interests using cached Data
};

/** \ingroup management
 */
enum RouteOrigin : uint16_t {
  ROUTE_ORIGIN_NONE          = std::numeric_limits<uint16_t>::max(),
  ROUTE_ORIGIN_APP           = 0,
  ROUTE_ORIGIN_AUTOREG       = 64,
  ROUTE_ORIGIN_CLIENT        = 65,
  ROUTE_ORIGIN_AUTOCONF      = 66,
  ROUTE_ORIGIN_NLSR          = 128,
  ROUTE_ORIGIN_SELFLEARNING  = 129,
  ROUTE_ORIGIN_STATIC        = 255,
};

/** \brief extract RouteOrigin from stream
 *  \post if the first token in \p contains a valid RouteOrigin as string or numeric value, it is
 *        extracted into \p routeOrigin ; otherwise, \p routeOrigin is set to \p ROUTE_ORIGIN_NONE ,
 *        and failbit is set on \p is
 */
std::istream&
operator>>(std::istream& is, RouteOrigin& routeOrigin);

std::ostream&
operator<<(std::ostream& os, RouteOrigin routeOrigin);

/** \ingroup management
 */
enum RouteFlags : uint64_t {
  ROUTE_FLAGS_NONE         = 0,
  ROUTE_FLAG_CHILD_INHERIT = 1,
  ROUTE_FLAG_CAPTURE       = 2,
};

std::ostream&
operator<<(std::ostream& os, RouteFlags routeFlags);

} // namespace nfd
} // namespace ndn

#endif // NDN_ENCODING_NFD_CONSTANTS_HPP
