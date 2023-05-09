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

#include "ndn-cxx/security/pib/identity-container.hpp"
#include "ndn-cxx/security/pib/impl/identity-impl.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn::security::pib {

NDN_LOG_INIT(ndn.security.IdentityContainer);

IdentityContainer::const_iterator::const_iterator(NameSet::const_iterator it,
                                                  const IdentityContainer& container) noexcept
  : m_it(it)
  , m_container(&container)
{
}

Identity
IdentityContainer::const_iterator::operator*()
{
  BOOST_ASSERT(m_container != nullptr);
  return m_container->get(*m_it);
}

bool
IdentityContainer::const_iterator::operator==(const const_iterator& other) const
{
  bool isThisEnd = m_container == nullptr || m_it == m_container->m_identityNames.end();
  bool isOtherEnd = other.m_container == nullptr || other.m_it == other.m_container->m_identityNames.end();
  if (isThisEnd)
    return isOtherEnd;
  return !isOtherEnd && m_container->m_pib == other.m_container->m_pib && m_it == other.m_it;
}

IdentityContainer::IdentityContainer(shared_ptr<PibImpl> pibImpl)
  : m_pib(std::move(pibImpl))
{
  BOOST_ASSERT(m_pib != nullptr);
  m_identityNames = m_pib->getIdentities();
}

IdentityContainer::const_iterator
IdentityContainer::find(const Name& identity) const
{
  return {m_identityNames.find(identity), *this};
}

Identity
IdentityContainer::add(const Name& identityName)
{
  bool didInsert = m_identityNames.insert(identityName).second;
  if (!didInsert) {
    // identity already exists
    return get(identityName);
  }

  NDN_LOG_DEBUG("Adding " << identityName);
  m_pib->addIdentity(identityName);
  auto ret = m_identities.emplace(identityName,
                                  std::make_shared<IdentityImpl>(identityName, m_pib));
  // consistency check
  BOOST_ASSERT(ret.second);

  return Identity(ret.first->second);
}

void
IdentityContainer::remove(const Name& identityName)
{
  if (m_identityNames.erase(identityName) > 0) {
    NDN_LOG_DEBUG("Removing " << identityName);
    m_identities.erase(identityName);
  }
  else {
    // consistency check
    BOOST_ASSERT(m_identities.find(identityName) == m_identities.end());
  }
  m_pib->removeIdentity(identityName);
}

Identity
IdentityContainer::get(const Name& identityName) const
{
  if (auto it = m_identities.find(identityName); it != m_identities.end()) {
    return Identity(it->second);
  }

  // check that the identity exists in the backend
  if (!m_pib->hasIdentity(identityName)) {
    NDN_THROW(Pib::Error("Identity `" + identityName.toUri() + "` does not exist"));
  }

  auto id = std::make_shared<IdentityImpl>(identityName, m_pib);
  m_identities[identityName] = id;
  return Identity(id);
}

void
IdentityContainer::reset()
{
  NDN_LOG_DEBUG("Reloading");
  m_identities.clear();
  m_identityNames = m_pib->getIdentities();
}

bool
IdentityContainer::isConsistent() const
{
  return m_identityNames == m_pib->getIdentities();
}

} // namespace ndn::security::pib
