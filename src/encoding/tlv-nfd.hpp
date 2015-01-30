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

#ifndef NDN_ENCODING_TLV_NFD_HPP
#define NDN_ENCODING_TLV_NFD_HPP

#include "tlv.hpp"
#include "nfd-constants.hpp"

namespace ndn {
namespace tlv {
namespace nfd {

// NFD Management protocol
enum {
  // ControlParameters
  ControlParameters   = 104,
  FaceId              = 105,
  Uri                 = 114,
  LocalControlFeature = 110,
  Origin              = 111,
  Cost                = 106,
  Flags               = 108,
  Strategy            = 107,
  ExpirationPeriod    = 109,

  // ControlResponse
  ControlResponse = 101,
  StatusCode      = 102,
  StatusText      = 103,

  // ForwarderStatus
  NfdVersion           = 128,
  StartTimestamp       = 129,
  CurrentTimestamp     = 130,
  NNameTreeEntries     = 131,
  NFibEntries          = 132,
  NPitEntries          = 133,
  NMeasurementsEntries = 134,
  NCsEntries           = 135,

  // Face Management
  FaceStatus            = 128,
  LocalUri              = 129,
  ChannelStatus         = 130,
  UriScheme             = 131,
  FaceScope             = 132,
  FacePersistency       = 133,
  LinkType              = 134,
  FaceQueryFilter       = 150,
  FaceEventNotification = 192,
  FaceEventKind         = 193,

  // ForwarderStatus and FaceStatus counters
  NInInterests  = 144,
  NInDatas      = 145,
  NOutInterests = 146,
  NOutDatas     = 147,
  NInBytes      = 148,
  NOutBytes     = 149,

  // FIB Management
  FibEntry      = 128,
  NextHopRecord = 129,

  // Strategy Choice Management
  StrategyChoice = 128,

  // RIB Management
  RibEntry = 128,
  Route    = 129

};

enum {
  // Local Control Header
  LocalControlHeader = 80,
  IncomingFaceId     = 81,
  NextHopFaceId      = 82,
  CachingPolicy      = 83,
  NoCache            = 96
};

} // namespace nfd
} // namespace tlv
} // namespace ndn

#endif // NDN_ENCODING_TLV_NFD_HPP
