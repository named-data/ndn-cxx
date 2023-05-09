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

#include "ndn-cxx/security/pib/pib.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn::security::pib {

NDN_LOG_INIT(ndn.security.Pib);

Pib::Pib(const std::string& locator, shared_ptr<PibImpl> impl)
  : m_locator(locator)
  , m_impl(std::move(impl))
  , m_identities(m_impl)
{
  BOOST_ASSERT(m_impl != nullptr);
}

Pib::~Pib() = default;

std::string
Pib::getTpmLocator() const
{
  return m_impl->getTpmLocator();
}

void
Pib::setTpmLocator(const std::string& tpmLocator)
{
  if (tpmLocator == m_impl->getTpmLocator()) {
    return;
  }

  NDN_LOG_DEBUG("Resetting TPM locator to " << tpmLocator);
  reset();
  m_impl->setTpmLocator(tpmLocator);
}

void
Pib::reset()
{
  m_impl->setTpmLocator("");
  m_impl->clearIdentities();
  m_defaultIdentity = {};
  m_identities.reset();
}

Identity
Pib::addIdentity(const Name& identityName)
{
  BOOST_ASSERT(m_identities.isConsistent());
  return m_identities.add(identityName);
}

void
Pib::removeIdentity(const Name& identityName)
{
  BOOST_ASSERT(m_identities.isConsistent());

  if (m_defaultIdentity && m_defaultIdentity.getName() == identityName) {
    NDN_LOG_DEBUG("Removing default identity " << identityName);
    m_defaultIdentity = {};
  }
  m_identities.remove(identityName);
}

Identity
Pib::getIdentity(const Name& identity) const
{
  BOOST_ASSERT(m_identities.isConsistent());
  return m_identities.get(identity);
}

const IdentityContainer&
Pib::getIdentities() const
{
  BOOST_ASSERT(m_identities.isConsistent());
  return m_identities;
}

Identity
Pib::setDefaultIdentity(const Name& identityName)
{
  BOOST_ASSERT(m_identities.isConsistent());

  m_defaultIdentity = m_identities.add(identityName);
  m_impl->setDefaultIdentity(identityName);
  NDN_LOG_DEBUG("Default identity set to " << identityName);

  BOOST_ASSERT(m_defaultIdentity);
  return m_defaultIdentity;
}

Identity
Pib::getDefaultIdentity() const
{
  BOOST_ASSERT(m_identities.isConsistent());

  if (!m_defaultIdentity) {
    m_defaultIdentity = m_identities.get(m_impl->getDefaultIdentity());
    NDN_LOG_DEBUG("Caching default identity " << m_defaultIdentity.getName());
  }

  BOOST_ASSERT(m_defaultIdentity);
  BOOST_ASSERT(m_defaultIdentity.getName() == m_impl->getDefaultIdentity());
  return m_defaultIdentity;
}

} // namespace ndn::security::pib
