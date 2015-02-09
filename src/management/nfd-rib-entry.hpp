/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_MANAGEMENT_NFD_RIB_ENTRY_HPP
#define NDN_MANAGEMENT_NFD_RIB_ENTRY_HPP

#include "nfd-rib-flags.hpp" // include this first, to ensure it compiles on its own.
#include "../name.hpp"
#include "../util/time.hpp"

#include <list>

namespace ndn {
namespace nfd {

/**
 * @ingroup management
 *
 * @brief Data abstraction for Route
 *
 * A route indicates the availability of content via a certain face and
 * provides meta-information about the face.
 *
 *     Route := ROUTE-TYPE TLV-LENGTH
 *                FaceId
 *                Origin
 *                Cost
 *                Flags
 *                ExpirationPeriod?
 *
 * @sa http://redmine.named-data.net/projects/nfd/wiki/RibMgmt
 */
class Route : public RibFlagsTraits<Route>
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what) : tlv::Error(what)
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
  setFaceId(uint64_t faceId)
  {
    m_faceId = faceId;
    m_wire.reset();
    return *this;
  }

  uint64_t
  getOrigin() const
  {
    return m_origin;
  }

  /** @brief set Origin
   *  @param origin a code defined in ndn::nfd::RouteOrigin
   */
  Route&
  setOrigin(uint64_t origin)
  {
    m_origin = origin;
    m_wire.reset();
    return *this;
  }

  uint64_t
  getCost() const
  {
    return m_cost;
  }

  Route&
  setCost(uint64_t cost)
  {
    m_cost = cost;
    m_wire.reset();
    return *this;
  }

  uint64_t
  getFlags() const
  {
    return m_flags;
  }

  /** @brief set route inheritance flags
   *  @param flags a bitwise OR'ed code from ndn::nfd::RouteFlags
   */
  Route&
  setFlags(uint64_t flags)
  {
    m_flags = flags;
    m_wire.reset();
    return *this;
  }

  static const time::milliseconds INFINITE_EXPIRATION_PERIOD;

  const time::milliseconds&
  getExpirationPeriod() const
  {
    return m_expirationPeriod;
  }

  Route&
  setExpirationPeriod(const time::milliseconds& expirationPeriod)
  {
    m_expirationPeriod = expirationPeriod;

    m_hasInfiniteExpirationPeriod = m_expirationPeriod == INFINITE_EXPIRATION_PERIOD;

    m_wire.reset();
    return *this;
  }

  bool
  hasInfiniteExpirationPeriod() const
  {
    return m_hasInfiniteExpirationPeriod;
  }

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

private:
  uint64_t m_faceId;
  uint64_t m_origin;
  uint64_t m_cost;
  uint64_t m_flags;
  time::milliseconds m_expirationPeriod;
  bool m_hasInfiniteExpirationPeriod;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const Route& route);

/**
 * @ingroup management
 *
 * @brief Data abstraction for RIB entry
 *
 * A RIB entry contains one or more routes for the name prefix
 *
 *     RibEntry := RIB-ENTRY-TYPE TLV-LENGTH
 *                Name
 *                Route+
 *
 * @sa http://redmine.named-data.net/projects/nfd/wiki/RibMgmt
 */
class RibEntry
{
public:
  class Error : public tlv::Error
  {
  public:
    Error(const std::string& what) : tlv::Error(what)
    {
    }
  };

  typedef std::list<Route> RouteList;
  typedef RouteList::const_iterator iterator;

  RibEntry();

  explicit
  RibEntry(const Block& block);

  const Name&
  getName() const
  {
    return m_prefix;
  }

  RibEntry&
  setName(const Name& prefix)
  {
    m_prefix = prefix;
    m_wire.reset();
    return *this;
  }

  const std::list<Route>&
  getRoutes() const
  {
    return m_routes;
  }

  RibEntry&
  addRoute(const Route& route)
  {
    m_routes.push_back(route);
    m_wire.reset();
    return *this;
  }

  RibEntry&
  clearRoutes()
  {
    m_routes.clear();
    return *this;
  }

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

  iterator
  begin() const;

  iterator
  end() const;

private:
  Name m_prefix;
  RouteList m_routes;

  mutable Block m_wire;
};

inline RibEntry::iterator
RibEntry::begin() const
{
  return m_routes.begin();
}

inline RibEntry::iterator
RibEntry::end() const
{
  return m_routes.end();
}

std::ostream&
operator<<(std::ostream& os, const RibEntry& entry);

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_RIB_ENTRY_HPP
