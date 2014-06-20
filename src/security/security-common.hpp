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

#ifndef NDN_SECURITY_COMMON_HPP
#define NDN_SECURITY_COMMON_HPP

namespace ndn {

namespace signed_interest{

enum {
  POS_SIG_VALUE = -1,
  POS_SIG_INFO = -2,
  POS_RANDOM_VAL = -3,
  POS_TIMESTAMP = -4,

  MIN_LENGTH = 4
};

} // namespace signed_interest

enum KeyType {
  KEY_TYPE_RSA   = 0,
  KEY_TYPE_ECDSA = 1,
  // KEY_TYPE_DSA,
  KEY_TYPE_AES   = 128,
  // KEY_TYPE_DES,
  // KEY_TYPE_RC4,
  // KEY_TYPE_RC2
  KEY_TYPE_NULL  = 255
};

enum KeyClass {
  KEY_CLASS_PUBLIC,
  KEY_CLASS_PRIVATE,
  KEY_CLASS_SYMMETRIC
};

enum DigestAlgorithm {
  DIGEST_ALGORITHM_SHA256
};

enum EncryptMode {
  ENCRYPT_MODE_DEFAULT,
  ENCRYPT_MODE_CFB_AES
  // ENCRYPT_MODE_CBC_AES
};

enum AclType {
  ACL_TYPE_PUBLIC,
  ACL_TYPE_PRIVATE
};

} // namespace ndn

#endif
