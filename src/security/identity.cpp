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

#include "identity.hpp"
#include "pib-impl.hpp"
#include "pib.hpp"

namespace ndn {
namespace security {

const name::Component Identity::EMPTY_KEY_ID;

Identity::Identity()
  : m_hasDefaultKey(false)
  , m_needRefreshKeys(false)
  , m_impl(nullptr)
{
}

Identity::Identity(const Name& identityName, shared_ptr<PibImpl> impl, bool needInit)
  : m_name(identityName)
  , m_hasDefaultKey(false)
  , m_needRefreshKeys(true)
  , m_impl(impl)
{
  validityCheck();

  if (needInit)
    m_impl->addIdentity(m_name);
  else if (!m_impl->hasIdentity(m_name))
    BOOST_THROW_EXCEPTION(Pib::Error("Identity: " + m_name.toUri() + " does not exist"));
}

const Name&
Identity::getName() const
{
  validityCheck();

  return m_name;
}

Key
Identity::addKey(const PublicKey& publicKey, const name::Component& keyId)
{
  validityCheck();

  name::Component actualKeyId = keyId;
  if (actualKeyId == EMPTY_KEY_ID) {
    const Block& digest = publicKey.computeDigest();
    actualKeyId = name::Component(digest.wire(), digest.size());
  }

  if (!m_needRefreshKeys && m_keys.find(actualKeyId) == m_keys.end()) {
    // if we have already loaded all the keys, but the new key is not one of them
    // the KeyContainer should be refreshed
    m_needRefreshKeys = true;
  }

  return Key(m_name, actualKeyId, publicKey, m_impl);
}

void
Identity::removeKey(const name::Component& keyId)
{
  validityCheck();

  if (m_hasDefaultKey && m_defaultKey.getKeyId() == keyId)
    m_hasDefaultKey = false;

  m_impl->removeKey(m_name, keyId);
  m_needRefreshKeys = true;
}

Key
Identity::getKey(const name::Component& keyId) const
{
  validityCheck();

  return Key(m_name, keyId, m_impl);
}

const KeyContainer&
Identity::getKeys() const
{
  validityCheck();

  if (m_needRefreshKeys) {
    m_keys = std::move(KeyContainer(m_name, m_impl->getKeysOfIdentity(m_name), m_impl));
    m_needRefreshKeys = false;
  }

  return m_keys;
}

Key&
Identity::setDefaultKey(const name::Component& keyId)
{
  validityCheck();

  m_defaultKey = std::move(Key(m_name, keyId, m_impl));
  m_hasDefaultKey = true;

  m_impl->setDefaultKeyOfIdentity(m_name, keyId);
  return m_defaultKey;
}

Key&
Identity::setDefaultKey(const PublicKey& publicKey, const name::Component& keyId)
{
  const Key& keyEntry = addKey(publicKey, keyId);
  return setDefaultKey(keyEntry.getKeyId());
}

Key&
Identity::getDefaultKey() const
{
  validityCheck();

  if (!m_hasDefaultKey) {
    m_defaultKey = std::move(Key(m_name, m_impl->getDefaultKeyOfIdentity(m_name), m_impl));
    m_hasDefaultKey = true;
  }

  return m_defaultKey;
}

Identity::operator bool() const
{
  return !(this->operator!());
}

bool
Identity::operator!() const
{
  return (m_impl == nullptr);
}

void
Identity::validityCheck() const
{
  if (m_impl == nullptr)
    BOOST_THROW_EXCEPTION(std::domain_error("Invalid Identity instance"));
}

} // namespace security
} // namespace ndn
