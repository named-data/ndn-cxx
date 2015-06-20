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

#ifndef NDN_SECURITY_KEY_HPP
#define NDN_SECURITY_KEY_HPP

#include "identity-certificate.hpp"
#include "certificate-container.hpp"

namespace ndn {
namespace security {

class PibImpl;
class Identity;
class KeyContainer;

/**
 * @brief represents a key
 *
 * Key is at the second level in PIB's Identity-Key-Certificate hierarchy.
 * An Key has a Name (identity + keyId), and contains one or more certificates,
 * one of which is set as the default certificate of this key.  A certificate
 * can be directly accessed from a Key object.
 *
 * @throw PibImpl::Error when underlying implementation has non-semantic error.
 */
class Key
{
public:
  friend class Identity;
  friend class KeyContainer;
  friend class KeyChain;

public:
  /**
   * @brief Default Constructor
   *
   * Key created using this default constructor is just a place holder.
   * It must obtain an actual instance from Identity::getKey(...).  A typical
   * usage would be for exception handling:
   *
   *   Key key;
   *   try {
   *     key = Identity.getKey(...);
   *   }
   *   catch (Pib::Error&) {
   *     ...
   *   }
   *
   * A Key instance created using the constructor is invalid. Calling a
   * member method on an invalid Key instance may cause an std::domain_error.
   */
  Key();

  /// @brief Get the name of the key.
  const Name&
  getName() const;

  /// @brief Get the name of the belonging identity.
  const Name&
  getIdentity() const;

  /// @brief Get the key id of the key.
  const name::Component&
  getKeyId() const;

  /// @brief Get public key
  const PublicKey&
  getPublicKey() const;

  /**
   * @brief Get a certificate.
   *
   * @return the certificate
   * @throws Pib::Error if the certificate does not exist.
   */
  IdentityCertificate
  getCertificate(const Name& certName) const;

  /// @brief Get all the certificates for this key.
  const CertificateContainer&
  getCertificates() const;

  /**
   * @brief Get the default certificate for this Key.
   *
   * @throws Pib::Error if the default certificate does not exist.
   */
  const IdentityCertificate&
  getDefaultCertificate() const;

  /// @brief Check if the Key instance is valid
  operator bool() const;

  /// @brief Check if the Key instance is invalid
  bool
  operator!() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations should be private

  /**
   * @brief Add a certificate.
   *
   * @param certificate The certificate to add.
   */
  void
  addCertificate(const IdentityCertificate& certificate);

  /**
   * @brief Remove a certificate.
   *
   * @param certName The name of the certificate to delete.
   */
  void
  removeCertificate(const Name& certName);

  /**
   * @brief Set the default certificate.
   *
   * @param certName The name of the default certificate of the key.
   * @return the default certificate
   * @throws Pib::Error if the certificate does not exist.
   */
  const IdentityCertificate&
  setDefaultCertificate(const Name& certName);

  /**
   * @brief Set the default certificate.
   *
   * If the certificate does not exist, add it and set it as the default certificate of the key.
   * If the certificate exists, simply set it as the default certificate of the key.
   *
   * @param certificate The certificate to add.
   * @return the default certificate
   */
  const IdentityCertificate&
  setDefaultCertificate(const IdentityCertificate& certificate);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Create a Key with @p identityName and @p keyId.
   *
   * If the key/identity does not exist in the backend, create it in backend.
   *
   * @param identityName The name of the Identity.
   * @param keyId The key id of the key.
   * @param publicKey The public key to add.
   * @param impl The actual backend implementation.
   */
  Key(const Name& identityName, const name::Component& keyId,
      const PublicKey& publicKey, shared_ptr<PibImpl> impl);

  /**
   * @brief Create an KeyEntry with @p identityName and @p keyId.
   *
   * @param identityName The name of the Identity.
   * @param keyId The key id of the key.
   * @param impl The actual backend implementation.
   * @throws Pib::Error if the key does not exist.
   */
  Key(const Name& identityName, const name::Component& keyId, shared_ptr<PibImpl> impl);

  /**
   * @brief Check the validity of this instance
   *
   * @throws std::domain_error if the instance is invalid
   */
  void
  validityCheck() const;

private:
  Name m_id;
  name::Component m_keyId;
  Name m_keyName;
  PublicKey m_key;

  mutable bool m_hasDefaultCertificate;
  mutable IdentityCertificate m_defaultCertificate;

  mutable bool m_needRefreshCerts;
  mutable CertificateContainer m_certificates;

  shared_ptr<PibImpl> m_impl;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_HPP
