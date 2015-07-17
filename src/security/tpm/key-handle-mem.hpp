/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_SECURITY_TPM_KEY_HANDLE_MEM_HPP
#define NDN_SECURITY_TPM_KEY_HANDLE_MEM_HPP

#include "key-handle.hpp"

namespace ndn {
namespace security {

namespace transform {
class PrivateKey;
} // namespace transform

namespace tpm {

/**
 * @brief A TPM key handle that keeps the private key in memory
 */
class KeyHandleMem : public KeyHandle
{
public:
  explicit
  KeyHandleMem(shared_ptr<transform::PrivateKey> key);

private:
  ConstBufferPtr
  doSign(DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t size) const final;

  ConstBufferPtr
  doDecrypt(const uint8_t* cipherText, size_t cipherTextLen) const final;

  ConstBufferPtr
  doDerivePublicKey() const final;

private:
  shared_ptr<transform::PrivateKey> m_key;
};

} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_TPM_KEY_HANDLE_MEM_HPP
