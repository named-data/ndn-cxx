/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_TLV_NFD_HPP
#define NDN_TLV_NFD_HPP

#include "../common.hpp"

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
  NInInterests         = 144,
  NInDatas             = 145,
  NOutInterests        = 146,
  NOutDatas            = 147,

  // Face Management
  FaceStatus            = 128,
  LocalUri              = 129,
  FaceFlags             = 194,
  FaceEventNotification = 192,
  FaceEventKind         = 193,

  // FIB Management
  FibEntry      = 128,
  NextHopRecord = 129
};

enum {
  // Local Control Header
  LocalControlHeader = 80,
  IncomingFaceId     = 81,
  NextHopFaceId      = 82
};

} // namespace nfd
} // namespace tlv

namespace nfd {

const uint64_t INVALID_FACE_ID = std::numeric_limits<uint64_t>::max();

} // namespace nfd

} // namespace ndn

#endif // NDN_TLV_NFD_HPP
