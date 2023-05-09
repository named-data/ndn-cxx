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

#include "ndn-cxx/security/pib/identity.hpp"
#include "ndn-cxx/security/pib/impl/identity-impl.hpp"

namespace ndn::security::pib {

Identity::Identity() noexcept = default;

Identity::Identity(weak_ptr<IdentityImpl> impl) noexcept
  : m_impl(std::move(impl))
{
}

const Name&
Identity::getName() const
{
  return lock()->getName();
}

Key
Identity::addKey(span<const uint8_t> key, const Name& keyName) const
{
  return lock()->addKey(key, keyName);
}

void
Identity::removeKey(const Name& keyName) const
{
  lock()->removeKey(keyName);
}

Key
Identity::getKey(const Name& keyName) const
{
  return lock()->getKey(keyName);
}

const KeyContainer&
Identity::getKeys() const
{
  return lock()->getKeys();
}

Key
Identity::setDefaultKey(const Name& keyName) const
{
  return lock()->setDefaultKey(keyName);
}

Key
Identity::setDefaultKey(span<const uint8_t> key, const Name& keyName) const
{
  return lock()->setDefaultKey(key, keyName);
}

Key
Identity::getDefaultKey() const
{
  return lock()->getDefaultKey();
}

Identity::operator bool() const noexcept
{
  return !m_impl.expired();
}

shared_ptr<IdentityImpl>
Identity::lock() const
{
  auto impl = m_impl.lock();
  if (impl == nullptr) {
    NDN_THROW(std::domain_error("Invalid PIB identity instance"));
  }
  return impl;
}

bool
Identity::equals(const Identity& other) const noexcept
{
  return !this->m_impl.owner_before(other.m_impl) &&
         !other.m_impl.owner_before(this->m_impl);
}

} // namespace ndn::security::pib
