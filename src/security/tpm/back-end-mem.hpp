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

#ifndef NDN_SECURITY_TPM_BACK_END_MEM_HPP
#define NDN_SECURITY_TPM_BACK_END_MEM_HPP

#include "back-end.hpp"

namespace ndn {
namespace security {
namespace tpm {

/**
 * @brief The back-end implementation of in-memory TPM.
 */
class BackEndMem : public BackEnd
{
public:
  class Error : public BackEnd::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : BackEnd::Error(what)
    {
    }
  };

public:
  /**
   * @brief Create memory-based TPM backend
   * @param location Not used (required by the TPM-registration interface)
   */
  explicit
  BackEndMem(const std::string& location = "");

  ~BackEndMem() override;

  static const std::string&
  getScheme();

private: // inherited from tpm::BackEnd

  /**
   * @return True if a key with name @p keyName exists in TPM.
   */
  bool
  doHasKey(const Name& keyName) const final;

  /**
   * @return The handle of a key with name @p keyName, or nullptr if the key does not exist
   */
  unique_ptr<KeyHandle>
  doGetKeyHandle(const Name& keyName) const final;

  /**
   * @brief Create key for @p identityName according to @p params.
   *
   * The created key is named as: /<identityName>/[keyId]/KEY
   * The key name is set in the returned KeyHandle.
   * If the key with the same name is created, the old one will be removed.
   * The behavior of using KeyHandler of removed key is undefined.
   *
   * @return The handle of the created key.
   */
  unique_ptr<KeyHandle>
  doCreateKey(const Name& identityName, const KeyParams& params) final;

  /**
   * @brief Delete a key with name @p keyName.
   *
   * @throw Error the deletion failed
   */
  void
  doDeleteKey(const Name& keyName) final;

  /**
   * @return A private key with name @p keyName in encrypted PKCS #8 format using password @p pw
   * @throw Error the key cannot be exported, e.g., not enough privilege
   */
  ConstBufferPtr
  doExportKey(const Name& keyName, const char* pw, size_t pwLen) final;

  /**
   * @brief Import a private key in encrypted PKCS #8 format
   *
   * @param keyName The name of imported private key
   * @param buf Pointer to the key in encrypted PKCS #8 format
   * @param size The size of the key in encrypted PKCS #8 format
   * @param pw The password to decrypt the key
   * @param pwLen The length of password
   * @throw Error import failed
   */
  void
  doImportKey(const Name& keyName, const uint8_t* buf, size_t size, const char* pw, size_t pwLen) final;

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_BACK_END_MEM_HPP
