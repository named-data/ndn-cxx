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

#ifndef NDN_TLV_NRD_HPP
#define NDN_TLV_NRD_HPP

#include "tlv.hpp"

namespace ndn {
namespace tlv {
namespace nrd {

// \deprecated use NFD RIB Management
enum {
  PrefixRegOptions = 101,
  FaceId           = 102,
  Flags            = 103,
  Cost             = 104,
  ExpirationPeriod = 105,
  StrategyName     = 106,
  Protocol         = 107
};

enum {
  NDN_FORW_CHILD_INHERIT = 1,
  NDN_FORW_CAPTURE       = 2
};

} // namespace nrd
} // namespace tlv
} // namespace ndn

#endif // NDN_TLV_NRD_HPP
