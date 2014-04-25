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

#ifndef NDN_SECURITY_COMMON_HPP
#define NDN_SECURITY_COMMON_HPP

namespace ndn {

enum KeyType {
  KEY_TYPE_RSA,
  // KEY_TYPE_DSA,
  KEY_TYPE_AES
  // KEY_TYPE_DES,
  // KEY_TYPE_RC4,
  // KEY_TYPE_RC2
};

enum KeyClass {
  KEY_CLASS_PUBLIC,
  KEY_CLASS_PRIVATE,
  KEY_CLASS_SYMMETRIC
};

enum DigestAlgorithm {
  // DIGEST_ALGORITHM_MD2,
  // DIGEST_ALGORITHM_MD5,
  // DIGEST_ALGORITHM_SHA1,
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

}

#endif
