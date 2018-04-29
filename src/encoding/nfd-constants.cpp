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

#include "nfd-constants.hpp"
#include "util/string-helper.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <istream>
#include <map>
#include <ostream>

namespace ndn {
namespace nfd {

std::ostream&
operator<<(std::ostream& os, FaceScope faceScope)
{
  switch (faceScope) {
    case FACE_SCOPE_NONE:
      return os << "none";
    case FACE_SCOPE_NON_LOCAL:
      return os << "non-local";
    case FACE_SCOPE_LOCAL:
      return os << "local";
  }
  return os << static_cast<unsigned>(faceScope);
}

std::ostream&
operator<<(std::ostream& os, FacePersistency facePersistency)
{
  switch (facePersistency) {
    case FACE_PERSISTENCY_NONE:
      return os << "none";
    case FACE_PERSISTENCY_PERSISTENT:
      return os << "persistent";
    case FACE_PERSISTENCY_ON_DEMAND:
      return os << "on-demand";
    case FACE_PERSISTENCY_PERMANENT:
      return os << "permanent";
  }
  return os << static_cast<unsigned>(facePersistency);
}

std::ostream&
operator<<(std::ostream& os, LinkType linkType)
{
  switch (linkType) {
    case LINK_TYPE_NONE:
      return os << "none";
    case LINK_TYPE_POINT_TO_POINT:
      return os << "point-to-point";
    case LINK_TYPE_MULTI_ACCESS:
      return os << "multi-access";
    case LINK_TYPE_AD_HOC:
      return os << "adhoc";
  }
  return os << static_cast<unsigned>(linkType);
}

std::ostream&
operator<<(std::ostream& os, FaceEventKind faceEventKind)
{
  switch (faceEventKind) {
    case FACE_EVENT_NONE:
      return os << "none";
    case FACE_EVENT_CREATED:
      return os << "created";
    case FACE_EVENT_DESTROYED:
      return os << "destroyed";
    case FACE_EVENT_UP:
      return os << "up";
    case FACE_EVENT_DOWN:
      return os << "down";
  }
  return os << static_cast<unsigned>(faceEventKind);
}

std::istream&
operator>>(std::istream& is, RouteOrigin& routeOrigin)
{
  using boost::algorithm::iequals;

  std::string s;
  is >> s;

  if (iequals(s, "none"))
    routeOrigin = ROUTE_ORIGIN_NONE;
  else if (iequals(s, "app"))
    routeOrigin = ROUTE_ORIGIN_APP;
  else if (iequals(s, "autoreg"))
    routeOrigin = ROUTE_ORIGIN_AUTOREG;
  else if (iequals(s, "client"))
    routeOrigin = ROUTE_ORIGIN_CLIENT;
  else if (iequals(s, "autoconf"))
    routeOrigin = ROUTE_ORIGIN_AUTOCONF;
  else if (iequals(s, "nlsr"))
    routeOrigin = ROUTE_ORIGIN_NLSR;
  else if (iequals(s, "selflearning"))
    routeOrigin = ROUTE_ORIGIN_SELFLEARNING;
  else if (iequals(s, "static"))
    routeOrigin = ROUTE_ORIGIN_STATIC;
  else {
    // To reject negative numbers, we parse as a wider signed type, and compare with the range.
    static_assert(std::numeric_limits<std::underlying_type<RouteOrigin>::type>::max() <=
                  std::numeric_limits<int>::max(), "");

    int v = -1;
    try {
      v = boost::lexical_cast<int>(s);
    }
    catch (const boost::bad_lexical_cast&) {
    }

    if (v >= std::numeric_limits<std::underlying_type<RouteOrigin>::type>::min() &&
        v <= std::numeric_limits<std::underlying_type<RouteOrigin>::type>::max()) {
      routeOrigin = static_cast<RouteOrigin>(v);
    }
    else {
      routeOrigin = ROUTE_ORIGIN_NONE;
      is.setstate(std::ios::failbit);
    }
  }

  return is;
}

std::ostream&
operator<<(std::ostream& os, RouteOrigin routeOrigin)
{
  switch (routeOrigin) {
    case ROUTE_ORIGIN_NONE:
      return os << "none";
    case ROUTE_ORIGIN_APP:
      return os << "app";
    case ROUTE_ORIGIN_AUTOREG:
      return os << "autoreg";
    case ROUTE_ORIGIN_CLIENT:
      return os << "client";
    case ROUTE_ORIGIN_AUTOCONF:
      return os << "autoconf";
    case ROUTE_ORIGIN_NLSR:
      return os << "nlsr";
    case ROUTE_ORIGIN_SELFLEARNING:
      return os << "selflearning";
    case ROUTE_ORIGIN_STATIC:
      return os << "static";
  }
  return os << static_cast<unsigned>(routeOrigin);
}

std::ostream&
operator<<(std::ostream& os, RouteFlags routeFlags)
{
  if (routeFlags == ROUTE_FLAGS_NONE) {
    return os << "none";
  }

  static const std::map<RouteFlags, std::string> knownBits = {
    {ROUTE_FLAG_CHILD_INHERIT, "child-inherit"},
    {ROUTE_FLAG_CAPTURE, "capture"}
  };

  auto join = make_ostream_joiner(os, '|');
  for (const auto& pair : knownBits) {
    RouteFlags bit = ROUTE_FLAGS_NONE;
    std::string token;
    std::tie(bit, token) = pair;

    if ((routeFlags & bit) != 0) {
      join = token;
      routeFlags = static_cast<RouteFlags>(routeFlags & ~bit);
    }
  }

  if (routeFlags != ROUTE_FLAGS_NONE) {
    join = AsHex{routeFlags};
  }

  return os;
}

} // namespace nfd
} // namespace ndn
