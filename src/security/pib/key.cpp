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

#include "key.hpp"
#include "detail/key-impl.hpp"
#include "../v2/certificate.hpp"

namespace ndn {
namespace security {
namespace pib {

Key::Key() = default;

Key::Key(weak_ptr<detail::KeyImpl> impl)
  : m_impl(impl)
{
}

const Name&
Key::getName() const
{
  return lock()->getName();
}

const Name&
Key::getIdentity() const
{
  return lock()->getIdentity();
}

KeyType
Key::getKeyType() const
{
  return lock()->getKeyType();
}

const Buffer&
Key::getPublicKey() const
{
  return lock()->getPublicKey();
}

void
Key::addCertificate(const v2::Certificate& certificate) const
{
  return lock()->addCertificate(certificate);
}

void
Key::removeCertificate(const Name& certName) const
{
  return lock()->removeCertificate(certName);
}

v2::Certificate
Key::getCertificate(const Name& certName) const
{
  return lock()->getCertificate(certName);
}

const CertificateContainer&
Key::getCertificates() const
{
  return lock()->getCertificates();
}

const v2::Certificate&
Key::setDefaultCertificate(const Name& certName) const
{
  return lock()->setDefaultCertificate(certName);
}

const v2::Certificate&
Key::setDefaultCertificate(const v2::Certificate& certificate) const
{
  return lock()->setDefaultCertificate(certificate);
}

const v2::Certificate&
Key::getDefaultCertificate() const
{
  return lock()->getDefaultCertificate();
}

Key::operator bool() const
{
  return !m_impl.expired();
}

shared_ptr<detail::KeyImpl>
Key::lock() const
{
  auto impl = m_impl.lock();

  if (impl == nullptr) {
    BOOST_THROW_EXCEPTION(std::domain_error("Invalid key instance"));
  }

  return impl;
}

bool
operator!=(const Key& lhs, const Key& rhs)
{
  return lhs.m_impl.owner_before(rhs.m_impl) || rhs.m_impl.owner_before(lhs.m_impl);
}

std::ostream&
operator<<(std::ostream& os, const Key& key)
{
  if (key) {
    os << key.getName();
  }
  else {
    os << "(empty)";
  }
  return os;
}

} // namespace pib

namespace v2 {

Name
constructKeyName(const Name& identity, const name::Component& keyId)
{
  Name keyName = identity;
  keyName
    .append(Certificate::KEY_COMPONENT)
    .append(keyId);
  return keyName;
}

bool
isValidKeyName(const Name& keyName)
{
  return (keyName.size() >= Certificate::MIN_KEY_NAME_LENGTH &&
          keyName.get(-Certificate::MIN_KEY_NAME_LENGTH) == Certificate::KEY_COMPONENT);
}

Name
extractIdentityFromKeyName(const Name& keyName)
{
  if (!isValidKeyName(keyName)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Key name `" + keyName.toUri() + "` "
                                                "does not follow the naming conventions"));
  }

  return keyName.getPrefix(-Certificate::MIN_KEY_NAME_LENGTH); // trim everything after and including "KEY"
}

} // namespace v2

} // namespace security
} // namespace ndn
