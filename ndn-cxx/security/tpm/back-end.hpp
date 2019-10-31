/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_SECURITY_TPM_BACK_END_HPP
#define NDN_SECURITY_TPM_BACK_END_HPP

#include "ndn-cxx/security/tpm/tpm.hpp"

namespace ndn {
namespace security {
namespace tpm {

/**
 * @brief Abstract interface for a TPM backend implementation.
 *
 * This class provides KeyHandle to the front-end and other TPM management operations.
 */
class BackEnd : noncopyable
{
public:
  using Error = Tpm::Error;

  virtual
  ~BackEnd();

public: // key management
  /**
   * @brief Check if the key with name @p keyName exists in the TPM.
   *
   * @return True if the key exists, false otherwise.
   */
  bool
  hasKey(const Name& keyName) const;

  /**
   * @brief Get the handle of the key with name @p keyName.
   *
   * Calling this function multiple times with the same @p keyName will return different KeyHandle
   * objects that all refer to the same key.
   *
   * @return The handle of the key, or nullptr if the key does not exist.
   */
  unique_ptr<KeyHandle>
  getKeyHandle(const Name& keyName) const;

  /**
   * @brief Create a key for @p identityName according to @p params.
   *
   * @return The handle of the created key.
   * @throw std::invalid_argument @p params are invalid.
   * @throw Error The key could not be created.
   */
  unique_ptr<KeyHandle>
  createKey(const Name& identityName, const KeyParams& params);

  /**
   * @brief Delete the key with name @p keyName.
   *
   * @warning Continuing to use existing KeyHandle objects for a deleted key
   *          results in undefined behavior.
   *
   * @throw Error The key could not be deleted.
   */
  void
  deleteKey(const Name& keyName);

  /**
   * @brief Get the private key with name @p keyName in encrypted PKCS #8 format.
   *
   * @param keyName The name of the key.
   * @param pw The password to encrypt the private key.
   * @param pwLen The length of the password.
   *
   * @return The encoded private key.
   * @throw Error The key does not exist or cannot be exported.
   */
  ConstBufferPtr
  exportKey(const Name& keyName, const char* pw, size_t pwLen);

  /**
   * @brief Import a private key in encrypted PKCS #8 format.
   *
   * @param keyName The name of the key to use in the TPM.
   * @param pkcs8 Pointer to the key in encrypted PKCS #8 format.
   * @param pkcs8Len The size of the key in encrypted PKCS #8 format.
   * @param pw The password to decrypt the private key.
   * @param pwLen The length of the password.
   *
   * @throw Error The key could not be imported.
   */
  void
  importKey(const Name& keyName, const uint8_t* pkcs8, size_t pkcs8Len, const char* pw, size_t pwLen);

  /**
   * @brief Import a private key.
   *
   * @throw Error The key could not be imported.
   */
  void
  importKey(const Name& keyName, shared_ptr<transform::PrivateKey> key);

  /**
   * @brief Check if the TPM is in terminal mode.
   *
   * The default implementation always returns true.
   *
   * @return True if in terminal mode, false otherwise.
   */
  virtual bool
  isTerminalMode() const;

  /**
   * @brief Set the terminal mode of the TPM.
   *
   * In terminal mode, the TPM will not ask for a password from the GUI.
   * The default implementation does nothing.
   */
  virtual void
  setTerminalMode(bool isTerminal) const;

  /**
   * @brief Check if the TPM is locked.
   *
   * The default implementation always returns false.
   *
   * @return True if locked, false otherwise.
   */
  virtual bool
  isTpmLocked() const;

  /**
   * @brief Unlock the TPM.
   *
   * The default implementation does nothing and returns `!isTpmLocked()`.
   *
   * @param pw The password to unlock the TPM.
   * @param pwLen The length of the password.
   *
   * @return True if the TPM was unlocked.
   */
  NDN_CXX_NODISCARD virtual bool
  unlockTpm(const char* pw, size_t pwLen) const;

protected: // helper methods
  /**
   * @brief Construct and return the name of a RSA or EC key, based on @p identity and @p params.
   */
  Name
  constructAsymmetricKeyName(const KeyHandle& key, const Name& identity,
                             const KeyParams& params) const;

  /**
   * @brief Construct and return the name of a HMAC key, based on @p identity and @p params.
   */
  Name
  constructHmacKeyName(const transform::PrivateKey& key, const Name& identity,
                       const KeyParams& params) const;

private: // pure virtual methods
  virtual bool
  doHasKey(const Name& keyName) const = 0;

  virtual unique_ptr<KeyHandle>
  doGetKeyHandle(const Name& keyName) const = 0;

  virtual unique_ptr<KeyHandle>
  doCreateKey(const Name& identity, const KeyParams& params) = 0;

  virtual void
  doDeleteKey(const Name& keyName) = 0;

  virtual ConstBufferPtr
  doExportKey(const Name& keyName, const char* pw, size_t pwLen) = 0;

  virtual void
  doImportKey(const Name& keyName, const uint8_t* pkcs8, size_t pkcs8Len, const char* pw, size_t pwLen) = 0;

  virtual void
  doImportKey(const Name& keyName, shared_ptr<transform::PrivateKey> key) = 0;
};

} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_BACK_END_HPP
