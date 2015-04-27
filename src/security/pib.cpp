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
  return Identity(identity, m_impl, true);
}

void
Pib::removeIdentity(const Name& identity)
{
  m_impl->removeIdentity(identity);
}

Identity
Pib::getIdentity(const Name& identity)
{
  return Identity(identity, m_impl, false);
}

IdentityContainer
Pib::getIdentities() const
{
  return IdentityContainer(m_impl->getIdentities(), m_impl);
}

Identity
Pib::setDefaultIdentity(const Name& identityName)
{
  m_impl->setDefaultIdentity(identityName);
  return Identity(identityName, m_impl, true);
}

Identity
Pib::getDefaultIdentity()
{
  return Identity(m_impl->getDefaultIdentity(), m_impl, false);
}


} // namespace security
} // namespace ndn
