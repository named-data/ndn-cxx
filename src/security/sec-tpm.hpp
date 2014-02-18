/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_TPM_HPP
#define NDN_SECURITY_SEC_TPM_HPP

#include "../common.hpp"
#include "security-common.hpp"
#include "../name.hpp"
#include "../data.hpp"
#include "public-key.hpp"

namespace ndn {

/**
 * @brief SecTpm is the base class of the TPM classes.
 *
 * It specifies the interfaces of private/secret key related operations.
 */
class SecTpm {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * The virtual destructor.
   */    
  virtual 
  ~SecTpm() {}

  /**
   * @brief Generate a pair of asymmetric keys.
   *
   * @param keyName The name of the key pair.
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   * @throws SecTpm::Error if fails.
   */
  virtual void 
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize) = 0;
  
  /**
   * @brief Delete a key pair of asymmetric keys.
   *
   * @param keyName The name of the key pair.
   */
  virtual void
  deleteKeyPairInTpm(const Name &keyName) = 0;

  /**
   * @brief Get a public key.
   *
   * @param keyName The public key name.
   * @return The public key if exists, otherwise a NULL pointer.
   */
  virtual shared_ptr<PublicKey> 
  getPublicKeyFromTpm(const Name& keyName) = 0;
  
  /**
   * @brief Sign data.
   *
   * @param data Pointer to the byte array to be signed.
   * @param dataLength The length of data.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @return The signature block.
   * @throws SecTpm::Error if signing fails.
   */  
  virtual Block
  signInTpm(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm) = 0;
  
  /**
   * @brief Decrypt data.
   *
   * @param data Pointer to the byte arry to be decrypted.
   * @param dataLength The length of data.
   * @param keyName The name of the decrypting key.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric encryption.
   * @return The decrypted data.
   * @throws SecTpm::Error if decryption fails.
   */
  virtual ConstBufferPtr 
  decryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric) = 0;

  /**
   * @brief Encrypt data.
   *
   * @param data Pointer to the byte arry to be decrypted.
   * @param dataLength The length of data.
   * @param keyName The name of the encrypting key.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric encryption.
   * @return The encrypted data.
   * @throws SecTpm::Error if encryption fails.
   */
  virtual ConstBufferPtr
  encryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric) = 0;

  /**
   * @brief Generate a symmetric key.
   *
   * @param keyName The name of the key.
   * @param keyType The type of the key, e.g. KEY_TYPE_AES.
   * @param keySize The size of the key.
   */
  virtual void 
  generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize) = 0;

  /**
   * @brief Check if a particular key exists.
   *
   * @param keyName The name of the key.
   * @param keyClass The class of the key, e.g. KEY_CLASS_PUBLIC, KEY_CLASS_PRIVATE, or KEY_CLASS_SYMMETRIC.
   * @return True if the key exists, otherwise false.
   */
  virtual bool
  doesKeyExistInTpm(const Name& keyName, KeyClass keyClass) = 0;  

  /**
   * @brief Generate a random block.
   * 
   * @param res The pointer to the generated block.
   * @param size The random block size.
   * @return true for success, otherwise false.
   */
  virtual bool
  generateRandomBlock(uint8_t* res, size_t size) = 0;

  /**
   * @brief Export a private key in PKCS#8 format.
   * 
   * @param keyName The private key name.
   * @param password The password to encrypt the private key.
   * @param inTerminal If password is not supplied, get it via terminal if inTerminal is true, otherwise fail.
   * @return The private key info (in PKCS8 format) if exist, otherwise a NULL pointer.
   */
  ConstBufferPtr
  exportPrivateKeyPkcs8FromTpm(const Name& keyName, bool inTerminal, const std::string& password);

  /**
   * @brief Import a private key in PKCS#8 format.
   * 
   * Also recover the public key and installed it in TPM.
   * 
   * @param keyName The private key name.
   * @param key The encoded private key info.
   * @param password The password to encrypt the private key.
   * @param inTerminal If password is not supplied, get it via terminal if inTerminal is true, otherwise fail.
   * @return False if import fails.
   */
  bool
  importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buf, size_t size, bool inTerminal, const std::string& password);

protected:
  /**
   * @brief Export a private key in PKCS#1 format.
   * 
   * @param keyName The private key name.
   * @return The private key info (in PKCS#1 format) if exist, otherwise a NULL pointer.
   */
  virtual ConstBufferPtr
  exportPrivateKeyPkcs1FromTpm(const Name& keyName) = 0;

  /**
   * @brief Import a private key in PKCS#1 format.
   * 
   * @param keyName The private key name.
   * @param key The encoded private key info.
   * @return False if import fails.
   */
  virtual bool
  importPrivateKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size) = 0;

  /**
   * @brief Import a public key in PKCS#1 format.
   * 
   * @param keyName The public key name.
   * @param key The encoded public key info.
   * @return False if import fails.
   */
  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size) = 0;


  /**
   * @brief Get password.
   *
   * @param password On return, the password.
   * @param prompt Prompt for password, i.e., "Password for key:"
   * @return true if password has been obtained.
   */
  inline virtual bool
  getPassWord(std::string& password, const std::string& prompt);
};

bool
SecTpm::getPassWord(std::string& password, const std::string& prompt)
{
  int result = false;

  char* pw0 = NULL;
  
  pw0 = getpass(prompt.c_str());
  if(!pw0) 
    return false;
  std::string password1 = pw0;
  memset(pw0, 0, strlen(pw0));

  pw0 = getpass("Confirm:");
  if(!pw0)
    {
      char* pw1 = const_cast<char*>(password1.c_str());
      memset(pw1, 0, password1.size());
      return false;
    }

  if(!password1.compare(pw0))
    {
      result = true;
      password.swap(password1);
    }

  char* pw1 = const_cast<char*>(password1.c_str());
  memset(pw1, 0, password1.size());
  memset(pw0, 0, strlen(pw0));  
  return result;
}

} // namespace ndn

#endif //NDN_SECURITY_SEC_TPM_HPP
