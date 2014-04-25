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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_SECURITY_ENCRYPTION_MANAGER_HPP
#define NDN_SECURITY_ENCRYPTION_MANAGER_HPP

#include "../../name.hpp"
#include "../security-common.hpp"

namespace ndn {

class EncryptionManager
{
public:
  virtual ~EncryptionManager()
  {
  }

  virtual void
  createSymmetricKey(const Name& keyName, KeyType keyType,
                     const Name& signkeyName = Name(), bool isSymmetric = true) = 0;

  virtual ConstBufferPtr
  encrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool useSymmetric = false,
          EncryptMode encryptMode = ENCRYPT_MODE_DEFAULT) = 0;

  virtual ConstBufferPtr
  decrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool useSymmetric = false,
          EncryptMode encryptMode = ENCRYPT_MODE_DEFAULT) = 0;
};

} // namespace ndn

#endif // NDN_SECURITY_ENCRYPTION_MANAGER_HPP
