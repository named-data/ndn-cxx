/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "identity.hpp"
#include "detail/identity-impl.hpp"

namespace ndn {
namespace security {
namespace pib {

Identity::Identity() = default;

Identity::Identity(weak_ptr<detail::IdentityImpl> impl)
  : m_impl(impl)
{
}

const Name&
Identity::getName() const
{
  return lock()->getName();
}

Key
Identity::addKey(const uint8_t* key, size_t keyLen, const Name& keyName) const
{
  return lock()->addKey(key, keyLen, keyName);
}

void
Identity::removeKey(const Name& keyName) const
{
  return lock()->removeKey(keyName);
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

const Key&
Identity::setDefaultKey(const Name& keyName) const
{
  return lock()->setDefaultKey(keyName);
}

const Key&
Identity::setDefaultKey(const uint8_t* key, size_t keyLen, const Name& keyName) const
{
  return lock()->setDefaultKey(key, keyLen, keyName);
}

const Key&
Identity::getDefaultKey() const
{
  return lock()->getDefaultKey();
}

Identity::operator bool() const
{
  return !m_impl.expired();
}

shared_ptr<detail::IdentityImpl>
Identity::lock() const
{
  auto impl = m_impl.lock();

  if (impl == nullptr)
    BOOST_THROW_EXCEPTION(std::domain_error("Invalid Identity instance"));

  return impl;
}

bool
operator!=(const Identity& lhs, const Identity& rhs)
{
  return lhs.m_impl.owner_before(rhs.m_impl) || rhs.m_impl.owner_before(lhs.m_impl);
}

std::ostream&
operator<<(std::ostream& os, const Identity& id)
{
  if (id) {
    os << id.getName();
  }
  else {
    os << "(empty)";
  }
  return os;
}

} // namespace pib
} // namespace security
} // namespace ndn
