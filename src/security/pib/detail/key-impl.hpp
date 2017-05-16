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

#ifndef NDN_SECURITY_PIB_DETAIL_KEY_IMPL_HPP
#define NDN_SECURITY_PIB_DETAIL_KEY_IMPL_HPP

#include "../../security-common.hpp"
#include "../certificate-container.hpp"

namespace ndn {
namespace security {
namespace pib {

class PibImpl;

namespace detail {

/**
 * @brief Backend instance of Key
 *
 * An Key has only one backend instance, but may have multiple frontend handles.
 * Each frontend handle is associated with the only one backend KeyImpl.
 *
 * @throw PibImpl::Error when underlying implementation has non-semantic error.
 */
class KeyImpl : noncopyable
{
public:
  /**
   * @brief Create a KeyImpl with @p keyName.
   *
   * If the key does not exist in the backend, it will be added.
   * If a key with the same name already exists, it will be overwritten.
   *
   * @param keyName The name of the key.
   * @param key The public key to add.
   * @param keyLen The length of the key.
   * @param pibImpl The Pib backend implementation.
   * @throw std::invalid_argument @p key is invalid.
   */
  KeyImpl(const Name& keyName, const uint8_t* key, size_t keyLen, shared_ptr<PibImpl> pibImpl);

  /**
   * @brief Create a KeyImpl with @p keyName.
   *
   * @param keyName The name of the key.
   * @param pibImpl The Pib backend implementation.
   * @throw Pib::Error the key does not exist.
   */
  KeyImpl(const Name& keyName, shared_ptr<PibImpl> pibImpl);

  /**
   * @brief Get the name of the key.
   */
  const Name&
  getName() const
  {
    return m_keyName;
  }

  /**
   * @brief Get the name of the belonging identity.
   */
  const Name&
  getIdentity() const
  {
    return m_identity;
  }

  /**
   * @brief Get key type.
   */
  KeyType
  getKeyType() const
  {
    return m_keyType;
  }

  /**
   * @brief Get public key bits.
   */
  const Buffer&
  getPublicKey() const
  {
    return m_key;
  }

  /**
   * @brief Add @p certificate.
   *
   * If no default certificate is set before, the new certificate will be set as the default
   * certificate of the key.
   *
   * If a certificate with the same name (without implicit digest) already exists, it will
   * be overwritten.
   *
   * @throw std::invalid_argument the certificate name does not match the key name.
   */
  void
  addCertificate(const v2::Certificate& certificate);

  /**
   * @brief Remove a certificate with @p certName.
   * @throw std::invalid_argument @p certName does not match the key name.
   */
  void
  removeCertificate(const Name& certName);

  /**
   * @brief Get a certificate with @p certName.
   * @throw std::invalid_argument @p certName does not match the key name.
   * @throw Pib::Error the certificate does not exist.
   */
  v2::Certificate
  getCertificate(const Name& certName) const;

  /**
   * @brief Get all the certificates for this key.
   */
  const CertificateContainer&
  getCertificates() const;

  /**
   * @brief Set an existing certificate with name @p certName as the default certificate.
   * @throw std::invalid_argument @p certName does not match the key name.
   * @throw Pib::Error the certificate does not exist.
   * @return the default certificate
   */
  const v2::Certificate&
  setDefaultCertificate(const Name& certName);

  /**
   * @brief Add @p certificate and set it as the default certificate for this key.
   *
   * If a certificate with the same name (without implicit digest) already exists, it will
   * be overwritten.
   *
   * @throw std::invalid_argument @p certificate does not match the key name.
   * @return the default certificate
   */
  const v2::Certificate&
  setDefaultCertificate(const v2::Certificate& certificate);

  /**
   * @brief Get the default certificate for this key.
   * @throw Pib::Error the default certificate does not exist.
   */
  const v2::Certificate&
  getDefaultCertificate() const;

private:
  Name m_identity;
  Name m_keyName;
  Buffer m_key;
  KeyType m_keyType;

  shared_ptr<PibImpl> m_pib;

  CertificateContainer m_certificates;
  mutable bool m_isDefaultCertificateLoaded;
  mutable v2::Certificate m_defaultCertificate;
};

} // namespace detail
} // namespace pib
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_DETAIL_KEY_IMPL_HPP
