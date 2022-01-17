/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/tpm/tpm.hpp"
#include "ndn-cxx/security/tpm/back-end.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace tpm {

Tpm::Tpm(const std::string& scheme, const std::string& location, unique_ptr<BackEnd> backEnd)
  : m_scheme(scheme)
  , m_location(location)
  , m_backEnd(std::move(backEnd))
{
}

Tpm::~Tpm() = default;

std::string
Tpm::getTpmLocator() const
{
  return m_scheme + ":" + m_location;
}

bool
Tpm::hasKey(const Name& keyName) const
{
  return m_backEnd->hasKey(keyName);
}

Name
Tpm::createKey(const Name& identityName, const KeyParams& params)
{
  auto keyHandle = m_backEnd->createKey(identityName, params);
  auto keyName = keyHandle->getKeyName();
  m_keys[keyName] = std::move(keyHandle);
  return keyName;
}

void
Tpm::deleteKey(const Name& keyName)
{
  auto it = m_keys.find(keyName);
  if (it != m_keys.end())
    m_keys.erase(it);

  m_backEnd->deleteKey(keyName);
}

ConstBufferPtr
Tpm::getPublicKey(const Name& keyName) const
{
  const KeyHandle* key = findKey(keyName);
  return key ? key->derivePublicKey() : nullptr;
}

ConstBufferPtr
Tpm::sign(const InputBuffers& bufs, const Name& keyName, DigestAlgorithm digestAlgorithm) const
{
  const KeyHandle* key = findKey(keyName);
  return key ? key->sign(digestAlgorithm, bufs) : nullptr;
}

boost::logic::tribool
Tpm::verify(const InputBuffers& bufs, span<const uint8_t> sig, const Name& keyName,
            DigestAlgorithm digestAlgorithm) const
{
  const KeyHandle* key = findKey(keyName);
  if (key == nullptr)
    return boost::logic::indeterminate;

  return key->verify(digestAlgorithm, bufs, sig);
}

ConstBufferPtr
Tpm::decrypt(span<const uint8_t> buf, const Name& keyName) const
{
  const KeyHandle* key = findKey(keyName);
  return key ? key->decrypt(buf) : nullptr;
}

bool
Tpm::isTerminalMode() const
{
  return m_backEnd->isTerminalMode();
}

void
Tpm::setTerminalMode(bool isTerminal) const
{
  m_backEnd->setTerminalMode(isTerminal);
}

bool
Tpm::isTpmLocked() const
{
  return m_backEnd->isTpmLocked();
}

bool
Tpm::unlockTpm(const char* password, size_t passwordLength) const
{
  return m_backEnd->unlockTpm(password, passwordLength);
}

ConstBufferPtr
Tpm::exportPrivateKey(const Name& keyName, const char* pw, size_t pwLen) const
{
  return m_backEnd->exportKey(keyName, pw, pwLen);
}

void
Tpm::importPrivateKey(const Name& keyName, span<const uint8_t> pkcs8, const char* pw, size_t pwLen)
{
  m_backEnd->importKey(keyName, pkcs8, pw, pwLen);
}

void
Tpm::importPrivateKey(const Name& keyName, shared_ptr<transform::PrivateKey> key)
{
  m_backEnd->importKey(keyName, std::move(key));
}

const KeyHandle*
Tpm::findKey(const Name& keyName) const
{
  auto it = m_keys.find(keyName);
  if (it != m_keys.end())
    return it->second.get();

  auto handle = m_backEnd->getKeyHandle(keyName);
  if (handle == nullptr)
    return nullptr;

  const KeyHandle* key = handle.get();
  m_keys[keyName] = std::move(handle);
  return key;
}

} // namespace tpm
} // namespace security
} // namespace ndn
