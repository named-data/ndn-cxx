/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TLV_NRD_HPP
#define NDN_TLV_NRD_HPP

#include "tlv.hpp"

namespace ndn {
namespace tlv {
namespace nrd {

enum {
  PrefixRegOptions = 101,
  FaceId           = 102,
  Flags            = 103,
  Cost             = 104,
  ExpirationPeriod = 105,
  StrategyName     = 106,
  Protocol         = 107,
};

enum {
  NDN_FORW_CHILD_INHERIT = 1,
  NDN_FORW_CAPTURE       = 2,
};

} // namespace nrd
} // namespace tlv
} // namespace ndn

#endif // NDN_TLV_NRD_HPP
