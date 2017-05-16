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

#ifndef NDN_SECURITY_PIB_DETAIL_IDENTITY_IMPL_HPP
#define NDN_SECURITY_PIB_DETAIL_IDENTITY_IMPL_HPP

#include "../key-container.hpp"

namespace ndn {
namespace security {
namespace pib {

class PibImpl;

namespace detail {

/**
 * @brief Backend instance of Identity
 *
 * An Identity has only one backend instance, but may have multiple frontend handles.
 * Each frontend handle is associated with the only one backend IdentityImpl.
 *
 * @throw PibImpl::Error when underlying implementation has non-semantic error.
 */
class IdentityImpl : noncopyable
{
public:
  /**
   * @brief Create an Identity with @p identityName.
   *
   * @param identityName The name of the Identity.
   * @param pibImpl The PIB backend implementation.
   * @param needInit If true, create the identity in backend when the identity does not exist.
   *                 Otherwise, throw Pib::Error when the identity does not exist.
   */
  IdentityImpl(const Name& identityName, shared_ptr<PibImpl> pibImpl, bool needInit = false);

  /**
   * @brief Get the name of the identity.
   */
  const Name&
  getName() const
  {
    return m_name;
  }

  /**
   * @brief Add a @p key of @p keyLen bytes with @p keyName (in PKCS#8 format).
   *
   * If no default key is set before, the new key will be set as the default key of the identity.
   * If a key with the same name already exists, overwrite the key.
   *
   * @return the added key.
   * @throw std::invalid_argument key name does not match identity
   */
  Key
  addKey(const uint8_t* key, size_t keyLen, const Name& keyName);

  /**
   * @brief Remove a key with @p keyName
   * @throw std::invalid_argument @p keyName does not match identity
   */
  void
  removeKey(const Name& keyName);

  /**
   * @brief Get a key with id @p keyName.
   *
   * @throw std::invalid_argument @p keyName does not match identity
   * @throw Pib::Error the key does not exist.
   */
  Key
  getKey(const Name& keyName) const;

  /**
   * @brief Get all keys for this Identity.
   */
  const KeyContainer&
  getKeys() const;

  /**
   * @brief Set the key with id @p keyName.
   * @throw std::invalid_argument @p keyName does not match identity
   * @throw Pib::Error the key does not exist.
   * @return The default key
   */
  const Key&
  setDefaultKey(const Name& keyName);

  /**
   * @brief Add @p key of @p keyLen bytes with @p keyName and set it as the default key
   * @throw std::invalid_argument @p keyName does not match identity
   * @throw Pib::Error the key with the same name already exists
   * @return the default key
   */
  const Key&
  setDefaultKey(const uint8_t* key, size_t keyLen, const Name& keyName);

  /**
   * @brief Get the default key for this Identity.
   * @throw Pib::Error the default key does not exist.
   */
  const Key&
  getDefaultKey() const;

private:
  Name m_name;

  shared_ptr<PibImpl> m_pib;

  KeyContainer m_keys;
  mutable bool m_isDefaultKeyLoaded;
  mutable Key m_defaultKey;
};

} // namespace detail
} // namespace pib
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_DETAIL_IDENTITY_IMPL_HPP
