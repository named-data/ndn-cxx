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

PibMemory::PibMemory()
  : m_hasDefaultIdentity(false)
{
}

void
PibMemory::setTpmLocator(const std::string& tpmLocator)
{
  // The locator of PibMemory is always 'tpm-memory:'
  BOOST_THROW_EXCEPTION(Error("PibMemory does not need a locator"));
}

std::string
PibMemory::getTpmLocator() const
{
  return "tpm-memory:";
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
  if (identity == m_defaultIdentity)
    m_hasDefaultIdentity = false;

  auto keyNames = this->getKeysOfIdentity(identity);
  for (const Name& keyName : keyNames) {
    this->removeKey(keyName);
  }
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
  if (m_hasDefaultIdentity)
    return m_defaultIdentity;

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
  this->addIdentity(identity);

  m_keys[keyName] = Buffer(key, keyLen);

  if (m_defaultKey.find(identity) == m_defaultKey.end())
    m_defaultKey[identity] = keyName;
}

void
PibMemory::removeKey(const Name& keyName)
{
  Name identity = v2::extractIdentityFromKeyName(keyName);

  m_keys.erase(keyName);
  m_defaultKey.erase(identity);

  auto certNames = this->getCertificatesOfKey(keyName);
  for (const auto& certName : certNames) {
    this->removeCertificate(certName);
  }
}

Buffer
PibMemory::getKeyBits(const Name& keyName) const
{
  if (!hasKey(keyName))
    BOOST_THROW_EXCEPTION(Pib::Error("Key `" + keyName.toUri() + "` not found"));

  auto it = m_keys.find(keyName);
  return it->second;
}

std::set<Name>
PibMemory::getKeysOfIdentity(const Name& identity) const
{
  std::set<Name> ids;
  for (const auto& it : m_keys) {
    if (identity == v2::extractIdentityFromKeyName(it.first))
      ids.insert(it.first);
  }
  return ids;
}

void
PibMemory::setDefaultKeyOfIdentity(const Name& identity, const Name& keyName)
{
  if (!hasKey(keyName))
    BOOST_THROW_EXCEPTION(Pib::Error("Key `" + keyName.toUri() + "` not found"));

  m_defaultKey[identity] = keyName;
}

Name
PibMemory::getDefaultKeyOfIdentity(const Name& identity) const
{
  auto it = m_defaultKey.find(identity);
  if (it == m_defaultKey.end())
    BOOST_THROW_EXCEPTION(Pib::Error("No default key for identity `" + identity.toUri() + "`"));

  return it->second;
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

  this->addKey(identity, keyName, certificate.getContent().value(), certificate.getContent().value_size());

  m_certs[certName] = certificate;
  if (m_defaultCert.find(keyName) == m_defaultCert.end())
    m_defaultCert[keyName] = certName;
}

void
PibMemory::removeCertificate(const Name& certName)
{
  m_certs.erase(certName);
  m_defaultCert.erase(v2::extractKeyNameFromCertName(certName));
}

v2::Certificate
PibMemory::getCertificate(const Name& certName) const
{
  if (!hasCertificate(certName))
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate `" + certName.toUri() +  "` does not exist"));

  auto it = m_certs.find(certName);
  return it->second;
}

std::set<Name>
PibMemory::getCertificatesOfKey(const Name& keyName) const
{
  std::set<Name> certNames;
  for (const auto& it : m_certs) {
    if (v2::extractKeyNameFromCertName(it.second.getName()) == keyName)
      certNames.insert(it.first);
  }
  return certNames;
}

void
PibMemory::setDefaultCertificateOfKey(const Name& keyName, const Name& certName)
{
  if (!hasCertificate(certName))
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate `" + certName.toUri() +  "` does not exist"));

  m_defaultCert[keyName] = certName;
}

v2::Certificate
PibMemory::getDefaultCertificateOfKey(const Name& keyName) const
{
  auto it = m_defaultCert.find(keyName);
  if (it == m_defaultCert.end())
    BOOST_THROW_EXCEPTION(Pib::Error("No default certificate for key `" + keyName.toUri() + "`"));

  auto certIt = m_certs.find(it->second);
  if (certIt == m_certs.end())
    BOOST_THROW_EXCEPTION(Pib::Error("No default certificate for key `" + keyName.toUri() + "`"));
  else
    return certIt->second;
}

} // namespace pib
} // namespace security
} // namespace ndn
