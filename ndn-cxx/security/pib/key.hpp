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

#ifndef NDN_CXX_SECURITY_PIB_KEY_HPP
#define NDN_CXX_SECURITY_PIB_KEY_HPP

#include "ndn-cxx/security/pib/certificate-container.hpp"
#include "ndn-cxx/security/security-common.hpp"

namespace ndn::security {

class KeyChain;

namespace pib {

class KeyContainer;
class KeyImpl;

/**
 * @brief Frontend handle for a key in the PIB.
 *
 * Key is at the second level in PIB's Identity-Key-Certificate hierarchy. A key has the name
 * `/<Identity>/KEY/<KeyId>`, and contains one or more certificates, one of which is set as
 * default certificate of that key. Certificates can be directly accessed from a Key object.
 */
class Key
{
public:
  /**
   * @brief Default constructor.
   *
   * A Key created using this default constructor is just a placeholder.
   * You can obtain an actual instance from Identity::getKey(). A typical
   * usage would be for exception handling:
   *
   * @code
   * Key key;
   * try {
   *   key = identity.getKey(...);
   * }
   * catch (const Pib::Error&) {
   *   ...
   * }
   * @endcode
   *
   * An instance created using this constructor is invalid. Calling a member
   * function on an invalid Key instance may throw an std::domain_error.
   */
  Key() noexcept;

  /**
   * @brief Return the name of the key.
   */
  const Name&
  getName() const;

  /**
   * @brief Return the name of the owning identity.
   */
  const Name&
  getIdentity() const;

  /**
   * @brief Return the key type.
   */
  KeyType
  getKeyType() const;

  /**
   * @brief Return the raw public key bits.
   */
  span<const uint8_t>
  getPublicKey() const;

  /**
   * @brief Return the certificate with the given name.
   * @throw std::invalid_argument @p certName does not match the key name.
   * @throw Pib::Error the certificate does not exist.
   */
  Certificate
  getCertificate(const Name& certName) const;

  /**
   * @brief Return all the certificates of this key.
   */
  const CertificateContainer&
  getCertificates() const;

  /**
   * @brief Return the default certificate for this key.
   * @throw Pib::Error the default certificate does not exist.
   */
  const Certificate&
  getDefaultCertificate() const;

  /**
   * @brief Returns true if the instance is valid.
   */
  explicit
  operator bool() const noexcept;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations are accessible only by KeyChain
  /**
   * @brief Add @p certificate.
   *
   * If no default certificate is set before, the new certificate will be set as the default
   * certificate of the key.
   * If a certificate with the same name (excluding implicit digest) already exists, it will
   * be overwritten.
   *
   * @throw std::invalid_argument the certificate name does not match the key name.
   */
  void
  addCertificate(const Certificate& certificate) const;

  /**
   * @brief Remove a certificate with @p certName.
   * @throw std::invalid_argument @p certName does not match the key name.
   */
  void
  removeCertificate(const Name& certName) const;

  /**
   * @brief Set an existing certificate with name @p certName as the default certificate.
   * @return The default certificate.
   * @throw std::invalid_argument @p certName does not match the key name.
   * @throw Pib::Error the certificate does not exist.
   */
  const Certificate&
  setDefaultCertificate(const Name& certName) const;

  /**
   * @brief Add/replace @p certificate and set it as the default certificate.
   * @throw std::invalid_argument the certificate name does not match the key name.
   */
  void
  setDefaultCertificate(const Certificate& certificate) const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // private interface for KeyContainer
  explicit
  Key(weak_ptr<KeyImpl> impl) noexcept;

private:
  /**
   * @brief Check the validity of the instance
   * @return a shared_ptr when the instance is valid
   * @throw std::domain_error the instance is invalid
   */
  shared_ptr<KeyImpl>
  lock() const;

  bool
  equals(const Key& other) const noexcept;

  // NOTE
  // The following "hidden friend" non-member operators are available
  // via argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const Key& lhs, const Key& rhs)
  {
    return lhs.equals(rhs);
  }

  friend bool
  operator!=(const Key& lhs, const Key& rhs)
  {
    return !lhs.equals(rhs);
  }

  friend std::ostream&
  operator<<(std::ostream& os, const Key& key)
  {
    if (key)
      return os << key.getName();
    else
      return os << "(empty)";
  }

private:
  weak_ptr<KeyImpl> m_impl;

  friend KeyChain;
  friend KeyContainer;
};

} // namespace pib

using pib::Key;

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

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_PIB_KEY_HPP
