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

#ifndef NDN_SECURITY_IDENTITY_HPP
#define NDN_SECURITY_IDENTITY_HPP

#include "key-container.hpp"

namespace ndn {
namespace security {

class PibImpl;
class Pib;
class IdentityContainer;

/**
 * @brief represents an identity
 *
 * Identity is at the top level in PIB's Identity-Key-Certificate hierarchy.
 * An identity has a Name, and contains one or more keys, one of which is set
 * as the default key of this identity.  Properties of a key can be accessed
 * after obtaining a Key object.
 *
 * @throw PibImpl::Error when underlying implementation has non-semantic error.
 */
class Identity
{
public:
  friend class Pib;
  friend class IdentityContainer;
  friend class KeyChain;

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
   *   catch (Pib::Error&) {
   *     ...
   *   }
   *
   * An Identity instance created using the constructor is invalid. Calling a
   * member method on an invalid Identity instance may cause an std::domain_error.
   */
  Identity();

  /// @brief Get the name of the identity.
  const Name&
  getName() const;

  /**
   * @brief Get a key with id @keyId.
   *
   * @param identityName The name for the identity to get.
   * @throw Pib::Error if the identity does not exist.
   */
  Key
  getKey(const name::Component& keyId) const;

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
   * @brief Add a key.
   *
   * If the key already exists, do nothing.
   *
   * If no default key is set before, the new key will be set as the default key of the identity.
   *
   * @param publicKey The public key to add.
   * @param keyId The key id component of the new key to add.
   *              By default, the keyId will be set to the hash of the public key bits.
   * @return the added key or existing key with the same key id.
   */
  Key
  addKey(const PublicKey& publicKey, const name::Component& keyId = EMPTY_KEY_ID);

  /**
   * @brief Remove a key.
   *
   * @param keyId The key id component of the key to delete.
   */
  void
  removeKey(const name::Component& keyId);

  /**
   * @brief Set the key with id @p keyId as the default key.
   *
   * @param keyId The key id component of the default key.
   * @return The default key
   * @throws Pib::Error if the key does not exist.
   */
  Key&
  setDefaultKey(const name::Component& keyId);

  /**
   * @brief Set the default key.
   *
   * If the key does not exist, add the key and set it as the default of the Identity.
   * If the key exists, simply set it as the default key of the Identity.
   *
   * @param publicKey The public key to add.
   * @param keyId The key id component of the default key.
   * @return the default key
   */
  Key&
  setDefaultKey(const PublicKey& publicKey, const name::Component& keyId = EMPTY_KEY_ID);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Create an Identity with @p identityName.
   *
   * @param identityName The name of the Identity.
   * @param impl The backend implementation.
   * @param needInit If true, create the identity in backend when the identity does not exist.
   *                 Otherwise, throw Pib::Error when the identity does not exist.
   */
  Identity(const Name& identityName, shared_ptr<PibImpl> impl, bool needInit = false);

  /**
   * @brief Check the validity of this instance
   *
   * @throws std::domain_error if the instance is invalid
   */
  void
  validityCheck() const;

public:
  /**
   * @brief The default value of keyId when add a new key.
   *
   * An empty keyId implies that the key digest should be used as the actual keyId.
   */
  static const name::Component EMPTY_KEY_ID;

private:
  Name m_name;

  mutable bool m_hasDefaultKey;
  mutable Key m_defaultKey;

  mutable bool m_needRefreshKeys;
  mutable KeyContainer m_keys;

  shared_ptr<PibImpl> m_impl;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_IDENTITY_HPP
