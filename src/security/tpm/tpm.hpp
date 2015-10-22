/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_SECURITY_TPM_TPM_HPP
#define NDN_SECURITY_TPM_TPM_HPP

#include "../../common.hpp"
#include "../security-common.hpp"
#include "../../name.hpp"
#include "../key-params.hpp"
#include "key-handle.hpp"
#include <unordered_map>

namespace ndn {
namespace security {

namespace v2 {
class KeyChain;
} // namespace v2

namespace tpm {

class BackEnd;

/**
 * @brief represents the front-end of TPM
 *
 * The TPM (Trusted Platform Module) stores the private portion of a user's cryptography keys.
 * The format and location of stored information is indicated by the TpmLocator.
 * The TPM is designed to work with a PIB (Public Information Base) which stores public keys and
 * related information such as certificate.
 *
 * The TPM also provides functionalities of crypto transformation, such as signing and decryption.
 *
 * A TPM consists of a unified front-end interface and a back-end implementation. The front-end
 * cache the handles of private keys which is provided by the back-end implementation.
 *
 * @note Tpm instance is created and managed only by v2::KeyChain.  v2::KeyChain::getTpm()
 *       returns a const reference to the managed Tpm instance, through which it is possible to
 *       check existence of private keys, get public keys for the private keys, sign, and decrypt
 *       the supplied buffers using managed private keys.
 *
 * @throw BackEnd::Error Failure with the underlying implementation having non-semantic errors
 * @throw Tpm::Error Failure with semantic error in the underlying implementation
 */
class Tpm : noncopyable
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

public:
  ~Tpm();

  std::string
  getTpmLocator() const;

  /**
   * @brief Check if a private key exist
   *
   * @param keyName The key name
   * @return true if the key exists
   */
  bool
  hasKey(const Name& keyName) const;

  /**
   * @return The public portion of an asymmetric key with name @p name
   *         or nullptr if the key does not exist
   *
   * The public key is in PKCS#8 format
   */
  ConstBufferPtr
  getPublicKey(const Name& keyName) const;

  /**
   * @brief Sign blob using key with name @p keyName with digest @p digestAlgorithm.
   *
   * @return The signature, or nullptr if the key does not exist
   */
  ConstBufferPtr
  sign(const uint8_t* buf, size_t size, const Name& keyName, DigestAlgorithm digestAlgorithm) const;

  /**
   * @brief Decrypt blob using key with name @p keyName.
   *
   * @return The signature, or nullptr if the key does not exist
   */
  ConstBufferPtr
  decrypt(const uint8_t* buf, size_t size, const Name& keyName) const;

public: // Management
  /**
   * @brief Check if TPM is in terminal mode
   */
  bool
  isTerminalMode() const;

  /**
   * @brief Set the terminal mode of TPM.
   *
   * In terminal mode, TPM will not ask user permission from GUI.
   */
  void
  setTerminalMode(bool isTerminal) const;

  /**
   * @return True if TPM is locked, otherwise false
   */
  bool
  isTpmLocked() const;

  /**
   * @brief Unlock TPM
   *
   * @param password       The password to unlock TPM
   * @param passwordLength The password size.
   */
  bool
  unlockTpm(const char* password, size_t passwordLength) const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /*
   * @brief Create a new TPM instance with the specified @p location
   *
   * @param scheme The scheme for the TPM
   * @param location The location for the TPM
   * @param impl The back-end implementation
   */
  Tpm(const std::string& scheme, const std::string& location, unique_ptr<BackEnd> impl);

  BackEnd*
  getBackEnd()
  {
    return m_backEnd.get();
  }

  /**
   * @brief Create key for @p identityName according to @p params.
   *
   * The created key is named as: /<identityName>/[keyId]/KEY
   *
   * @return the key name
   * @throw Tpm::Error the key has already existed or the params is invalid
   */
  Name
  createKey(const Name& identityName, const KeyParams& params);

  /**
   * @brief Delete a key pair with name @p keyName.
   */
  void
  deleteKey(const Name& keyName);

  /**
   * @brief Export a private key
   *
   * This method will export the private key in encrypted PKCS #8 format if the key exists.
   *
   * @param keyName  The private key name
   * @param pw The password to encrypt the private key
   * @param pwLen The length of the password
   * @return The encoded private key wrapper or an empty block if the key cannot be exported.
   */
  ConstBufferPtr
  exportPrivateKey(const Name& keyName, const char* pw, size_t pwLen);

  /**
   * @brief Import a private key
   *
   * @param keyName The private key name
   * @param pkcs8 The private key wrapper
   * @param pkcs8Len The length of the private key wrapper
   * @param pw The password to encrypt the private key
   * @param pwLen The length of the password
   * @return false if importing fails
   */
  bool
  importPrivateKey(const Name& keyName,
                   const uint8_t* pkcs8, size_t pkcs8Len,
                   const char* pw, size_t pwLen);

  /**
   * @brief Clear the key cache
   *
   * An empty cache can force Tpm to do key lookup in back-end.
   */
  void
  clearKeyCache()
  {
    m_keys.clear();
  }

private:
  /**
   * @brief Internal KeyHandle lookup
   *
   * @return A pointer to the handle of key @p keyName if it exists, otherwise nullptr.
   */
  const KeyHandle*
  findKey(const Name& keyName) const;

private:
  std::string m_scheme;
  std::string m_location;

  mutable std::unordered_map<Name, unique_ptr<KeyHandle>> m_keys;

  unique_ptr<BackEnd> m_backEnd;

  friend class v2::KeyChain;
};

} // namespace tpm

using tpm::Tpm;

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_TPM_HPP
