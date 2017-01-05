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

#include "pib.hpp"
#include "pib-impl.hpp"
#include "util/logger.hpp"

namespace ndn {
namespace security {
namespace pib {

NDN_LOG_INIT(ndn.security.pib.Pib);

Pib::Pib(const std::string& scheme, const std::string& location, shared_ptr<PibImpl> impl)
  : m_scheme(scheme)
  , m_location(location)
  , m_isDefaultIdentityLoaded(false)
  , m_identities(impl)
  , m_impl(impl)
{
  BOOST_ASSERT(impl != nullptr);
}

Pib::~Pib() = default;

std::string
Pib::getPibLocator() const
{
  return m_scheme + ":" + m_location;
}

void
Pib::setTpmLocator(const std::string& tpmLocator)
{
  if (tpmLocator == m_impl->getTpmLocator()) {
    return;
  }
  reset();
  m_impl->setTpmLocator(tpmLocator);
}

std::string
Pib::getTpmLocator() const
{
  std::string tpmLocator = m_impl->getTpmLocator();
  if (tpmLocator.empty()) {
    BOOST_THROW_EXCEPTION(Pib::Error("TPM info does not exist"));
  }
  return tpmLocator;
}

void
Pib::reset()
{
  m_impl->clearIdentities();
  m_impl->setTpmLocator("");
  m_isDefaultIdentityLoaded = false;
  m_identities.reset();
}

Identity
Pib::addIdentity(const Name& identity)
{
  BOOST_ASSERT(m_identities.isConsistent());

  return m_identities.add(identity);
}

void
Pib::removeIdentity(const Name& identity)
{
  BOOST_ASSERT(m_identities.isConsistent());

  if (m_isDefaultIdentityLoaded && m_defaultIdentity.getName() == identity) {
    m_isDefaultIdentityLoaded = false;
  }

  m_identities.remove(identity);
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

const Identity&
Pib::setDefaultIdentity(const Name& identityName)
{
  BOOST_ASSERT(m_identities.isConsistent());

  m_defaultIdentity = m_identities.add(identityName);
  m_isDefaultIdentityLoaded = true;
  NDN_LOG_DEBUG("Default identity is set to " << identityName);

  m_impl->setDefaultIdentity(identityName);
  return m_defaultIdentity;
}

const Identity&
Pib::getDefaultIdentity() const
{
  BOOST_ASSERT(m_identities.isConsistent());

  if (!m_isDefaultIdentityLoaded) {
    m_defaultIdentity = m_identities.get(m_impl->getDefaultIdentity());
    m_isDefaultIdentityLoaded = true;
    NDN_LOG_DEBUG("Default identity is " << m_defaultIdentity.getName());
  }

  BOOST_ASSERT(m_impl->getDefaultIdentity() == m_defaultIdentity.getName());

  return m_defaultIdentity;
}

} // namespace pib
} // namespace security
} // namespace ndn
