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

#include "ndn-cxx/security/pib/impl/identity-impl.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn::security::pib {

NDN_LOG_INIT(ndn.security.Identity);

IdentityImpl::IdentityImpl(const Name& identityName, shared_ptr<PibImpl> pibImpl)
  : m_name(identityName)
  , m_pib(std::move(pibImpl))
  , m_keys(m_name, m_pib)
{
  BOOST_ASSERT(m_pib != nullptr);
  BOOST_ASSERT(m_pib->hasIdentity(m_name));
}

Key
IdentityImpl::addKey(span<const uint8_t> key, const Name& keyName)
{
  BOOST_ASSERT(m_keys.isConsistent());
  return m_keys.add(key, keyName);
}

void
IdentityImpl::removeKey(const Name& keyName)
{
  BOOST_ASSERT(m_keys.isConsistent());

  if (m_defaultKey && m_defaultKey.getName() == keyName) {
    NDN_LOG_DEBUG("Removing default key " << keyName);
    m_defaultKey = {};
  }
  m_keys.remove(keyName);
}

Key
IdentityImpl::setDefaultKey(Key key)
{
  BOOST_ASSERT(m_keys.isConsistent());
  BOOST_ASSERT(key);

  m_defaultKey = std::move(key);
  m_pib->setDefaultKeyOfIdentity(m_name, m_defaultKey.getName());
  NDN_LOG_DEBUG("Default key set to " << m_defaultKey.getName());

  return m_defaultKey;
}

Key
IdentityImpl::getDefaultKey() const
{
  BOOST_ASSERT(m_keys.isConsistent());

  if (!m_defaultKey) {
    m_defaultKey = m_keys.get(m_pib->getDefaultKeyOfIdentity(m_name));
    NDN_LOG_DEBUG("Caching default key " << m_defaultKey.getName());
  }

  BOOST_ASSERT(m_defaultKey);
  BOOST_ASSERT(m_defaultKey.getName() == m_pib->getDefaultKeyOfIdentity(m_name));
  return m_defaultKey;
}

} // namespace ndn::security::pib
