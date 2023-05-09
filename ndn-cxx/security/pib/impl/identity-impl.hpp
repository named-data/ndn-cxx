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

#ifndef NDN_CXX_SECURITY_PIB_IMPL_IDENTITY_IMPL_HPP
#define NDN_CXX_SECURITY_PIB_IMPL_IDENTITY_IMPL_HPP

#include "ndn-cxx/security/pib/key-container.hpp"

namespace ndn::security::pib {

class PibImpl;

/**
 * @brief Backend instance of Identity.
 *
 * An Identity has only one backend instance, but may have multiple frontend handles.
 * Each frontend handle is associated with the only one backend IdentityImpl.
 *
 * @throw PibImpl::Error When the underlying implementation has a non-semantic error.
 * @sa Identity
 */
class IdentityImpl : noncopyable
{
public:
  /**
   * @brief Create an identity with name @p identityName.
   *
   * @param identityName The name of the identity.
   * @param pibImpl The PIB backend implementation.
   * @pre The identity must exist in the backend.
   */
  IdentityImpl(const Name& identityName, shared_ptr<PibImpl> pibImpl);

  // See security::pib::Identity for the documentation of the following methods

  const Name&
  getName() const
  {
    return m_name;
  }

  Key
  addKey(span<const uint8_t> key, const Name& keyName);

  void
  removeKey(const Name& keyName);

  Key
  getKey(const Name& keyName) const
  {
    BOOST_ASSERT(m_keys.isConsistent());
    return m_keys.get(keyName);
  }

  const KeyContainer&
  getKeys() const
  {
    BOOST_ASSERT(m_keys.isConsistent());
    return m_keys;
  }

  Key
  setDefaultKey(const Name& keyName)
  {
    return setDefaultKey(m_keys.get(keyName));
  }

  Key
  setDefaultKey(span<const uint8_t> key, const Name& keyName)
  {
    return setDefaultKey(m_keys.add(key, keyName));
  }

  Key
  getDefaultKey() const;

private:
  Key
  setDefaultKey(Key key);

private:
  const Name m_name;

  const shared_ptr<PibImpl> m_pib;

  KeyContainer m_keys;
  mutable Key m_defaultKey;
};

} // namespace ndn::security::pib

#endif // NDN_CXX_SECURITY_PIB_IMPL_IDENTITY_IMPL_HPP
