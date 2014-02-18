/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TLV_NFD_HPP
#define NDN_TLV_NFD_HPP

namespace ndn {
namespace tlv {
namespace nfd {

enum {
  // ControlResponse
  ControlResponse = 101,
  StatusCode      = 102,
  StatusText      = 103,

  // FIB Management Protocol
  FibManagementOptions = 104,
  FaceId          = 105,
  Cost            = 106,
  Strategy        = 107,

  // Face Management Protocol
  FaceManagementOptions = 108,
  Uri                   = 114
};

enum {
  // Local Control Header
  LocalControlHeader = 80,
  IncomingFaceId     = 81,
  NextHopFaceId      = 82
};

} // namespace nfd
} // namespace tlv
} // namespace ndn

#endif // NDN_TLV_NFD_HPP
