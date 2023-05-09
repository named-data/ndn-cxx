/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/security/pib/impl/pib-memory.hpp"

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm_ext/insert.hpp>

namespace ndn::security::pib {

PibMemory::PibMemory(const std::string&)
{
}

const std::string&
PibMemory::getScheme()
{
  static const std::string scheme("pib-memory");
  return scheme;
}

void
PibMemory::setTpmLocator(const std::string& tpmLocator)
{
  m_tpmLocator = tpmLocator;
}

bool
PibMemory::hasIdentity(const Name& identity) const
{
  return m_identities.count(identity) > 0;
}

void
PibMemory::addIdentity(const Name& identity)
{
  m_identities.insert(identity);

  if (!m_defaultIdentity) {
    m_defaultIdentity = identity;
  }
}

void
PibMemory::removeIdentity(const Name& identity)
{
  m_identities.erase(identity);

  if (identity == m_defaultIdentity) {
    m_defaultIdentity.reset();
  }

  for (const Name& keyName : getKeysOfIdentity(identity)) {
    removeKey(keyName);
  }
}

void
PibMemory::clearIdentities()
{
  m_defaultIdentity.reset();
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
  if (!hasIdentity(identityName)) {
    NDN_THROW(Pib::Error("Cannot set non-existing identity `" + identityName.toUri() + "` as default"));
  }

  m_defaultIdentity = identityName;
}

Name
PibMemory::getDefaultIdentity() const
{
  if (!m_defaultIdentity) {
    NDN_THROW(Pib::Error("No default identity"));
  }

  return *m_defaultIdentity;
}

bool
PibMemory::hasKey(const Name& keyName) const
{
  return m_keys.count(keyName) > 0;
}

void
PibMemory::addKey(const Name& identity, const Name& keyName, span<const uint8_t> key)
{
  // ensure identity exists
  addIdentity(identity);

  m_keys[keyName] = Buffer(key.begin(), key.end()); // use insert_or_assign in C++17

  if (m_defaultKeys.count(identity) == 0) {
    m_defaultKeys[identity] = keyName;
  }
}

void
PibMemory::removeKey(const Name& keyName)
{
  Name identity = extractIdentityFromKeyName(keyName);

  m_keys.erase(keyName);
  m_defaultKeys.erase(identity);

  for (const auto& certName : getCertificatesOfKey(keyName)) {
    removeCertificate(certName);
  }
}

Buffer
PibMemory::getKeyBits(const Name& keyName) const
{
  if (!hasKey(keyName)) {
    NDN_THROW(Pib::Error("Key `" + keyName.toUri() + "` not found in PIB"));
  }

  auto key = m_keys.find(keyName);
  BOOST_ASSERT(key != m_keys.end());
  return key->second;
}

std::set<Name>
PibMemory::getKeysOfIdentity(const Name& identity) const
{
  std::set<Name> keyNames;
  boost::insert(keyNames,
                m_keys | boost::adaptors::map_keys | boost::adaptors::filtered(
                  [&] (const auto& kn) { return extractIdentityFromKeyName(kn) == identity; }));
  return keyNames;
}

void
PibMemory::setDefaultKeyOfIdentity(const Name& identity, const Name& keyName)
{
  if (!hasKey(keyName)) {
    NDN_THROW(Pib::Error("Cannot set non-existing key `" + keyName.toUri() + "` as default"));
  }

  m_defaultKeys[identity] = keyName;
}

Name
PibMemory::getDefaultKeyOfIdentity(const Name& identity) const
{
  auto defaultKey = m_defaultKeys.find(identity);
  if (defaultKey == m_defaultKeys.end()) {
    NDN_THROW(Pib::Error("No default key for identity `" + identity.toUri() + "`"));
  }

  return defaultKey->second;
}

bool
PibMemory::hasCertificate(const Name& certName) const
{
  return m_certs.count(certName) > 0;
}

void
PibMemory::addCertificate(const Certificate& certificate)
{
  const Name& certName = certificate.getName();
  const Name& keyName = certificate.getKeyName();

  // ensure key exists
  addKey(certificate.getIdentity(), keyName, certificate.getPublicKey());

  m_certs[certName] = certificate;
  if (m_defaultCerts.count(keyName) == 0) {
    m_defaultCerts[keyName] = certName;
  }
}

void
PibMemory::removeCertificate(const Name& certName)
{
  m_certs.erase(certName);
  auto defaultCert = m_defaultCerts.find(extractKeyNameFromCertName(certName));
  if (defaultCert != m_defaultCerts.end() && defaultCert->second == certName) {
    m_defaultCerts.erase(defaultCert);
  }
}

Certificate
PibMemory::getCertificate(const Name& certName) const
{
  if (!hasCertificate(certName)) {
    NDN_THROW(Pib::Error("Certificate `" + certName.toUri() + "` not found in PIB"));
  }

  auto it = m_certs.find(certName);
  return it->second;
}

std::set<Name>
PibMemory::getCertificatesOfKey(const Name& keyName) const
{
  std::set<Name> certNames;
  boost::insert(certNames,
                m_certs | boost::adaptors::map_keys | boost::adaptors::filtered(
                  [&] (const auto& cn) { return extractKeyNameFromCertName(cn) == keyName; }));
  return certNames;
}

void
PibMemory::setDefaultCertificateOfKey(const Name& keyName, const Name& certName)
{
  if (!hasCertificate(certName)) {
    NDN_THROW(Pib::Error("Cannot set non-existing certificate `" + certName.toUri() + "` as default"));
  }

  m_defaultCerts[keyName] = certName;
}

Certificate
PibMemory::getDefaultCertificateOfKey(const Name& keyName) const
{
  auto it = m_defaultCerts.find(keyName);
  if (it == m_defaultCerts.end()) {
    NDN_THROW(Pib::Error("No default certificate for key `" + keyName.toUri() + "`"));
  }

  auto certIt = m_certs.find(it->second);
  BOOST_ASSERT(certIt != m_certs.end());
  return certIt->second;
}

} // namespace ndn::security::pib
