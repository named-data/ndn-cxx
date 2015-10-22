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

#ifndef NDN_SECURITY_TPM_BACK_END_HPP
#define NDN_SECURITY_TPM_BACK_END_HPP

#include "../../common.hpp"
#include "../../name.hpp"
#include "../../encoding/buffer.hpp"
#include "../key-params.hpp"

namespace ndn {
namespace security {
namespace tpm {

class KeyHandle;

/**
 * @brief Abstraction of Tpm back-end.
 *
 * This class provides KeyHandle to the front-end and other TPM management operations.
 */
class BackEnd : noncopyable
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
  virtual
  ~BackEnd();

public: // key management
  /**
   * @return True if a key with name @p keyName exists in TPM.
   */
  bool
  hasKey(const Name& keyName) const;

  /**
   * @return The handle of a key with name @p keyName, or nullptr if the key does not exist.
   *
   * Calling getKeyHandle multiple times with the same keyName will return different KeyHandle
   * objects that all refer to the same key.
   */
  unique_ptr<KeyHandle>
  getKeyHandle(const Name& keyName) const;

  /**
   * @brief Create key for @p identity according to @p params.
   *
   * The key name is set in the returned KeyHandle.
   *
   * @return The handle of the created key.
   * @throw Tpm::Error @p params are invalid
   * @throw Error the key cannot be created
   */
  unique_ptr<KeyHandle>
  createKey(const Name& identity, const KeyParams& params);

  /**
   * @brief Delete a key with name @p keyName.
   *
   * Continuing to use existing KeyHandles on a deleted key results in undefined behavior.
   *
   * @throw Error if the deletion fails.
   */
  void
  deleteKey(const Name& keyName);

  /**
   * @return A private key with name @p keyName in encrypted PKCS #8 format using password @p pw
   * @throw Error the key does not exist
   * @throw Error the key cannot be exported, e.g., insufficient privilege
   */
  ConstBufferPtr
  exportKey(const Name& keyName, const char* pw, size_t pwLen);

  /**
   * @brief Import a private key in encrypted PKCS #8 format
   *
   * @param keyName The name of imported private key
   * @param pkcs8 Pointer to the key in encrypted PKCS #8 format
   * @param pkcs8Len The size of the key in encrypted PKCS #8 format
   * @param pw The password to decrypt the private key
   * @param pwLen The length of the password
   * @throw Error import failed
   */
  void
  importKey(const Name& keyName, const uint8_t* pkcs8, size_t pkcs8Len, const char* pw, size_t pwLen);

  /**
   * @brief Check if TPM is in terminal mode
   *
   * Default implementation always returns true.
   */
  virtual bool
  isTerminalMode() const;

  /**
   * @brief Set the terminal mode of TPM.
   *
   * In terminal mode, TPM will not ask user permission from GUI.
   *
   * Default implementation does nothing.
   */
  virtual void
  setTerminalMode(bool isTerminal) const;

  /**
   * @return True if TPM is locked, otherwise false
   *
   * Default implementation always returns false.
   */
  virtual bool
  isTpmLocked() const;

  /**
   * @brief Unlock TPM
   *
   * @param pw    The password to unlock TPM
   * @param pwLen The password size.
   *
   * Default implementation always returns !isTpmLocked()
   */
  virtual bool
  unlockTpm(const char* pw, size_t pwLen) const;

protected: // static helper method
  /**
   * @brief Set the key name in @p keyHandle according to @p identity and @p params
   */
  static void
  setKeyName(KeyHandle& keyHandle, const Name& identity, const KeyParams& params);

private: // pure virtual methods
  /**
   * @return True if a key with name @p keyName exists in TPM.
   */
  virtual bool
  doHasKey(const Name& keyName) const = 0;

  /**
   * @return The handle of a key with name @p keyName, or nullptr if the key does not exist
   */
  virtual unique_ptr<KeyHandle>
  doGetKeyHandle(const Name& keyName) const = 0;

  /**
   * @brief Create key for @p identityName according to @p params.
   *
   * The created key is named as: /<identityName>/[keyId]/KEY
   * The key name is set in the returned KeyHandle.
   *
   * @return The handle of the created key.
   * @throw Error key cannot be created
   */
  virtual unique_ptr<KeyHandle>
  doCreateKey(const Name& identity, const KeyParams& params) = 0;

  /**
   * @brief Delete a key with name @p keyName.
   *
   * @throw Error the deletion failed
   */
  virtual void
  doDeleteKey(const Name& keyName) = 0;

  /**
   * @return A private key with name @p keyName in encrypted PKCS #8 format using password @p pw
   * @throw Error the key cannot be exported, e.g., insufficient privilege
   */
  virtual ConstBufferPtr
  doExportKey(const Name& keyName, const char* pw, size_t pwLen) = 0;

  /**
   * @brief Import a private key in encrypted PKCS #8 format using @p password
   *
   * @param keyName The name of imported private key
   * @param pkcs8 Pointer to the key in PKCS #8 format
   * @param pkcs8Len The size of the key in PKCS #8 format
   * @param pw The password to decrypt the private key
   * @param pwLen The length of the password
   * @throw Error import failed
   */
  virtual void
  doImportKey(const Name& keyName, const uint8_t* pkcs8, size_t pkcs8Len, const char* pw, size_t pwLen) = 0;
};

} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_BACK_END_HPP
