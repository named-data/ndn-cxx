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

#ifndef NDN_TLV_NDND_HPP
#define NDN_TLV_NDND_HPP

#include "tlv.hpp"

namespace ndn {
namespace tlv {
namespace ndnd {

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

} // namespace ndnd
} // namespace tlv


// temporary, until all the dependent code is updated
namespace Tlv {
namespace FaceManagement {
using namespace ::ndn::tlv::ndnd;
} // namespace FaceManagement
} // namespace Tlv

} // namespace ndn

#endif // NDN_TLV_NDND_HPP
