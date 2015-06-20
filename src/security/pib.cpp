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

#include "pib.hpp"
#include "pib-impl.hpp"

namespace ndn {
namespace security {

Pib::Pib(const std::string scheme, const std::string& location, shared_ptr<PibImpl> impl)
  : m_scheme(scheme)
  , m_location(location)
  , m_hasDefaultIdentity(false)
  , m_needRefreshIdentities(true)
  , m_impl(impl)
{
}

Pib::~Pib()
{
}

std::string
Pib::getPibLocator() const
{
  return m_scheme + ":" + m_location;
}

void
Pib::setTpmLocator(const std::string& tpmLocator)
{
  m_impl->setTpmLocator(tpmLocator);
}

std::string
Pib::getTpmLocator() const
{
  return m_impl->getTpmLocator();
}

Identity
Pib::addIdentity(const Name& identity)
{
  if (!m_needRefreshIdentities && m_identities.find(identity) == m_identities.end()) {
    // if we have already loaded all the identities, but the new identity is not one of them
    // the IdentityContainer should be refreshed
    m_needRefreshIdentities = true;
  }
  return Identity(identity, m_impl, true);
}

void
Pib::removeIdentity(const Name& identity)
{
  if (m_hasDefaultIdentity && m_defaultIdentity.getName() == identity)
    m_hasDefaultIdentity = false;

  m_impl->removeIdentity(identity);
  m_needRefreshIdentities = true;
}

Identity
Pib::getIdentity(const Name& identity) const
{
  return Identity(identity, m_impl, false);
}

const IdentityContainer&
Pib::getIdentities() const
{
  if (m_needRefreshIdentities) {
    m_identities = std::move(IdentityContainer(m_impl->getIdentities(), m_impl));
    m_needRefreshIdentities = false;
  }

  return m_identities;
}

Identity&
Pib::setDefaultIdentity(const Name& identityName)
{
  m_defaultIdentity = addIdentity(identityName);
  m_hasDefaultIdentity = true;

  m_impl->setDefaultIdentity(identityName);
  return m_defaultIdentity;
}

Identity&
Pib::getDefaultIdentity() const
{
  if (!m_hasDefaultIdentity) {
    m_defaultIdentity = std::move(Identity(m_impl->getDefaultIdentity(), m_impl, false));
    m_hasDefaultIdentity = true;
  }

  return m_defaultIdentity;
}


} // namespace security
} // namespace ndn
