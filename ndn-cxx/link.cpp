/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/link.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Link>));
BOOST_CONCEPT_ASSERT((WireEncodable<Link>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Link>));
BOOST_CONCEPT_ASSERT((WireDecodable<Link>));
static_assert(std::is_base_of<Data::Error, Link::Error>::value,
              "Link::Error should inherit from Data::Error");

Link::Link() = default;

Link::Link(const Block& wire)
{
  this->wireDecode(wire);
}

Link::Link(const Name& name, std::initializer_list<Name> delegations)
  : Data(name)
  , m_delegations(delegations)
{
  encodeContent();
}

void
Link::encodeContent()
{
  setContentType(tlv::ContentType_Link);

  if (m_delegations.empty()) {
    setContent(span<uint8_t>{});
  }
  else {
    setContent(makeNestedBlock(tlv::Content, m_delegations.begin(), m_delegations.end()));
  }
}

void
Link::wireDecode(const Block& wire)
{
  Data::wireDecode(wire);

  if (getContentType() != tlv::ContentType_Link) {
    NDN_THROW(Error("Expecting ContentType Link, got " + to_string(getContentType())));
  }

  // LinkContent = CONTENT-TYPE TLV-LENGTH 1*Name

  m_delegations.clear();
  auto content = getContent();
  content.parse();
  for (const auto& del : content.elements()) {
    if (del.type() == tlv::Name) {
      m_delegations.emplace_back(del);
    }
    else if (tlv::isCriticalType(del.type())) {
      NDN_THROW(Error("Unexpected TLV-TYPE " + to_string(del.type()) + " while decoding LinkContent"));
    }
  }
}

void
Link::setDelegationList(std::vector<Name> delegations)
{
  m_delegations = std::move(delegations);
  encodeContent();
}

bool
Link::addDelegation(const Name& name)
{
  if (std::find(m_delegations.begin(), m_delegations.end(), name) != m_delegations.end()) {
    return false;
  }

  m_delegations.push_back(name);
  encodeContent();
  return true;
}

bool
Link::removeDelegation(const Name& name)
{
  auto last = std::remove(m_delegations.begin(), m_delegations.end(), name);
  if (last == m_delegations.end()) {
    return false;
  }

  m_delegations.erase(last, m_delegations.end());
  encodeContent();
  return true;
}

} // namespace ndn
