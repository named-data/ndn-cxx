/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TLV_FACE_MANAGEMENT_HPP
#define NDN_TLV_FACE_MANAGEMENT_HPP

#include "tlv.hpp"

namespace ndn {
namespace Tlv {
namespace FaceManagement {

enum {
  FaceInstance       = 128,
  ForwardingEntry    = 129,
  StatusResponse     = 130,
  Action             = 131,
  FaceID             = 132,
  IPProto            = 133,
  Host               = 134,
  Port               = 135,
  MulticastInterface = 136,
  MulticastTTL       = 137,
  ForwardingFlags    = 138,
  StatusCode         = 139,
  StatusText         = 140
};

enum {
  FORW_ACTIVE         = 1,
  FORW_CHILD_INHERIT  = 2,
  FORW_ADVERTISE      = 4,
  FORW_LAST           = 8,
  FORW_CAPTURE       = 16,
  FORW_LOCAL         = 32,
  FORW_TAP           = 64,
  FORW_CAPTURE_OK   = 128
};

} // namespace FaceManagement
} // namespace Tlv
} // namespace ndn

#endif // NDN_TLV_FACE_MANAGEMENT_HPP
