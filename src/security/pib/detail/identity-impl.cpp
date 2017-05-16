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

#include "identity-impl.hpp"
#include "../pib-impl.hpp"
#include "../pib.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace detail {

IdentityImpl::IdentityImpl(const Name& identityName, shared_ptr<PibImpl> pibImpl, bool needInit)
  : m_name(identityName)
  , m_pib(std::move(pibImpl))
  , m_keys(identityName, m_pib)
  , m_isDefaultKeyLoaded(false)
{
  BOOST_ASSERT(m_pib != nullptr);

  if (needInit) {
    m_pib->addIdentity(m_name);
  }
  else if (!m_pib->hasIdentity(m_name)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Identity " + m_name.toUri() + " does not exist"));
  }
}

Key
IdentityImpl::addKey(const uint8_t* key, size_t keyLen, const Name& keyName)
{
  BOOST_ASSERT(m_keys.isConsistent());
  return m_keys.add(key, keyLen, keyName);
}

void
IdentityImpl::removeKey(const Name& keyName)
{
  BOOST_ASSERT(m_keys.isConsistent());

  if (m_isDefaultKeyLoaded && m_defaultKey.getName() == keyName)
    m_isDefaultKeyLoaded = false;

  m_keys.remove(keyName);
}

Key
IdentityImpl::getKey(const Name& keyName) const
{
  BOOST_ASSERT(m_keys.isConsistent());
  return m_keys.get(keyName);
}

const KeyContainer&
IdentityImpl::getKeys() const
{
  BOOST_ASSERT(m_keys.isConsistent());
  return m_keys;
}

const Key&
IdentityImpl::setDefaultKey(const Name& keyName)
{
  BOOST_ASSERT(m_keys.isConsistent());

  m_defaultKey = m_keys.get(keyName);
  m_isDefaultKeyLoaded = true;
  m_pib->setDefaultKeyOfIdentity(m_name, keyName);
  return m_defaultKey;
}

const Key&
IdentityImpl::setDefaultKey(const uint8_t* key, size_t keyLen, const Name& keyName)
{
  addKey(key, keyLen, keyName);
  return setDefaultKey(keyName);
}

const Key&
IdentityImpl::getDefaultKey() const
{
  BOOST_ASSERT(m_keys.isConsistent());

  if (!m_isDefaultKeyLoaded) {
    m_defaultKey = m_keys.get(m_pib->getDefaultKeyOfIdentity(m_name));
    m_isDefaultKeyLoaded = true;
  }
  BOOST_ASSERT(m_pib->getDefaultKeyOfIdentity(m_name) == m_defaultKey.getName());

  return m_defaultKey;
}

} // namespace detail
} // namespace pib
} // namespace security
} // namespace ndn
