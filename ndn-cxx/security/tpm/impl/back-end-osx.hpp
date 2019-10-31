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

#ifndef NDN_SECURITY_TPM_IMPL_BACK_END_OSX_HPP
#define NDN_SECURITY_TPM_IMPL_BACK_END_OSX_HPP

#include "ndn-cxx/security/tpm/back-end.hpp"

#ifndef NDN_CXX_HAVE_OSX_FRAMEWORKS
#error "This file should not be compiled ..."
#endif

#include "ndn-cxx/security/tpm/impl/key-ref-osx.hpp"

namespace ndn {
namespace security {
namespace tpm {

/**
 * @brief The back-end implementation of TPM based on macOS Keychain Services.
 */
class BackEndOsx final : public BackEnd
{
public:
  /**
   * @brief Create TPM backed based on macOS Keychain Services.
   *
   * @param location Not used (required by the TPM registration interface).
   */
  explicit
  BackEndOsx(const std::string& location = "");

  ~BackEndOsx() final;

  static const std::string&
  getScheme();

public: // management
  bool
  isTerminalMode() const final;

  void
  setTerminalMode(bool isTerminal) const final;

  bool
  isTpmLocked() const final;

  bool
  unlockTpm(const char* pw, size_t pwLen) const final;

public: // crypto transformation
  /**
   * @brief Sign @p buf with @p key using @p digestAlgorithm.
   */
  static ConstBufferPtr
  sign(const KeyRefOsx& key, DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t size);

  static ConstBufferPtr
  decrypt(const KeyRefOsx& key, const uint8_t* cipherText, size_t cipherSize);

  static ConstBufferPtr
  derivePublicKey(const KeyRefOsx& key);

private: // inherited from tpm::BackEnd
  bool
  doHasKey(const Name& keyName) const final;

  unique_ptr<KeyHandle>
  doGetKeyHandle(const Name& keyName) const final;

  unique_ptr<KeyHandle>
  doCreateKey(const Name& identityName, const KeyParams& params) final;

  void
  doDeleteKey(const Name& keyName) final;

  ConstBufferPtr
  doExportKey(const Name& keyName, const char* pw, size_t pwLen) final;

  void
  doImportKey(const Name& keyName, const uint8_t* buf, size_t size, const char* pw, size_t pwLen) final;

  void
  doImportKey(const Name& keyName, shared_ptr<transform::PrivateKey> key) final;

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_IMPL_BACK_END_OSX_HPP
