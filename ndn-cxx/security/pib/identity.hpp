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

#ifndef NDN_CXX_SECURITY_PIB_IDENTITY_HPP
#define NDN_CXX_SECURITY_PIB_IDENTITY_HPP

#include "ndn-cxx/security/pib/key-container.hpp"

namespace ndn::security {

class KeyChain;

namespace pib {

class IdentityContainer;
class IdentityImpl;

/**
 * @brief Frontend handle for an identity in the PIB.
 *
 * Identity is at the top level in PIB's Identity-Key-Certificate hierarchy.  An identity has a
 * name, and contains zero or more keys, at most one of which is set as the default key of that
 * identity.  The properties of a key can be accessed after obtaining a Key object.
 */
class Identity
{
public:
  /**
   * @brief Default constructor.
   *
   * An Identity created using this default constructor is just a placeholder.
   * You can obtain an actual instance from Pib::getIdentity(). A typical
   * usage would be for exception handling:
   *
   * @code
   * Identity id;
   * try {
   *   id = pib.getIdentity(...);
   * }
   * catch (const Pib::Error&) {
   *   ...
   * }
   * @endcode
   *
   * An instance created using this constructor is invalid. Calling a member
   * function on an invalid Identity instance may throw an std::domain_error.
   */
  Identity() noexcept;

  /**
   * @brief Return the name of the identity.
   */
  const Name&
  getName() const;

  /**
   * @brief Obtain a handle to the key with the given name.
   * @throw std::invalid_argument @p keyName does not match the identity.
   * @throw Pib::Error the key does not exist.
   */
  Key
  getKey(const Name& keyName) const;

  /**
   * @brief Return all the keys of this identity.
   */
  const KeyContainer&
  getKeys() const;

  /**
   * @brief Return the default key for this identity.
   * @throw Pib::Error the default key does not exist.
   */
  Key
  getDefaultKey() const;

  /**
   * @brief Returns true if the instance is valid.
   */
  explicit
  operator bool() const noexcept;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations are accessible only by KeyChain
  /**
   * @brief Add @p key (in PKCS #8 format) with name @p keyName.
   *
   * If no default key is set before, the new key will be set as the default key of the identity.
   * If a key with the same name already exists, it will be overwritten.
   *
   * @return Handle to the added key.
   * @throw std::invalid_argument @p keyName does not match the identity.
   */
  Key
  addKey(span<const uint8_t> key, const Name& keyName) const;

  /**
   * @brief Remove key with given name.
   * @throw std::invalid_argument @p keyName does not match the identity.
   */
  void
  removeKey(const Name& keyName) const;

  /**
   * @brief Set an existing key with name @p keyName as the default key.
   * @return Handle to the default key.
   * @throw std::invalid_argument @p keyName does not match the identity.
   * @throw Pib::Error the key does not exist.
   */
  Key
  setDefaultKey(const Name& keyName) const;

  /**
   * @brief Add/replace @p key with name @p keyName and set it as the default key.
   * @return Handle to the default key.
   * @throw std::invalid_argument @p keyName does not match the identity.
   */
  Key
  setDefaultKey(span<const uint8_t> key, const Name& keyName) const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // private interface for IdentityContainer
  explicit
  Identity(weak_ptr<IdentityImpl> impl) noexcept;

private:
  /**
   * @brief Check the validity of the instance
   * @return a shared_ptr when the instance is valid
   * @throw std::domain_error the instance is invalid
   */
  shared_ptr<IdentityImpl>
  lock() const;

  bool
  equals(const Identity& other) const noexcept;

  // NOTE
  // The following "hidden friend" non-member operators are available
  // via argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const Identity& lhs, const Identity& rhs)
  {
    return lhs.equals(rhs);
  }

  friend bool
  operator!=(const Identity& lhs, const Identity& rhs)
  {
    return !lhs.equals(rhs);
  }

  friend std::ostream&
  operator<<(std::ostream& os, const Identity& id)
  {
    if (id)
      return os << id.getName();
    else
      return os << "(empty)";
  }

private:
  weak_ptr<IdentityImpl> m_impl;

  friend KeyChain;
  friend IdentityContainer;
};

} // namespace pib

using pib::Identity;

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_PIB_IDENTITY_HPP
