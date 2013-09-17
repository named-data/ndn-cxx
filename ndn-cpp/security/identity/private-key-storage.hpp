/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_PRIVATE_KEY_STORAGE_HPP
#define	NDN_PRIVATE_KEY_STORAGE_HPP

#include <string>
#include "../../util/blob.hpp"
#include "../security-common.hpp"

namespace ndn {

class PrivateKeyStorage {
public:
  /**
   * The virtual destructor
   */    
  virtual 
  ~PrivateKeyStorage();

#if 0
  /**
   * @brief generate a pair of asymmetric keys
   * @param keyName the name of the key pair
   * @param keyType the type of the key pair, e.g. RSA
   * @param keySize the size of the key pair
   */
  virtual void 
  generateKeyPair(const string & keyName, KeyType keyType = KEY_TYPE_RSA, int keySize = 2048) = 0;

  /**
   * @brief get the public key
   * @param keyName the name of public key
   * @return the public key
   */
  virtual Ptr<Publickey> 
  getPublickey(const string & keyName) = 0;
#endif
  
  /**
   * Fetch the private key for keyName and sign the data, returning a signature Blob.
   * @param data Pointer to the input byte array.
   * @param dataLength The length of data.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @return The signature, or 0 if signing fails.
   */  
  virtual Blob 
  sign(const unsigned char *data, unsigned int dataLength, const std::string& keyName, DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256);
    
#if 0
  /**
   * @brief decrypt data
   * @param keyName the name of the decrypting key
   * @param blob the blob to be decrypted
   * @param sym if true symmetric encryption is used, otherwise asymmetric decryption is used.
   * @return decrypted data
   */
  virtual Ptr<Blob> 
  decrypt(const string & keyName, const Blob & data, bool sym = false) = 0;

  /**
   * @brief encrypt data
   * @param keyName the name of the encrypting key
   * @param blob the blob to be encrypted
   * @param sym if true symmetric encryption is used, otherwise asymmetric decryption is used.
   * @return encrypted data
   */
  virtual Ptr<Blob> 
  encrypt(const string & keyName, const Blob & pData, bool sym = false) = 0;

  /**
   * @brief generate a symmetric key
   * @param keyName the name of the key 
   * @param keyType the type of the key, e.g. AES
   * @param keySize the size of the key
   */
  virtual void 
  generateKey(const string & keyName, KeyType keyType = KEY_TYPE_AES, int keySize = 256) = 0;

  /**
   * @brief check if a particular key exist
   * @param keyName the name of the key
   * @param keyClass the class of the key, e.g. public, private, or symmetric
   * @return true if the key exists, otherwise false
   */
  virtual bool
  doesKeyExist(const string & keyName, KeyClass keyClass) = 0;  
#endif
};

}

#endif
