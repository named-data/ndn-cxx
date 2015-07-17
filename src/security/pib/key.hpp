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

#ifndef NDN_SECURITY_PIB_KEY_HPP
#define NDN_SECURITY_PIB_KEY_HPP

#include "../../data.hpp"
#include "certificate-container.hpp"
#include "../security-common.hpp"

namespace ndn {
namespace security {

class KeyChain;

namespace pib {

class PibImpl;

/**
 * @brief represents a key
 *
 * Key is at the second level in PIB's Identity-Key-Certificate hierarchy.  A Key has a Name
 * (identity + "KEY" + keyId), and contains one or more certificates, one of which is set as
 * the default certificate of this key.  A certificate can be directly accessed from a Key
 * object.
 */
class Key
{
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
   *     key = identity.getKey(...);
   *   }
   *   catch (const Pib::Error&) {
   *     ...
   *   }
   *
   * A Key instance created using this constructor is invalid. Calling a
   * member method on an invalid Key instance may cause an std::domain_error.
   */
  Key();

  /**
   * @brief Create a Key with @p keyName
   *
   * If the key/identity does not exist in the backend, create it in backend.
   *
   * @param keyName Key name
   * @param key The public key to add.
   * @param keyLen The length of the key.
   * @param impl The actual backend implementation.
   */
  Key(const Name& keyName, const uint8_t* key, size_t keyLen, shared_ptr<PibImpl> impl);

  /**
   * @brief Create a Key with @p keyName
   *
   * @param keyName Key name
   * @param impl The actual backend implementation.
   * @throws Pib::Error if the key does not exist.
   */
  Key(const Name& keyName, shared_ptr<PibImpl> impl);

  /// @brief Get the key name.
  const Name&
  getName() const;

  /// @brief Get the name of the belonging identity.
  const Name&
  getIdentity() const;

  /// @brief Get key type.
  KeyType
  getKeyType() const
  {
    return m_keyType;
  }

  /// @brief Get public key.
  const Buffer&
  getPublicKey() const;

  /**
   * @brief Get a certificate.
   *
   * @return the certificate
   * @throws Pib::Error if the certificate does not exist.
   */
  v2::Certificate
  getCertificate(const Name& certName) const;

  /// @brief Get all certificates for this key.
  const CertificateContainer&
  getCertificates() const;

  /**
   * @brief Get the default certificate for this Key.
   *
   * @throws Pib::Error if the default certificate does not exist.
   */
  const v2::Certificate&
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
  addCertificate(const v2::Certificate& certificate);

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
  const v2::Certificate&
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
  const v2::Certificate&
  setDefaultCertificate(const v2::Certificate& certificate);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Check the validity of this instance
   *
   * @throws std::domain_error if the instance is invalid
   */
  void
  validityCheck() const;

private:
  Name m_identity;
  Name m_keyName;
  Buffer m_key;
  KeyType m_keyType;

  mutable bool m_hasDefaultCertificate;
  mutable v2::Certificate m_defaultCertificate;

  mutable bool m_needRefreshCerts;
  mutable CertificateContainer m_certificates;

  shared_ptr<PibImpl> m_impl;
};

} // namespace pib

using pib::Key;

namespace v2 {

/**
 * @brief Construct key name based on the appropriate naming conventions
 */
Name
constructKeyName(const Name& identity, const name::Component& keyId);

/**
 * @brief Check if @p keyName follow the naming conventions for the key name
 */
bool
isValidKeyName(const Name& keyName);

/**
 * @brief Extract identity namespace from the key name @p keyName
 */
Name
extractIdentityFromKeyName(const Name& keyName);

} // namespace v2

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_KEY_HPP
