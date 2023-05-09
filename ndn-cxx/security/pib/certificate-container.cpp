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

#include "ndn-cxx/security/pib/certificate-container.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn::security::pib {

NDN_LOG_INIT(ndn.security.CertificateContainer);

CertificateContainer::const_iterator::const_iterator(NameSet::const_iterator it,
                                                     const CertificateContainer& container) noexcept
  : m_it(it)
  , m_container(&container)
{
}

Certificate
CertificateContainer::const_iterator::operator*()
{
  BOOST_ASSERT(m_container != nullptr);
  return m_container->get(*m_it);
}

bool
CertificateContainer::const_iterator::operator==(const const_iterator& other) const
{
  bool isThisEnd = m_container == nullptr || m_it == m_container->m_certNames.end();
  bool isOtherEnd = other.m_container == nullptr || other.m_it == other.m_container->m_certNames.end();
  if (isThisEnd)
    return isOtherEnd;
  return !isOtherEnd && m_container->m_pib == other.m_container->m_pib && m_it == other.m_it;
}

CertificateContainer::CertificateContainer(const Name& keyName, shared_ptr<PibImpl> pibImpl)
  : m_keyName(keyName)
  , m_pib(std::move(pibImpl))
{
  BOOST_ASSERT(m_pib != nullptr);
  m_certNames = m_pib->getCertificatesOfKey(keyName);
}

CertificateContainer::const_iterator
CertificateContainer::find(const Name& certName) const
{
  return {m_certNames.find(certName), *this};
}

void
CertificateContainer::add(const Certificate& certificate)
{
  if (m_keyName != certificate.getKeyName()) {
    NDN_THROW(std::invalid_argument("Certificate name `" + certificate.getName().toUri() + "` "
                                    "does not match key `" + m_keyName.toUri() + "`"));
  }

  const Name& certName = certificate.getName();
  bool isNew = m_certNames.insert(certName).second;
  NDN_LOG_DEBUG((isNew ? "Adding " : "Replacing ") << certName);

  m_pib->addCertificate(certificate);
  m_certs[certName] = certificate; // use insert_or_assign in C++17
}

void
CertificateContainer::remove(const Name& certName)
{
  if (m_keyName != extractKeyNameFromCertName(certName)) {
    NDN_THROW(std::invalid_argument("Certificate name `" + certName.toUri() + "` "
                                    "does not match key `" + m_keyName.toUri() + "`"));
  }

  if (m_certNames.erase(certName) > 0) {
    NDN_LOG_DEBUG("Removing " << certName);
    m_certs.erase(certName);
  }
  else {
    // consistency check
    BOOST_ASSERT(m_certs.find(certName) == m_certs.end());
  }
  m_pib->removeCertificate(certName);
}

Certificate
CertificateContainer::get(const Name& certName) const
{
  if (m_keyName != extractKeyNameFromCertName(certName)) {
    NDN_THROW(std::invalid_argument("Certificate name `" + certName.toUri() + "` "
                                    "does not match key `" + m_keyName.toUri() + "`"));
  }

  if (auto it = m_certs.find(certName); it != m_certs.end()) {
    return it->second;
  }

  auto ret = m_certs.emplace(certName, m_pib->getCertificate(certName));
  return ret.first->second;
}

bool
CertificateContainer::isConsistent() const
{
  return m_certNames == m_pib->getCertificatesOfKey(m_keyName);
}

} // namespace ndn::security::pib
