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

#include "identity-container.hpp"
#include "pib-impl.hpp"
#include "detail/identity-impl.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace security {
namespace pib {

NDN_CXX_ASSERT_FORWARD_ITERATOR(IdentityContainer::const_iterator);

IdentityContainer::const_iterator::const_iterator()
  : m_container(nullptr)
{
}

IdentityContainer::const_iterator::const_iterator(std::set<Name>::const_iterator it,
                                                  const IdentityContainer& container)
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

IdentityContainer::const_iterator&
IdentityContainer::const_iterator::operator++()
{
  ++m_it;
  return *this;
}

IdentityContainer::const_iterator
IdentityContainer::const_iterator::operator++(int)
{
  const_iterator it(*this);
  ++m_it;
  return it;
}

bool
IdentityContainer::const_iterator::operator==(const const_iterator& other)
{
  bool isThisEnd = m_container == nullptr || m_it == m_container->m_identityNames.end();
  bool isOtherEnd = other.m_container == nullptr || other.m_it == other.m_container->m_identityNames.end();
  return ((isThisEnd || isOtherEnd) ?
          (isThisEnd == isOtherEnd) :
          m_container->m_pibImpl == other.m_container->m_pibImpl && m_it == other.m_it);
}

bool
IdentityContainer::const_iterator::operator!=(const const_iterator& other)
{
  return !(*this == other);
}

IdentityContainer::IdentityContainer(shared_ptr<PibImpl> pibImpl)
  : m_pibImpl(pibImpl)
{
  BOOST_ASSERT(pibImpl != nullptr);
  m_identityNames = pibImpl->getIdentities();
}

IdentityContainer::const_iterator
IdentityContainer::begin() const
{
  return const_iterator(m_identityNames.begin(), *this);
}

IdentityContainer::const_iterator
IdentityContainer::end() const
{
  return const_iterator();
}

IdentityContainer::const_iterator
IdentityContainer::find(const Name& identity) const
{
  return const_iterator(m_identityNames.find(identity), *this);
}

size_t
IdentityContainer::size() const
{
  return m_identityNames.size();
}

Identity
IdentityContainer::add(const Name& identityName)
{
  if (m_identityNames.count(identityName) == 0) {
    m_identityNames.insert(identityName);
    m_identities[identityName] =
      shared_ptr<detail::IdentityImpl>(new detail::IdentityImpl(identityName, m_pibImpl, true));
  }
  return get(identityName);
}

void
IdentityContainer::remove(const Name& identityName)
{
  m_identityNames.erase(identityName);
  m_identities.erase(identityName);
  m_pibImpl->removeIdentity(identityName);
}

Identity
IdentityContainer::get(const Name& identityName) const
{
  shared_ptr<detail::IdentityImpl> id;
  auto it = m_identities.find(identityName);

  if (it != m_identities.end()) {
    id = it->second;
  }
  else {
    id = shared_ptr<detail::IdentityImpl>(new detail::IdentityImpl(identityName, m_pibImpl, false));
    m_identities[identityName] = id;
  }
  return Identity(id);
}

void
IdentityContainer::reset()
{
  m_identities.clear();
  m_identityNames = m_pibImpl->getIdentities();
}

bool
IdentityContainer::isConsistent() const
{
  return m_identityNames == m_pibImpl->getIdentities();
}

} // namespace pib
} // namespace security
} // namespace ndn
