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

#include "key-impl.hpp"
#include "../pib-impl.hpp"
#include "../pib.hpp"
#include "../../transform/public-key.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace detail {

KeyImpl::KeyImpl(const Name& keyName, const uint8_t* key, size_t keyLen, shared_ptr<PibImpl> pibImpl)
  : m_identity(v2::extractIdentityFromKeyName(keyName))
  , m_keyName(keyName)
  , m_key(key, keyLen)
  , m_pib(std::move(pibImpl))
  , m_certificates(keyName, m_pib)
  , m_isDefaultCertificateLoaded(false)
{
  BOOST_ASSERT(m_pib != nullptr);

  transform::PublicKey publicKey;
  try {
    publicKey.loadPkcs8(key, keyLen);
  }
  catch (const transform::PublicKey::Error&) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Invalid key bits"));
  }
  m_keyType = publicKey.getKeyType();

  m_pib->addKey(m_identity, m_keyName, key, keyLen);
}

KeyImpl::KeyImpl(const Name& keyName, shared_ptr<PibImpl> pibImpl)
  : m_identity(v2::extractIdentityFromKeyName(keyName))
  , m_keyName(keyName)
  , m_pib(std::move(pibImpl))
  , m_certificates(keyName, m_pib)
  , m_isDefaultCertificateLoaded(false)
{
  BOOST_ASSERT(m_pib != nullptr);

  m_key = m_pib->getKeyBits(m_keyName);

  transform::PublicKey key;
  key.loadPkcs8(m_key.data(), m_key.size());
  m_keyType = key.getKeyType();
}

void
KeyImpl::addCertificate(const v2::Certificate& certificate)
{
  BOOST_ASSERT(m_certificates.isConsistent());
  m_certificates.add(certificate);
}

void
KeyImpl::removeCertificate(const Name& certName)
{
  BOOST_ASSERT(m_certificates.isConsistent());

  if (m_isDefaultCertificateLoaded && m_defaultCertificate.getName() == certName)
    m_isDefaultCertificateLoaded = false;

  m_certificates.remove(certName);
}

v2::Certificate
KeyImpl::getCertificate(const Name& certName) const
{
  BOOST_ASSERT(m_certificates.isConsistent());
  return m_certificates.get(certName);
}

const CertificateContainer&
KeyImpl::getCertificates() const
{
  BOOST_ASSERT(m_certificates.isConsistent());
  return m_certificates;
}

const v2::Certificate&
KeyImpl::setDefaultCertificate(const Name& certName)
{
  BOOST_ASSERT(m_certificates.isConsistent());

  m_defaultCertificate = m_certificates.get(certName);
  m_pib->setDefaultCertificateOfKey(m_keyName, certName);
  m_isDefaultCertificateLoaded = true;
  return m_defaultCertificate;
}

const v2::Certificate&
KeyImpl::setDefaultCertificate(const v2::Certificate& certificate)
{
  addCertificate(certificate);
  return setDefaultCertificate(certificate.getName());
}

const v2::Certificate&
KeyImpl::getDefaultCertificate() const
{
  BOOST_ASSERT(m_certificates.isConsistent());

  if (!m_isDefaultCertificateLoaded) {
    m_defaultCertificate = m_pib->getDefaultCertificateOfKey(m_keyName);
    m_isDefaultCertificateLoaded = true;
  }
  BOOST_ASSERT(m_pib->getDefaultCertificateOfKey(m_keyName).wireEncode() == m_defaultCertificate.wireEncode());

  return m_defaultCertificate;
}

} // namespace detail
} // namespace pib
} // namespace security
} // namespace ndn
