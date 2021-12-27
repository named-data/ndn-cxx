/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/security/tpm/key-handle.hpp"

namespace ndn {
namespace security {
namespace tpm {

KeyHandle::~KeyHandle() = default;

ConstBufferPtr
KeyHandle::sign(DigestAlgorithm digestAlgorithm, const InputBuffers& bufs) const
{
  return doSign(digestAlgorithm, bufs);
}

bool
KeyHandle::verify(DigestAlgorithm digestAlgorithm, const InputBuffers& bufs,
                  span<const uint8_t> sig) const
{
  return doVerify(digestAlgorithm, bufs, sig);
}

ConstBufferPtr
KeyHandle::decrypt(span<const uint8_t> cipherText) const
{
  return doDecrypt(cipherText);
}

ConstBufferPtr
KeyHandle::derivePublicKey() const
{
  return doDerivePublicKey();
}

} // namespace tpm
} // namespace security
} // namespace ndn
