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

#ifndef NDN_SECURITY_PIB_KEY_HPP
#define NDN_SECURITY_PIB_KEY_HPP

#include "certificate-container.hpp"
#include "../security-common.hpp"

namespace ndn {
namespace security {

namespace v2 {
class KeyChain;
} // namespace v2

namespace pib {

namespace detail {
class KeyImpl;
} // namespace detail

/**
 * @brief A frontend handle of a key instance
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
   * It can obtain an actual instance from Identity::getKey(...).  A typical
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
   * @brief Create a Key with a backend implementation @p impl.
   *
   * This method should only be used by KeyContainer.
   */
  explicit
  Key(weak_ptr<detail::KeyImpl> impl);

  /**
   * @brief Get key name.
   */
  const Name&
  getName() const;

  /**
   * @brief Get the name of the belonging identity.
   */
  const Name&
  getIdentity() const;

  /**
   * @brief Get key type.
   */
  KeyType
  getKeyType() const;

  /**
   * @brief Get public key bits.
   */
  const Buffer&
  getPublicKey() const;

  /**
   * @brief Get a certificate with @p certName
   * @throw std::invalid_argument @p certName does not match key name
   * @throw Pib::Error the certificate does not exist.
   */
  v2::Certificate
  getCertificate(const Name& certName) const;

  /**
   * @brief Get all certificates for this key.
   */
  const CertificateContainer&
  getCertificates() const;

  /**
   * @brief Get the default certificate for this Key.
   * @throw Pib::Error the default certificate does not exist.
   */
  const v2::Certificate&
  getDefaultCertificate() const;

  /**
   * @brief Check if the Key instance is valid.
   */
  explicit
  operator bool() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations should be private
  /**
   * @brief Add @p certificate.
   * @throw std::invalid_argument certificate name does not match key name
   *
   * If a certificate with the same name (without implicit digest) already exists, overwrite
   * the certificate.
   */
  void
  addCertificate(const v2::Certificate& certificate) const;

  /**
   * @brief Remove a certificate with @p certName
   * @throw std::invalid_argument @p certName does not match key name
   */
  void
  removeCertificate(const Name& certName) const;

  /**
   * @brief Set an existing certificate with @p certName as the default certificate
   * @throw std::invalid_argument @p certName does not match key name
   * @throw Pib::Error the certificate does not exist.
   * @return the default certificate
   */
  const v2::Certificate&
  setDefaultCertificate(const Name& certName) const;

  /**
   * @brief Add @p certificate and set it as the default certificate of the key
   * @throw std::invalid_argument @p certificate does not match key name
   * @return the default certificate
   */
  const v2::Certificate&
  setDefaultCertificate(const v2::Certificate& certificate) const;

private:
  /**
   * @brief Check the validity of the instance
   * @return a shared_ptr when the instance is valid
   * @throw std::domain_error the instance is invalid
   */
  shared_ptr<detail::KeyImpl>
  lock() const;

private:
  weak_ptr<detail::KeyImpl> m_impl;

  friend class v2::KeyChain;
  friend bool operator!=(const Key&, const Key&);
};

bool
operator!=(const Key& lhs, const Key& rhs);

inline bool
operator==(const Key& lhs, const Key& rhs)
{
  return !(lhs != rhs);
}

std::ostream&
operator<<(std::ostream& os, const Key& key);

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
