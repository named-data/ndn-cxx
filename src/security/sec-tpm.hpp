/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_SECURITY_SEC_TPM_HPP
#define NDN_SECURITY_SEC_TPM_HPP

#include "../common.hpp"
#include "security-common.hpp"
#include "../name.hpp"
#include "../data.hpp"
#include "public-key.hpp"
#include "key-params.hpp"

namespace ndn {

/**
 * @brief SecTpm is the base class of the TPM classes.
 *
 * It specifies the interfaces of private/secret key related operations.
 */
class SecTpm : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  explicit
  SecTpm(const std::string& location);

  virtual
  ~SecTpm();

  std::string
  getTpmLocator();

  /**
   * @brief set password of TPM
   *
   * Password is used to unlock TPM when it is locked.
   * You should be cautious when using this method, because remembering password is kind of
   * dangerous.
   *
   * @param password The password
   * @param passwordLength The length of password
   */
  virtual void
  setTpmPassword(const uint8_t* password, size_t passwordLength) = 0;

  /**
   * @brief reset password of TPM
   */
  virtual void
  resetTpmPassword() = 0;

  /**
   * @brief Set inTerminal flag to @param inTerminal
   *
   * If the inTerminal flag is set, and password is not set, TPM may ask for password via terminal.
   * inTerminal flag is set by default.
   */
  virtual void
  setInTerminal(bool inTerminal) = 0;

  /**
   * @brief Get value of inTerminal flag
   */
  virtual bool
  getInTerminal() const = 0;

  /**
   * @brief Check if TPM is locked
   */
  virtual bool
  isLocked() = 0;

  /**
   * @brief Unlock the TPM
   *
   * @param password The password.
   * @param passwordLength The password size. 0 indicates no password.
   * @param usePassword True if we want to use the supplied password to unlock the TPM.
   * @return true if TPM is unlocked, otherwise false.
   */
  virtual bool
  unlockTpm(const char* password, size_t passwordLength, bool usePassword) = 0;

  /**
   * @brief Generate a pair of asymmetric keys.
   *
   * @param keyName The name of the key pair.
   * @param params The parameters of key.
   * @throws SecTpm::Error if fails.
   */
  virtual void
  generateKeyPairInTpm(const Name& keyName, const KeyParams& params) = 0;

  /**
   * @brief Delete a key pair of asymmetric keys.
   *
   * @param keyName The name of the key pair.
   */
  virtual void
  deleteKeyPairInTpm(const Name& keyName) = 0;

  /**
   * @brief Get a public key.
   *
   * @param keyName The public key name.
   * @return The public key.
   * @throws SecTpm::Error if public key does not exist in TPM.
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
  signInTpm(const uint8_t* data, size_t dataLength,
            const Name& keyName,
            DigestAlgorithm digestAlgorithm) = 0;

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
   * @param params The parameter of the key.
   * @throws SecTpm::Error if key generating fails.
   */
  virtual void
  generateSymmetricKeyInTpm(const Name& keyName, const KeyParams& params) = 0;

  /**
   * @brief Check if a particular key exists.
   *
   * @param keyName The name of the key.
   * @param keyClass The class of the key, e.g. KEY_CLASS_PUBLIC, KEY_CLASS_PRIVATE.
   * @return True if the key exists, otherwise false.
   */
  virtual bool
  doesKeyExistInTpm(const Name& keyName, KeyClass keyClass) = 0;

  /**
   * @brief Generate a random block
   *
   * @param res The pointer to the generated block
   * @param size The random block size
   * @return true for success, otherwise false
   */
  virtual bool
  generateRandomBlock(uint8_t* res, size_t size) = 0;

  /**
   * @brief Add the application into the ACL of a particular key
   *
   * @param keyName the name of key
   * @param keyClass the class of key, e.g. Private Key
   * @param appPath the absolute path to the application
   * @param acl the new acl of the key
   */
  virtual void
  addAppToAcl(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl) = 0;

  /**
   * @brief Export a private key in PKCS#5 format
   *
   * @param keyName  The private key name
   * @param password The password to encrypt the private key
   * @return The private key info (in PKCS8 format) if exist
   * @throws SecTpm::Error if private key cannot be exported
   */
  ConstBufferPtr
  exportPrivateKeyPkcs5FromTpm(const Name& keyName, const std::string& password);

  /**
   * @brief Import a private key in PKCS#5 formatted @param buffer of size @param bufferSize
   *
   * Also recover the public key and installed it in TPM.
   *
   * @param keyName    The private key name
   * @param buffer     Pointer to the first byte of the buffer containing PKCS#5-encoded
   *                   private key info
   * @param bufferSize Size of the buffer
   * @param password   The password to encrypt the private key
   * @return false if import fails
   */
  bool
  importPrivateKeyPkcs5IntoTpm(const Name& keyName,
                               const uint8_t* buffer, size_t bufferSize,
                               const std::string& password);

protected:
  virtual std::string
  getScheme() = 0;

  /**
   * @brief Export a private key in PKCS#8 format.
   *
   * @param keyName The private key name.
   * @return The private key info (in PKCS#8 format) if exist, otherwise a NULL pointer.
   */
  virtual ConstBufferPtr
  exportPrivateKeyPkcs8FromTpm(const Name& keyName) = 0;

  /**
   * @brief Import a private key from PKCS#8 formatted @param buffer of size @param bufferSize
   *
   * @param keyName    The private key name.
   * @param buffer     Pointer to the first byte of the buffer containing PKCS#8-encoded
   *                   private key info
   * @param bufferSize Size of the buffer
   * @return false if import fails
   */
  virtual bool
  importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buffer, size_t bufferSize) = 0;

  /**
   * @brief Import a public key in PKCS#1 formatted @param buffer of size @param bufferSize
   *
   * @param keyName    The public key name
   * @param buffer     Pointer to the first byte of the buffer containing PKCS#1-encoded
   *                   private key info
   * @param bufferSize Size of the buffer
   * @return false if import fails
   */
  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buffer, size_t bufferSize) = 0;

  /**
   * @brief Get import/export password.
   *
   * @param password On return, the password.
   * @param prompt Prompt for password, i.e., "Password for key:"
   * @return true if password has been obtained.
   */
  virtual bool
  getImpExpPassWord(std::string& password, const std::string& prompt);

protected:
  std::string m_location;
};

} // namespace ndn

#endif // NDN_SECURITY_SEC_TPM_HPP
