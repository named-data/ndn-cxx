/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "secured-bag.hpp"
#include "encoding/tlv-security.hpp"
#include "util/concepts.hpp"

namespace ndn {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<SecuredBag>));
BOOST_CONCEPT_ASSERT((WireEncodable<SecuredBag>));
BOOST_CONCEPT_ASSERT((WireDecodable<SecuredBag>));
static_assert(std::is_base_of<tlv::Error, SecuredBag::Error>::value,
              "SecuredBag::Error must inherit from tlv::Error");

SecuredBag::SecuredBag()
  : m_wire(tlv::security::IdentityPackage)
{
}

SecuredBag::SecuredBag(const Block& wire)
{
  this->wireDecode(wire);
}

SecuredBag::SecuredBag(const IdentityCertificate& cert, ConstBufferPtr key)
  : m_cert(cert)
  , m_key(key)
  , m_wire(tlv::security::IdentityPackage)
{
  Block wireKey(tlv::security::KeyPackage, m_key);
  Block wireCert(tlv::security::CertificatePackage, cert.wireEncode());
  m_wire.push_back(wireCert);
  m_wire.push_back(wireKey);
}

SecuredBag::~SecuredBag()
{
}

void
SecuredBag::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  m_cert.wireDecode(m_wire.get(tlv::security::CertificatePackage).blockFromValue());

  Block wireKey = m_wire.get(tlv::security::KeyPackage);
  shared_ptr<Buffer> key = make_shared<Buffer>(wireKey.value(), wireKey.value_size());
  m_key = key;
}

const Block&
SecuredBag::wireEncode() const
{
  m_wire.encode();
  return m_wire;
}

} // namespace ndn
