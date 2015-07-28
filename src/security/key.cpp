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

#include "key.hpp"
#include "pib-impl.hpp"
#include "pib.hpp"

namespace ndn {
namespace security {

Key::Key()
  : m_hasDefaultCertificate(false)
  , m_needRefreshCerts(false)
  , m_impl(nullptr)
{
}

Key::Key(const Name& identityName, const name::Component& keyId,
         const PublicKey& publicKey, shared_ptr<PibImpl> impl)
  : m_id(identityName)
  , m_keyId(keyId)
  , m_key(publicKey)
  , m_hasDefaultCertificate(false)
  , m_needRefreshCerts(true)
  , m_impl(impl)
{
  validityCheck();

  m_keyName = m_id;
  m_keyName.append(m_keyId);

  m_impl->addIdentity(m_id);
  m_impl->addKey(m_id, m_keyId, publicKey);
}

Key::Key(const Name& identityName, const name::Component& keyId,
         shared_ptr<PibImpl> impl)
  : m_id(identityName)
  , m_keyId(keyId)
  , m_hasDefaultCertificate(false)
  , m_needRefreshCerts(true)
  , m_impl(impl)
{
  validityCheck();

  m_keyName = m_id;
  m_keyName.append(m_keyId);

  m_key = m_impl->getKeyBits(m_id, m_keyId);
}

const Name&
Key::getName() const
{
  validityCheck();

  return m_keyName;
}

const Name&
Key::getIdentity() const
{
  validityCheck();

  return m_id;
}

const name::Component&
Key::getKeyId() const
{
  validityCheck();

  return m_keyId;
}

const PublicKey&
Key::getPublicKey() const
{
  validityCheck();

  return m_key;
}

void
Key::addCertificate(const IdentityCertificate& certificate)
{
  validityCheck();

  if (!m_needRefreshCerts &&
      m_certificates.find(certificate.getName()) == m_certificates.end()) {
    // if we have already loaded all the certificate, but the new certificate is not one of them
    // the CertificateContainer should be refreshed
    m_needRefreshCerts = true;
  }

  m_impl->addCertificate(certificate);
}

void
Key::removeCertificate(const Name& certName)
{
  validityCheck();

  if (m_hasDefaultCertificate && m_defaultCertificate.getName() == certName)
    m_hasDefaultCertificate = false;

  m_impl->removeCertificate(certName);
  m_needRefreshCerts = true;
}

IdentityCertificate
Key::getCertificate(const Name& certName) const
{
  validityCheck();

  return m_impl->getCertificate(certName);
}

const CertificateContainer&
Key::getCertificates() const
{
  validityCheck();

  if (m_needRefreshCerts) {
    m_certificates = std::move(CertificateContainer(m_impl->getCertificatesOfKey(m_id, m_keyId),
                                                    m_impl));
    m_needRefreshCerts = false;
  }

  return m_certificates;
}

const IdentityCertificate&
Key::setDefaultCertificate(const Name& certName)
{
  validityCheck();

  m_defaultCertificate = m_impl->getCertificate(certName);
  m_impl->setDefaultCertificateOfKey(m_id, m_keyId, certName);
  m_hasDefaultCertificate = true;
  return m_defaultCertificate;
}

const IdentityCertificate&
Key::setDefaultCertificate(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  return setDefaultCertificate(certificate.getName());
}

const IdentityCertificate&
Key::getDefaultCertificate() const
{
  validityCheck();

  if (!m_hasDefaultCertificate) {
    m_defaultCertificate = m_impl->getDefaultCertificateOfKey(m_id, m_keyId);
    m_hasDefaultCertificate = true;
  }

  return m_defaultCertificate;
}

Key::operator bool() const
{
  return !(this->operator!());
}

bool
Key::operator!() const
{
  return (m_impl == nullptr);
}

void
Key::validityCheck() const
{
  if (m_impl == nullptr)
    BOOST_THROW_EXCEPTION(std::domain_error("Invalid Key instance"));
}

} // namespace security
} // namespace ndn
