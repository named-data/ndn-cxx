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

#ifndef NDN_SECURITY_PIB_IDENTITY_HPP
#define NDN_SECURITY_PIB_IDENTITY_HPP

#include "key-container.hpp"

namespace ndn {
namespace security {
namespace pib {

/**
 * @brief represents an identity
 *
 * Identity is at the top level in PIB's Identity-Key-Certificate hierarchy.  An identity has a
 * Name, and contains zero or more keys, at most one of which is set as the default key of this
 * identity.  Properties of a key can be accessed after obtaining a Key object.
 *
 * @throw Pib::Error when underlying implementation has non-semantic error.
 */
class Identity
{
public:
  /**
   * @brief Default Constructor
   *
   * Identity created using this default constructor is just a place holder.
   * It must obtain an actual instance from Pib::getIdentity(...).  A typical
   * usage would be for exception handling:
   *
   *   Identity id;
   *   try {
   *     id = pib.getIdentity(...);
   *   }
   *   catch (const Pib::Error&) {
   *     ...
   *   }
   *
   * An Identity instance created using this constructor is invalid. Calling a
   * member method on an invalid Identity instance may cause an std::domain_error.
   */
  Identity();

  /**
   * @brief Create an Identity with @p identityName.
   *
   * @param identityName The name of the Identity.
   * @param impl The backend implementation.
   * @param needInit If true, create the identity in backend when the identity does not exist.
   *                 Otherwise, throw Pib::Error when the identity does not exist.
   */
  Identity(const Name& identityName, shared_ptr<PibImpl> impl, bool needInit = false);

  /// @brief Get the name of the identity.
  const Name&
  getName() const;

  /**
   * @brief Get a key with @p keyName.
   * @throw Pib::Error if the key does not exist.
   */
  Key
  getKey(const Name& keyName) const;

  /// @brief Get all the keys for this Identity.
  const KeyContainer&
  getKeys() const;

  /**
   * @brief Get the default key for this Identity.
   *
   * @throws Pib::Error if the default key does not exist.
   */
  Key&
  getDefaultKey() const;

  /// @brief Check if the Identity instance is valid
  operator bool() const;

  /// @brief Check if the Identity instance is invalid
  bool
  operator!() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations should be private
  /**
   * @brief Add a @p key (in PKCS#8 format) with @p keyName.
   *
   * If the key already exists, do nothing.
   *
   * If no default key is set before, the new key will be set as the default key of the identity.
   *
   * @return the added key or existing key with the same key id.
   */
  Key
  addKey(const uint8_t* key, size_t keyLen, const Name& keyName);

  /**
   * @brief Remove a key with @p keyName
   */
  void
  removeKey(const Name& keyName);

  /**
   * @brief Set the key with id @p keyName.
   *
   * @return The default key
   * @throws Pib::Error if the key does not exist.
   */
  Key&
  setDefaultKey(const Name& keyName);

  /**
   * @brief Set the default key with @p keyName (in PKCS#8 format).
   *
   * If the key does not exist, add the key and set it as the default of the Identity.
   * If the key exists, simply set it as the default key of the Identity.
   *
   * @param key The public key to add.
   * @param keyLen The length of the key.
   * @return the default key
   */
  Key&
  setDefaultKey(const uint8_t* key, size_t keyLen, const Name& keyName);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Check the validity of this instance
   *
   * @throws std::domain_error if the instance is invalid
   */
  void
  validityCheck() const;

private:
  Name m_name;

  mutable bool m_hasDefaultKey;
  mutable Key m_defaultKey;

  mutable bool m_needRefreshKeys;
  mutable KeyContainer m_keys;

  shared_ptr<PibImpl> m_impl;
};

} // namespace pib

using pib::Identity;

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_IDENTITY_HPP
