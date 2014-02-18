/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDN_TLV_SECURITY_HPP
#define NDN_TLV_SECURITY_HPP

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

#endif // NDN_TLV_SECURITY_HPP
