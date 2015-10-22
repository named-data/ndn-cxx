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

#include "pib-memory.hpp"
#include "pib.hpp"
#include "../security-common.hpp"

namespace ndn {
namespace security {
namespace pib {

PibMemory::PibMemory(const std::string&)
  : m_hasDefaultIdentity(false)
{
}

const std::string&
PibMemory::getScheme()
{
  static std::string scheme = "pib-memory";
  return scheme;
}

void
PibMemory::setTpmLocator(const std::string& tpmLocator)
{
  m_tpmLocator = tpmLocator;
}

std::string
PibMemory::getTpmLocator() const
{
  return m_tpmLocator;
}

bool
PibMemory::hasIdentity(const Name& identity) const
{
  return (m_identities.count(identity) > 0);
}

void
PibMemory::addIdentity(const Name& identity)
{
  m_identities.insert(identity);

  if (!m_hasDefaultIdentity) {
    m_defaultIdentity = identity;
    m_hasDefaultIdentity = true;
  }
}

void
PibMemory::removeIdentity(const Name& identity)
{
  m_identities.erase(identity);
  if (identity == m_defaultIdentity) {
    m_hasDefaultIdentity = false;
    m_defaultIdentity.clear();
  }

  auto keyNames = getKeysOfIdentity(identity);
  for (const Name& keyName : keyNames) {
    removeKey(keyName);
  }
}

void
PibMemory::clearIdentities()
{
  m_hasDefaultIdentity = false;
  m_defaultIdentity.clear();
  m_identities.clear();
  m_defaultKeys.clear();
  m_keys.clear();
  m_defaultCerts.clear();
  m_certs.clear();
}

std::set<Name>
PibMemory::getIdentities() const
{
  return m_identities;
}

void
PibMemory::setDefaultIdentity(const Name& identityName)
{
  addIdentity(identityName);
  m_defaultIdentity = identityName;
  m_hasDefaultIdentity = true;
}

Name
PibMemory::getDefaultIdentity() const
{
  if (m_hasDefaultIdentity) {
    return m_defaultIdentity;
  }

  BOOST_THROW_EXCEPTION(Pib::Error("No default identity"));
}

bool
PibMemory::hasKey(const Name& keyName) const
{
  return (m_keys.count(keyName) > 0);
}

void
PibMemory::addKey(const Name& identity, const Name& keyName,
                  const uint8_t* key, size_t keyLen)
{
  addIdentity(identity);

  m_keys[keyName] = Buffer(key, keyLen);

  if (m_defaultKeys.count(identity) == 0) {
    m_defaultKeys[identity] = keyName;
  }
}

void
PibMemory::removeKey(const Name& keyName)
{
  Name identity = v2::extractIdentityFromKeyName(keyName);

  m_keys.erase(keyName);
  m_defaultKeys.erase(identity);

  auto certNames = getCertificatesOfKey(keyName);
  for (const auto& certName : certNames) {
    removeCertificate(certName);
  }
}

Buffer
PibMemory::getKeyBits(const Name& keyName) const
{
  if (!hasKey(keyName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Key `" + keyName.toUri() + "` not found"));
  }

  auto key = m_keys.find(keyName);
  BOOST_ASSERT(key != m_keys.end());
  return key->second;
}

std::set<Name>
PibMemory::getKeysOfIdentity(const Name& identity) const
{
  std::set<Name> ids;
  for (const auto& key : m_keys) {
    if (identity == v2::extractIdentityFromKeyName(key.first)) {
      ids.insert(key.first);
    }
  }
  return ids;
}

void
PibMemory::setDefaultKeyOfIdentity(const Name& identity, const Name& keyName)
{
  if (!hasKey(keyName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Key `" + keyName.toUri() + "` not found"));
  }

  m_defaultKeys[identity] = keyName;
}

Name
PibMemory::getDefaultKeyOfIdentity(const Name& identity) const
{
  auto defaultKey = m_defaultKeys.find(identity);
  if (defaultKey == m_defaultKeys.end()) {
    BOOST_THROW_EXCEPTION(Pib::Error("No default key for identity `" + identity.toUri() + "`"));
  }

  return defaultKey->second;
}

bool
PibMemory::hasCertificate(const Name& certName) const
{
  return (m_certs.count(certName) > 0);
}

void
PibMemory::addCertificate(const v2::Certificate& certificate)
{
  Name certName = certificate.getName();
  Name keyName = certificate.getKeyName();
  Name identity = certificate.getIdentity();

  addKey(identity, keyName, certificate.getContent().value(), certificate.getContent().value_size());

  m_certs[certName] = certificate;
  if (m_defaultCerts.count(keyName) == 0) {
    m_defaultCerts[keyName] = certName;
  }
}

void
PibMemory::removeCertificate(const Name& certName)
{
  m_certs.erase(certName);
  auto defaultCert = m_defaultCerts.find(v2::extractKeyNameFromCertName(certName));
  if (defaultCert != m_defaultCerts.end() && defaultCert->second == certName) {
    m_defaultCerts.erase(defaultCert);
  }
}

v2::Certificate
PibMemory::getCertificate(const Name& certName) const
{
  if (!hasCertificate(certName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate `" + certName.toUri() +  "` does not exist"));
  }

  auto it = m_certs.find(certName);
  return it->second;
}

std::set<Name>
PibMemory::getCertificatesOfKey(const Name& keyName) const
{
  std::set<Name> certNames;
  for (const auto& it : m_certs) {
    if (v2::extractKeyNameFromCertName(it.second.getName()) == keyName) {
      certNames.insert(it.first);
    }
  }
  return certNames;
}

void
PibMemory::setDefaultCertificateOfKey(const Name& keyName, const Name& certName)
{
  if (!hasCertificate(certName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate `" + certName.toUri() +  "` does not exist"));
  }

  m_defaultCerts[keyName] = certName;
}

v2::Certificate
PibMemory::getDefaultCertificateOfKey(const Name& keyName) const
{
  auto it = m_defaultCerts.find(keyName);
  if (it == m_defaultCerts.end()) {
    BOOST_THROW_EXCEPTION(Pib::Error("No default certificate for key `" + keyName.toUri() + "`"));
  }

  auto certIt = m_certs.find(it->second);
  BOOST_ASSERT(certIt != m_certs.end());
  return certIt->second;
}

} // namespace pib
} // namespace security
} // namespace ndn
