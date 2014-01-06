/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_ENCRYPTION_MANAGER_HPP
#define NDN_ENCRYPTION_MANAGER_HPP

#include "../../name.hpp"
#include "../security-common.hpp"

namespace ndn {

class EncryptionManager {
public:
  virtual ~EncryptionManager() {}
    
  virtual void 
  createSymmetricKey(const Name& keyName, KeyType keyType, const Name& signkeyName = Name(), bool isSymmetric = true) = 0;

  virtual ConstBufferPtr
  encrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool useSymmetric = false, 
          EncryptMode encryptMode = ENCRYPT_MODE_DEFAULT) = 0;

  virtual ConstBufferPtr
  decrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool useSymmetric = false, 
          EncryptMode encryptMode = ENCRYPT_MODE_DEFAULT) = 0;
};

}

#endif
