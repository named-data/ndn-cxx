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

#ifndef NDN_CXX_LP_TAGS_HPP
#define NDN_CXX_LP_TAGS_HPP

#include "cache-policy.hpp"
#include "../tag-host.hpp"

namespace ndn {
namespace lp {

/** \class IncomingFaceIdTag
 *  \brief a packet tag for IncomingFaceId field
 *
 *  This tag can be attached to Interest, Data, Nack.
 */
typedef SimpleTag<uint64_t, 10> IncomingFaceIdTag;

/** \class NextHopFaceIdTag
 *  \brief a packet tag for NextHopFaceId field
 *
 *  This tag can be attached to Interest.
 */
typedef SimpleTag<uint64_t, 11> NextHopFaceIdTag;

/** \class CachePolicyTag
 *  \brief a packet tag for CachePolicy field
 *
 *  This tag can be attached to Data.
 */
typedef SimpleTag<CachePolicy, 12> CachePolicyTag;


#define NDN_LP_KEEP_LOCAL_CONTROL_HEADER

#ifdef NDN_LP_KEEP_LOCAL_CONTROL_HEADER
/** \brief expose NDNLPv2 tags as LocalControlHeader API
 *
 *  This class has the same public API as ndn::nfd::LocalControlHeader,
 *  but internally accesses IncomingFaceIdTag, NextHopFaceIdTag, CachePolicyTag
 *  on the host packet.
 *
 *  \deprecated use getTag and setTag with IncomingFaceIdTag, NextHopFaceIdTag, CachePolicyTag
 */
class LocalControlHeaderFacade
{
public:
  DEPRECATED(
  explicit
  LocalControlHeaderFacade(TagHost& pkt));

  bool
  hasIncomingFaceId() const;

  uint64_t
  getIncomingFaceId() const;

  void
  setIncomingFaceId(uint64_t incomingFaceId);

  bool
  hasNextHopFaceId() const;

  uint64_t
  getNextHopFaceId() const;

  void
  setNextHopFaceId(uint64_t nextHopFaceId);

  bool
  hasCachingPolicy() const;

  enum CachingPolicy : uint8_t {
    INVALID_POLICY = 0,
    NO_CACHE       = 1
  };

  CachingPolicy
  getCachingPolicy() const;

  void
  setCachingPolicy(CachingPolicy cachingPolicy);

private:
  TagHost& m_pkt;
};
#endif // NDN_LP_KEEP_LOCAL_CONTROL_HEADER

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_TAGS_HPP
