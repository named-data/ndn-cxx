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
 * PARTICULAR PURPOSE.See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "tags.hpp"
#include "../encoding/nfd-constants.hpp"

namespace ndn {
namespace lp {

#ifdef NDN_LP_KEEP_LOCAL_CONTROL_HEADER

using ndn::nfd::INVALID_FACE_ID;

LocalControlHeaderFacade::LocalControlHeaderFacade(TagHost& pkt)
  : m_pkt(pkt)
{
}

bool
LocalControlHeaderFacade::hasIncomingFaceId() const
{
  return m_pkt.getTag<IncomingFaceIdTag>() != nullptr;
}

uint64_t
LocalControlHeaderFacade::getIncomingFaceId() const
{
  shared_ptr<IncomingFaceIdTag> tag = m_pkt.getTag<IncomingFaceIdTag>();
  if (tag == nullptr) {
    return INVALID_FACE_ID;
  }
  return *tag;
}

void
LocalControlHeaderFacade::setIncomingFaceId(uint64_t incomingFaceId)
{
  if (incomingFaceId == INVALID_FACE_ID) {
    m_pkt.removeTag<IncomingFaceIdTag>();
    return;
  }

  auto tag = make_shared<IncomingFaceIdTag>(incomingFaceId);
  m_pkt.setTag(tag);
}

bool
LocalControlHeaderFacade::hasNextHopFaceId() const
{
  return m_pkt.getTag<NextHopFaceIdTag>() != nullptr;
}

uint64_t
LocalControlHeaderFacade::getNextHopFaceId() const
{
  shared_ptr<NextHopFaceIdTag> tag = m_pkt.getTag<NextHopFaceIdTag>();
  if (tag == nullptr) {
    return INVALID_FACE_ID;
  }
  return *tag;
}

void
LocalControlHeaderFacade::setNextHopFaceId(uint64_t nextHopFaceId)
{
  if (nextHopFaceId == INVALID_FACE_ID) {
    m_pkt.removeTag<NextHopFaceIdTag>();
    return;
  }

  auto tag = make_shared<NextHopFaceIdTag>(nextHopFaceId);
  m_pkt.setTag(tag);
}

bool
LocalControlHeaderFacade::hasCachingPolicy() const
{
  return m_pkt.getTag<CachePolicyTag>() != nullptr;
}

LocalControlHeaderFacade::CachingPolicy
LocalControlHeaderFacade::getCachingPolicy() const
{
  shared_ptr<CachePolicyTag> tag = m_pkt.getTag<CachePolicyTag>();
  if (tag == nullptr) {
    return INVALID_POLICY;
  }
  switch (tag->get().getPolicy()) {
    case CachePolicyType::NO_CACHE:
      return NO_CACHE;
    default:
      return INVALID_POLICY;
  }
}

void
LocalControlHeaderFacade::setCachingPolicy(CachingPolicy cachingPolicy)
{
  switch (cachingPolicy) {
    case NO_CACHE: {
      m_pkt.setTag(make_shared<CachePolicyTag>(CachePolicy().setPolicy(CachePolicyType::NO_CACHE)));
      break;
    }
    default:
      m_pkt.removeTag<CachePolicyTag>();
      break;
  }
}

#endif // NDN_LP_KEEP_LOCAL_CONTROL_HEADER

} // namespace lp
} // namespace ndn
