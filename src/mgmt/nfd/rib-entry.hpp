/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_MGMT_NFD_RIB_ENTRY_HPP
#define NDN_MGMT_NFD_RIB_ENTRY_HPP

#include "route-flags-traits.hpp"
#include "../../encoding/block.hpp"
#include "../../name.hpp"
#include "../../util/time.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents a route in a RibEntry
 *
 * A route indicates the availability of content via a certain face and
 * provides meta-information about the face.
 *
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#Route
 */
class Route : public RouteFlagsTraits<Route>
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  Route();

  explicit
  Route(const Block& block);

  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  Route&
  setFaceId(uint64_t faceId);

  RouteOrigin
  getOrigin() const
  {
    return m_origin;
  }

  Route&
  setOrigin(RouteOrigin origin);

  uint64_t
  getCost() const
  {
    return m_cost;
  }

  Route&
  setCost(uint64_t cost);

  uint64_t
  getFlags() const
  {
    return m_flags;
  }

  Route&
  setFlags(uint64_t flags);

  bool
  hasExpirationPeriod() const
  {
    return !!m_expirationPeriod;
  }

  time::milliseconds
  getExpirationPeriod() const
  {
    return m_expirationPeriod ? *m_expirationPeriod : time::milliseconds::max();
  }

  Route&
  setExpirationPeriod(time::milliseconds expirationPeriod);

  Route&
  unsetExpirationPeriod();

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& block);

private:
  uint64_t m_faceId;
  RouteOrigin m_origin;
  uint64_t m_cost;
  uint64_t m_flags;
  optional<time::milliseconds> m_expirationPeriod;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Route);

bool
operator==(const Route& a, const Route& b);

inline bool
operator!=(const Route& a, const Route& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const Route& route);


/**
 * \ingroup management
 * \brief represents an item in NFD RIB dataset
 *
 * A RIB entry contains one or more routes for a name prefix
 *
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#RIB-Dataset
 */
class RibEntry
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  RibEntry();

  explicit
  RibEntry(const Block& block);

  const Name&
  getName() const
  {
    return m_prefix;
  }

  RibEntry&
  setName(const Name& prefix);

  const std::vector<Route>&
  getRoutes() const
  {
    return m_routes;
  }

  template<typename InputIt>
  RibEntry&
  setRoutes(InputIt first, InputIt last)
  {
    m_routes.assign(first, last);
    m_wire.reset();
    return *this;
  }

  RibEntry&
  addRoute(const Route& route);

  RibEntry&
  clearRoutes();

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& block);

private:
  Name m_prefix;
  std::vector<Route> m_routes;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(RibEntry);

bool
operator==(const RibEntry& a, const RibEntry& b);

inline bool
operator!=(const RibEntry& a, const RibEntry& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const RibEntry& entry);

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_RIB_ENTRY_HPP
