/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "key-handle-osx.hpp"
#include "back-end-osx.hpp"

namespace ndn {
namespace security {
namespace tpm {

KeyHandleOsx::KeyHandleOsx(const KeyRefOsx& key)
  : m_key(key)
{
  if (m_key.get() == 0)
    BOOST_THROW_EXCEPTION(Error("key is not set"));
}

ConstBufferPtr
KeyHandleOsx::doSign(DigestAlgorithm digestAlgorithm, const uint8_t* buf, size_t size) const
{
  return BackEndOsx::sign(m_key, digestAlgorithm, buf, size);
}

ConstBufferPtr
KeyHandleOsx::doDecrypt(const uint8_t* cipherText, size_t cipherTextLen) const
{
  return BackEndOsx::decrypt(m_key, cipherText, cipherTextLen);
}

ConstBufferPtr
KeyHandleOsx::doDerivePublicKey() const
{
  return BackEndOsx::derivePublicKey(m_key);
}

} // namespace tpm
} // namespace security
} // namespace ndn
