/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/lp/cache-policy.hpp"
#include "ndn-cxx/lp/empty-value.hpp"
#include "ndn-cxx/lp/prefix-announcement-header.hpp"
#include "ndn-cxx/tag.hpp"

namespace ndn::lp {

/**
 * \class IncomingFaceIdTag
 * \brief A packet tag for IncomingFaceId field.
 *
 * This tag can be attached to Interest, Data, Nack.
 */
using IncomingFaceIdTag = SimpleTag<uint64_t, 10>;

/**
 * \class NextHopFaceIdTag
 * \brief A packet tag for NextHopFaceId field.
 *
 * This tag can be attached to Interest.
 */
using NextHopFaceIdTag = SimpleTag<uint64_t, 11>;

/**
 * \class CachePolicyTag
 * \brief A packet tag for CachePolicy field.
 *
 * This tag can be attached to Data.
 */
using CachePolicyTag = SimpleTag<CachePolicy, 12>;

/**
 * \class CongestionMarkTag
 * \brief A packet tag for CongestionMark field.
 *
 * This tag can be attached to Interest, Data, Nack.
 */
using CongestionMarkTag = SimpleTag<uint64_t, 13>;

/**
 * \class NonDiscoveryTag
 * \brief A packet tag for NonDiscovery field.
 *
 * This tag can be attached to Interest.
 */
using NonDiscoveryTag = SimpleTag<EmptyValue, 14>;

/**
 * \class PrefixAnnouncementTag
 * \brief A packet tag for PrefixAnnouncement field.
 *
 * This tag can be attached to Data.
 */
using PrefixAnnouncementTag = SimpleTag<PrefixAnnouncementHeader, 15>;

} // namespace ndn::lp

#endif // NDN_CXX_LP_TAGS_HPP
