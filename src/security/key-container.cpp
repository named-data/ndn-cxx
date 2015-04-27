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

#include "key-container.hpp"
#include "pib-impl.hpp"

namespace ndn {
namespace security {

KeyContainer::const_iterator::const_iterator(const Name& identity,
                                             std::set<name::Component>::const_iterator it,
                                             shared_ptr<PibImpl> impl)
  : m_identity(identity)
  , m_it(it)
  , m_impl(impl)
{
}

Key
KeyContainer::const_iterator::operator*()
{
  return Key(m_identity, *m_it, m_impl);
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
  return (m_impl == other.m_impl && m_identity == other.m_identity && m_it == other.m_it);
}

bool
KeyContainer::const_iterator::operator!=(const const_iterator& other)
{
  return !(*this == other);
}

KeyContainer::KeyContainer()
{
}

KeyContainer::KeyContainer(const Name& identity,
                           std::set<name::Component>&& keyIds,
                           shared_ptr<PibImpl> impl)
  : m_identity(identity)
  , m_keyIds(keyIds)
  , m_impl(impl)
{
}

KeyContainer::const_iterator
KeyContainer::begin() const
{
  return const_iterator(m_identity, m_keyIds.begin(), m_impl);
}

KeyContainer::const_iterator
KeyContainer::end() const
{
  return const_iterator(m_identity, m_keyIds.end(), m_impl);
}

KeyContainer::const_iterator
KeyContainer::find(const name::Component& keyId) const
{
  return const_iterator(m_identity, m_keyIds.find(keyId), m_impl);
}

size_t
KeyContainer::size() const
{
  return m_keyIds.size();
}

} // namespace security
} // namespace ndn
