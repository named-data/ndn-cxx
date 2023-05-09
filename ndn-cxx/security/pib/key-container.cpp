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

#include "ndn-cxx/security/pib/key-container.hpp"
#include "ndn-cxx/security/pib/impl/key-impl.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn::security::pib {

NDN_LOG_INIT(ndn.security.KeyContainer);

KeyContainer::const_iterator::const_iterator(NameSet::const_iterator it,
                                             const KeyContainer& container) noexcept
  : m_it(it)
  , m_container(&container)
{
}

Key
KeyContainer::const_iterator::operator*()
{
  BOOST_ASSERT(m_container != nullptr);
  return m_container->get(*m_it);
}

bool
KeyContainer::const_iterator::operator==(const const_iterator& other) const
{
  bool isThisEnd = m_container == nullptr || m_it == m_container->m_keyNames.end();
  bool isOtherEnd = other.m_container == nullptr || other.m_it == other.m_container->m_keyNames.end();
  if (isThisEnd)
    return isOtherEnd;
  return !isOtherEnd && m_container->m_pib == other.m_container->m_pib && m_it == other.m_it;
}

KeyContainer::KeyContainer(const Name& identity, shared_ptr<PibImpl> pibImpl)
  : m_identity(identity)
  , m_pib(std::move(pibImpl))
{
  BOOST_ASSERT(m_pib != nullptr);
  m_keyNames = m_pib->getKeysOfIdentity(identity);
}

KeyContainer::const_iterator
KeyContainer::find(const Name& keyName) const
{
  return {m_keyNames.find(keyName), *this};
}

Key
KeyContainer::add(span<const uint8_t> keyBits, const Name& keyName)
{
  if (m_identity != extractIdentityFromKeyName(keyName)) {
    NDN_THROW(std::invalid_argument("Key name `" + keyName.toUri() + "` does not match identity "
                                    "`" + m_identity.toUri() + "`"));
  }

  bool isNew = m_keyNames.insert(keyName).second;
  NDN_LOG_DEBUG((isNew ? "Adding " : "Replacing ") << keyName);
  m_pib->addKey(m_identity, keyName, keyBits);

  auto key = std::make_shared<KeyImpl>(keyName, Buffer(keyBits.begin(), keyBits.end()), m_pib);
  m_keys[keyName] = key; // use insert_or_assign in C++17
  return Key(key);
}

void
KeyContainer::remove(const Name& keyName)
{
  if (m_identity != extractIdentityFromKeyName(keyName)) {
    NDN_THROW(std::invalid_argument("Key name `" + keyName.toUri() + "` does not match identity "
                                    "`" + m_identity.toUri() + "`"));
  }

  if (m_keyNames.erase(keyName) > 0) {
    NDN_LOG_DEBUG("Removing " << keyName);
    m_keys.erase(keyName);
  }
  else {
    // consistency check
    BOOST_ASSERT(m_keys.find(keyName) == m_keys.end());
  }
  m_pib->removeKey(keyName);
}

Key
KeyContainer::get(const Name& keyName) const
{
  if (m_identity != extractIdentityFromKeyName(keyName)) {
    NDN_THROW(std::invalid_argument("Key name `" + keyName.toUri() + "` does not match identity "
                                    "`" + m_identity.toUri() + "`"));
  }

  if (auto it = m_keys.find(keyName); it != m_keys.end()) {
    return Key(it->second);
  }

  // no need to check that the key exists in the backend
  // because getKeyBits will throw if it doesn't
  auto keyBits = m_pib->getKeyBits(keyName);

  auto key = std::make_shared<KeyImpl>(keyName, std::move(keyBits), m_pib);
  m_keys[keyName] = key;
  return Key(key);
}

bool
KeyContainer::isConsistent() const
{
  return m_keyNames == m_pib->getKeysOfIdentity(m_identity);
}

} // namespace ndn::security::pib
