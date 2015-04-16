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

#include "in-memory-pib-impl.hpp"
#include "pib.hpp"

namespace ndn {
namespace security {

InMemoryPibImpl::InMemoryPibImpl()
  : m_hasDefaultIdentity(false)
{
}

void
InMemoryPibImpl::setTpmLocator(const std::string& tpmLocator)
{
  throw Error("InMemoryPib/Tpm does not need a locator");
}

std::string
InMemoryPibImpl::getTpmLocator() const
{
  return "tpm-memory:";
}

bool
InMemoryPibImpl::hasIdentity(const Name& identity) const
{
  return (m_identities.count(identity) > 0);
}

void
InMemoryPibImpl::addIdentity(const Name& identity)
{
  m_identities.insert(identity);
}

void
InMemoryPibImpl::removeIdentity(const Name& identity)
{
  m_identities.erase(identity);
  if (identity == m_defaultIdentity)
    m_hasDefaultIdentity = false;
}

std::set<Name>
InMemoryPibImpl::getIdentities() const
{
  return m_identities;
}

void
InMemoryPibImpl::setDefaultIdentity(const Name& identityName)
{
  addIdentity(identityName);
  m_defaultIdentity = identityName;
  m_hasDefaultIdentity = true;
}

Name
InMemoryPibImpl::getDefaultIdentity() const
{
  if (m_hasDefaultIdentity)
    return m_defaultIdentity;

  throw Pib::Error("No default identity");
}

bool
InMemoryPibImpl::hasKey(const Name& identity, const name::Component& keyId) const
{
  return (m_keys.count(getKeyName(identity, keyId)) > 0);
}

void
InMemoryPibImpl::addKey(const Name& identity, const name::Component& keyId, const PublicKey& publicKey)
{
  m_keys[getKeyName(identity, keyId)] = publicKey;
}

void
InMemoryPibImpl::removeKey(const Name& identity, const name::Component& keyId)
{
  m_keys.erase(getKeyName(identity, keyId));
}

PublicKey
InMemoryPibImpl::getKeyBits(const Name& identity, const name::Component& keyId) const
{
  auto it = m_keys.find(getKeyName(identity, keyId));
  if (it == m_keys.end())
    throw Pib::Error("No default certificate");

  return it->second;
}

std::set<name::Component>
InMemoryPibImpl::getKeysOfIdentity(const Name& identity) const
{
  std::set<name::Component> ids;
  for (const auto& it : m_keys) {
    if (identity == it.first.getPrefix(-1))
      ids.insert(it.first.get(-1));
  }
  return ids;
}

void
InMemoryPibImpl::setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId)
{
  Name keyName = getKeyName(identity, keyId);

  if (!hasKey(identity, keyId))
    throw Pib::Error("No key");

  m_defaultKey[identity] = keyName;
}

name::Component
InMemoryPibImpl::getDefaultKeyOfIdentity(const Name& identity) const
{
  auto it = m_defaultKey.find(identity);
  if (it == m_defaultKey.end())
    throw Pib::Error("No default key");

  return it->second.get(-1);
}

Name
InMemoryPibImpl::getKeyName(const Name& identity, const name::Component& keyId) const
{
  Name keyName = identity;
  keyName.append(keyId);
  return keyName;
}

bool
InMemoryPibImpl::hasCertificate(const Name& certName) const
{
  return (m_certs.count(certName) > 0);
}

void
InMemoryPibImpl::addCertificate(const IdentityCertificate& certificate)
{
  m_certs[certificate.getName()] = certificate;
}

void
InMemoryPibImpl::removeCertificate(const Name& certName)
{
  m_certs.erase(certName);
}

IdentityCertificate
InMemoryPibImpl::getCertificate(const Name& certName) const
{
  auto it = m_certs.find(certName);
  if (it == m_certs.end())
    throw Pib::Error("No cert");

  return it->second;
}

std::set<Name>
InMemoryPibImpl::getCertificatesOfKey(const Name& identity, const name::Component& keyId) const
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
InMemoryPibImpl::setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId, const Name& certName)
{
  if (!hasCertificate(certName))
    throw Pib::Error("No cert");

  Name keyName = getKeyName(identity, keyId);
  m_defaultCert[keyName] = certName;
}

IdentityCertificate
InMemoryPibImpl::getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const
{
  Name keyName = getKeyName(identity, keyId);

  auto it = m_defaultCert.find(keyName);
  if (it == m_defaultCert.end())
    throw Pib::Error("No default certificate");

  auto certIt = m_certs.find(it->second);
  if (certIt == m_certs.end())
    throw Pib::Error("No default certificate");
  else
    return certIt->second;
}

} // namespace security
} // namespace ndn
