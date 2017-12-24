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

#include "tpm.hpp"
#include "back-end.hpp"
#include "../../encoding/buffer-stream.hpp"

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
  switch (params.getKeyType()) {
    case KeyType::RSA:
    case KeyType::EC: {
      unique_ptr<KeyHandle> keyHandle = m_backEnd->createKey(identityName, params);
      Name keyName = keyHandle->getKeyName();
      m_keys[keyName] = std::move(keyHandle);
      return keyName;
    }
    default: {
      BOOST_THROW_EXCEPTION(Error("Fail to create a key pair: Unsupported key type"));
    }
  }
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

  if (key == nullptr)
    return nullptr;
  else
    return key->derivePublicKey();
}

ConstBufferPtr
Tpm::sign(const uint8_t* buf, size_t size, const Name& keyName, DigestAlgorithm digestAlgorithm) const
{
  const KeyHandle* key = findKey(keyName);

  if (key == nullptr)
    return nullptr;
  else
    return key->sign(digestAlgorithm, buf, size);
}

ConstBufferPtr
Tpm::decrypt(const uint8_t* buf, size_t size, const Name& keyName) const
{
  const KeyHandle* key = findKey(keyName);

  if (key == nullptr)
    return nullptr;
  else
    return key->decrypt(buf, size);
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
Tpm::importPrivateKey(const Name& keyName, const uint8_t* pkcs8, size_t pkcs8Len,
                      const char* pw, size_t pwLen)
{
  m_backEnd->importKey(keyName, pkcs8, pkcs8Len, pw, pwLen);
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
