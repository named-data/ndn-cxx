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

#ifndef NDN_SECURITY_TPM_KEY_HANDLE_HPP
#define NDN_SECURITY_TPM_KEY_HANDLE_HPP

#include "ndn-cxx/name.hpp"
#include "ndn-cxx/security/security-common.hpp"

namespace ndn {
namespace security {
namespace tpm {

/**
 * @brief Abstraction of TPM key handle.
 *
 * KeyHandle provides an interface to perform crypto operations with a key stored in the TPM.
 */
class KeyHandle : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

public:
  virtual
  ~KeyHandle();

  /**
   * @return a digital signature created on @p buf using this key with @p digestAlgorithm.
   */
  ConstBufferPtr
  sign(DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t size) const;

  /**
   * @brief Verify the signature @p sig created on @p buf using this key and @p digestAlgorithm.
   */
  bool
  verify(DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t bufLen,
         const uint8_t* sig, size_t sigLen) const;

  /**
   * @return plain text content decrypted from @p cipherText using this key.
   */
  ConstBufferPtr
  decrypt(const uint8_t* cipherText, size_t cipherTextLen) const;

  /**
   * @return the PCKS#8 encoded public key bits derived from this key.
   */
  ConstBufferPtr
  derivePublicKey() const;

  Name
  getKeyName() const
  {
    return m_keyName;
  }

  void
  setKeyName(const Name& keyName)
  {
    m_keyName = keyName;
  }

private:
  virtual ConstBufferPtr
  doSign(DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t size) const = 0;

  virtual bool
  doVerify(DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t bufLen,
           const uint8_t* sig, size_t sigLen) const = 0;

  virtual ConstBufferPtr
  doDecrypt(const uint8_t* cipherText, size_t cipherTextLen) const = 0;

  virtual ConstBufferPtr
  doDerivePublicKey() const = 0;

private:
  Name m_keyName;
};

} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_KEY_HANDLE_HPP
