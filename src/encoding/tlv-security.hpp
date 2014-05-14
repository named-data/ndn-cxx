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

#ifndef NDN_ENCODING_TLV_SECURITY_HPP
#define NDN_ENCODING_TLV_SECURITY_HPP

#include "tlv.hpp"

namespace ndn {
namespace tlv {
namespace security {

enum {
  IdentityPackage    = 128,
  KeyPackage         = 129,
  CertificatePackage = 130
};

} // namespace security
} // namespace tlv
} // namespace ndn

#endif // NDN_ENCODING_TLV_SECURITY_HPP
