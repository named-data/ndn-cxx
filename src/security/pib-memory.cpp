/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

namespace ndn {
namespace security {

PibMemory::PibMemory()
  : m_hasDefaultIdentity(false)
{
}

void
PibMemory::setTpmLocator(const std::string& tpmLocator)
{
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

  auto keyIds = this->getKeysOfIdentity(identity);
  for (const name::Component& keyId : keyIds) {
    this->removeKey(identity, keyId);
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
PibMemory::hasKey(const Name& identity, const name::Component& keyId) const
{
  return (m_keys.count(getKeyName(identity, keyId)) > 0);
}

void
PibMemory::addKey(const Name& identity, const name::Component& keyId, const PublicKey& publicKey)
{
  this->addIdentity(identity);

  Name keyName = getKeyName(identity, keyId);
  m_keys[keyName] = publicKey;

  if (m_defaultKey.find(identity) == m_defaultKey.end())
    m_defaultKey[identity] = keyName;
}

void
PibMemory::removeKey(const Name& identity, const name::Component& keyId)
{
  Name keyName = getKeyName(identity, keyId);
  m_keys.erase(keyName);
  m_defaultKey.erase(identity);


  auto certNames = this->getCertificatesOfKey(identity, keyId);
  for (const auto& certName : certNames) {
    this->removeCertificate(certName);
  }
}

PublicKey
PibMemory::getKeyBits(const Name& identity, const name::Component& keyId) const
{
  if (!hasKey(identity, keyId))
    BOOST_THROW_EXCEPTION(Pib::Error("No key"));

  auto it = m_keys.find(getKeyName(identity, keyId));
  return it->second;
}

std::set<name::Component>
PibMemory::getKeysOfIdentity(const Name& identity) const
{
  std::set<name::Component> ids;
  for (const auto& it : m_keys) {
    if (identity == it.first.getPrefix(-1))
      ids.insert(it.first.get(-1));
  }
  return ids;
}

void
PibMemory::setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId)
{
  Name keyName = getKeyName(identity, keyId);

  if (!hasKey(identity, keyId))
    BOOST_THROW_EXCEPTION(Pib::Error("No key"));

  m_defaultKey[identity] = keyName;
}

name::Component
PibMemory::getDefaultKeyOfIdentity(const Name& identity) const
{
  auto it = m_defaultKey.find(identity);
  if (it == m_defaultKey.end())
    BOOST_THROW_EXCEPTION(Pib::Error("No default key"));

  return it->second.get(-1);
}

Name
PibMemory::getKeyName(const Name& identity, const name::Component& keyId) const
{
  Name keyName = identity;
  keyName.append(keyId);
  return keyName;
}

bool
PibMemory::hasCertificate(const Name& certName) const
{
  return (m_certs.count(certName) > 0);
}

void
PibMemory::addCertificate(const IdentityCertificate& certificate)
{
  this->addKey(certificate.getPublicKeyName().getPrefix(-1),
               certificate.getPublicKeyName().get(-1),
               certificate.getPublicKeyInfo());

  m_certs[certificate.getName()] = certificate;

  const Name& keyName = certificate.getPublicKeyName();
  if (m_defaultCert.find(keyName) == m_defaultCert.end())
    m_defaultCert[keyName] = certificate.getName();
}

void
PibMemory::removeCertificate(const Name& certName)
{
  m_certs.erase(certName);
  m_defaultCert.erase(IdentityCertificate::certificateNameToPublicKeyName(certName));
}

IdentityCertificate
PibMemory::getCertificate(const Name& certName) const
{
  if (!hasCertificate(certName))
    BOOST_THROW_EXCEPTION(Pib::Error("No cert"));

  auto it = m_certs.find(certName);
  return it->second;
}

std::set<Name>
PibMemory::getCertificatesOfKey(const Name& identity, const name::Component& keyId) const
{
  Name keyName = getKeyName(identity, keyId);

  std::set<Name> certNames;
  for (const auto& it : m_certs) {
    if (it.second.getPublicKeyName() == keyName)
      certNames.insert(it.first);
  }
  return certNames;
}

void
PibMemory::setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId, const Name& certName)
{
  if (!hasCertificate(certName))
    BOOST_THROW_EXCEPTION(Pib::Error("No cert"));

  Name keyName = getKeyName(identity, keyId);
  m_defaultCert[keyName] = certName;
}

IdentityCertificate
PibMemory::getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const
{
  Name keyName = getKeyName(identity, keyId);

  auto it = m_defaultCert.find(keyName);
  if (it == m_defaultCert.end())
    BOOST_THROW_EXCEPTION(Pib::Error("No default certificate"));

  auto certIt = m_certs.find(it->second);
  if (certIt == m_certs.end())
    BOOST_THROW_EXCEPTION(Pib::Error("No default certificate"));
  else
    return certIt->second;
}

} // namespace security
} // namespace ndn
