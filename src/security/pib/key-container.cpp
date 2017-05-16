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

#include "key-container.hpp"
#include "pib-impl.hpp"
#include "detail/key-impl.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace security {
namespace pib {

NDN_CXX_ASSERT_FORWARD_ITERATOR(KeyContainer::const_iterator);

KeyContainer::const_iterator::const_iterator()
  : m_container(nullptr)
{
}

KeyContainer::const_iterator::const_iterator(std::set<Name>::const_iterator it,
                                             const KeyContainer& container)
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

KeyContainer::const_iterator&
KeyContainer::const_iterator::operator++()
{
  ++m_it;
  return *this;
}

KeyContainer::const_iterator
KeyContainer::const_iterator::operator++(int)
{
  const_iterator it(*this);
  ++m_it;
  return it;
}

bool
KeyContainer::const_iterator::operator==(const const_iterator& other)
{
  bool isThisEnd = m_container == nullptr || m_it == m_container->m_keyNames.end();
  bool isOtherEnd = other.m_container == nullptr || other.m_it == other.m_container->m_keyNames.end();
  return ((isThisEnd || isOtherEnd) ?
          (isThisEnd == isOtherEnd) :
          m_container->m_pib == other.m_container->m_pib && m_it == other.m_it);
}

bool
KeyContainer::const_iterator::operator!=(const const_iterator& other)
{
  return !(*this == other);
}

KeyContainer::KeyContainer(const Name& identity, shared_ptr<PibImpl> pibImpl)
  : m_identity(identity)
  , m_pib(std::move(pibImpl))
{
  BOOST_ASSERT(m_pib != nullptr);
  m_keyNames = m_pib->getKeysOfIdentity(identity);
}

KeyContainer::const_iterator
KeyContainer::begin() const
{
  return const_iterator(m_keyNames.begin(), *this);
}

KeyContainer::const_iterator
KeyContainer::end() const
{
  return const_iterator();
}

KeyContainer::const_iterator
KeyContainer::find(const Name& keyName) const
{
  return const_iterator(m_keyNames.find(keyName), *this);
}

size_t
KeyContainer::size() const
{
  return m_keyNames.size();
}

Key
KeyContainer::add(const uint8_t* key, size_t keyLen, const Name& keyName)
{
  if (m_identity != v2::extractIdentityFromKeyName(keyName)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Key name `" + keyName.toUri() + "` does not match identity "
                                                "`" + m_identity.toUri() + "`"));
  }

  m_keyNames.insert(keyName);
  m_keys[keyName] = shared_ptr<detail::KeyImpl>(new detail::KeyImpl(keyName, key, keyLen, m_pib));

  return get(keyName);
}

void
KeyContainer::remove(const Name& keyName)
{
  if (m_identity != v2::extractIdentityFromKeyName(keyName)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Key name `" + keyName.toUri() + "` does not match identity "
                                                "`" + m_identity.toUri() + "`"));
  }

  m_keyNames.erase(keyName);
  m_keys.erase(keyName);
  m_pib->removeKey(keyName);
}

Key
KeyContainer::get(const Name& keyName) const
{
  if (m_identity != v2::extractIdentityFromKeyName(keyName)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Key name `" + keyName.toUri() + "` does not match identity "
                                                "`" + m_identity.toUri() + "`"));
  }

  shared_ptr<detail::KeyImpl> key;
  auto it = m_keys.find(keyName);

  if (it != m_keys.end()) {
    key = it->second;
  }
  else {
    key = shared_ptr<detail::KeyImpl>(new detail::KeyImpl(keyName, m_pib));
    m_keys[keyName] = key;
  }

  return Key(key);
}

bool
KeyContainer::isConsistent() const
{
  return m_keyNames == m_pib->getKeysOfIdentity(m_identity);
}

} // namespace pib
} // namespace security
} // namespace ndn
