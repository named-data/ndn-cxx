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

#include "link.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Link>));
BOOST_CONCEPT_ASSERT((WireEncodable<Link>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Link>));
BOOST_CONCEPT_ASSERT((WireDecodable<Link>));
static_assert(std::is_base_of<Data::Error, Link::Error>::value,
              "Link::Error should inherit from Data::Error");

Link::Link() = default;

Link::Link(const Block& wire, bool wantSort)
{
  this->wireDecode(wire, wantSort);
}

Link::Link(const Name& name, std::initializer_list<Delegation> dels)
  : Data(name)
  , m_delList(dels)
{
  encodeContent();
}

void
Link::encodeContent()
{
  setContentType(tlv::ContentType_Link);

  if (m_delList.size() > 0) {
    EncodingEstimator estimator;
    size_t estimatedSize = m_delList.wireEncode(estimator, tlv::Content);

    EncodingBuffer buffer(estimatedSize, 0);
    m_delList.wireEncode(buffer, tlv::Content);

    setContent(buffer.block());
  }
  else {
    setContent(nullptr, 0);
  }
}

void
Link::wireDecode(const Block& wire, bool wantSort)
{
  Data::wireDecode(wire);

  if (getContentType() != tlv::ContentType_Link) {
    BOOST_THROW_EXCEPTION(Error("Expected ContentType Link"));
  }

  m_delList.wireDecode(getContent(), wantSort);
}

void
Link::setDelegationList(const DelegationList& dels)
{
  m_delList = dels;
  encodeContent();
}

void
Link::addDelegation(uint32_t preference, const Name& name)
{
  m_delList.insert(preference, name, DelegationList::INS_REPLACE);
  encodeContent();
}

bool
Link::removeDelegation(const Name& name)
{
  size_t nErased = m_delList.erase(name);
  if (nErased > 0) {
    encodeContent();
  }
  return nErased > 0;
}

} // namespace ndn
