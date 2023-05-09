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

#ifndef NDN_CXX_SECURITY_PIB_IMPL_KEY_IMPL_HPP
#define NDN_CXX_SECURITY_PIB_IMPL_KEY_IMPL_HPP

#include "ndn-cxx/security/security-common.hpp"
#include "ndn-cxx/security/pib/certificate-container.hpp"

namespace ndn::security::pib {

class PibImpl;

/**
 * @brief Backend instance of Key.
 *
 * A Key has only one backend instance, but may have multiple frontend handles.
 * Each frontend handle is associated with the only one backend KeyImpl.
 *
 * @throw PibImpl::Error When the underlying implementation has a non-semantic error.
 * @sa Key
 */
class KeyImpl : noncopyable
{
public:
  /**
   * @brief Create a key with name @p keyName.
   *
   * @param keyName The name of the key.
   * @param key The public key bits.
   * @param pibImpl The PIB backend implementation.
   * @pre The key must exist in the backend.
   * @throw std::invalid_argument @p key is invalid or unsupported.
   */
  KeyImpl(const Name& keyName, Buffer key, shared_ptr<PibImpl> pibImpl);

  // See security::pib::Key for the documentation of the following methods

  const Name&
  getName() const
  {
    return m_keyName;
  }

  const Name&
  getIdentity() const
  {
    return m_identity;
  }

  KeyType
  getKeyType() const
  {
    return m_keyType;
  }

  span<const uint8_t>
  getPublicKey() const
  {
    return m_key;
  }

  void
  addCertificate(const Certificate& cert);

  void
  removeCertificate(const Name& certName);

  Certificate
  getCertificate(const Name& certName) const
  {
    BOOST_ASSERT(m_certificates.isConsistent());
    return m_certificates.get(certName);
  }

  const CertificateContainer&
  getCertificates() const
  {
    BOOST_ASSERT(m_certificates.isConsistent());
    return m_certificates;
  }

  const Certificate&
  setDefaultCertificate(const Name& certName)
  {
    return setDefaultCert(m_certificates.get(certName));
  }

  void
  setDefaultCertificate(const Certificate& cert)
  {
    m_certificates.add(cert);
    setDefaultCert(cert);
  }

  const Certificate&
  getDefaultCertificate() const;

private:
  const Certificate&
  setDefaultCert(Certificate cert);

private:
  const Name m_identity;
  const Name m_keyName;
  const Buffer m_key;
  KeyType m_keyType = KeyType::NONE;

  const shared_ptr<PibImpl> m_pib;

  CertificateContainer m_certificates;
  mutable std::optional<Certificate> m_defaultCert;
};

} // namespace ndn::security::pib

#endif // NDN_CXX_SECURITY_PIB_IMPL_KEY_IMPL_HPP
