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

#include "certificate-container.hpp"
#include "pib-impl.hpp"

namespace ndn {
namespace security {

CertificateContainer::const_iterator::const_iterator(std::set<Name>::const_iterator it,
                                                     shared_ptr<PibImpl> impl)
  : m_it(it)
  , m_impl(impl)
{
}

IdentityCertificate
CertificateContainer::const_iterator::operator*()
{
  return m_impl->getCertificate(*m_it);
}

CertificateContainer::const_iterator&
CertificateContainer::const_iterator::operator++()
{
  ++m_it;
  return *this;
}

CertificateContainer::const_iterator
CertificateContainer::const_iterator::operator++(int)
{
  const_iterator it(m_it, m_impl);
  ++m_it;
  return it;
}

bool
CertificateContainer::const_iterator::operator==(const const_iterator& other)
{
  return (m_impl == other.m_impl && m_it == other.m_it);
}

bool
CertificateContainer::const_iterator::operator!=(const const_iterator& other)
{
  return !(*this == other);
}

CertificateContainer::CertificateContainer()
{
}

CertificateContainer::CertificateContainer(std::set<Name>&& certNames,
                                           shared_ptr<PibImpl> impl)
  : m_certNames(certNames)
  , m_impl(impl)
{
}

CertificateContainer::const_iterator
CertificateContainer::begin() const
{
  return const_iterator(m_certNames.begin(), m_impl);
}

CertificateContainer::const_iterator
CertificateContainer::end() const
{
  return const_iterator(m_certNames.end(), m_impl);
}

CertificateContainer::const_iterator
CertificateContainer::find(const Name& certName) const
{
  return const_iterator(m_certNames.find(certName), m_impl);
}

size_t
CertificateContainer::size() const
{
  return m_certNames.size();
}

} // namespace security
} // namespace ndn
