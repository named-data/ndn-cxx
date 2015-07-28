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

#include "link.hpp"
#include "interest.hpp"
#include "encoding/block-helpers.hpp"
#include "util/crypto.hpp"
#include "security/key-chain.hpp"

#include <algorithm>

#include <boost/range/adaptors.hpp>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Link>));
BOOST_CONCEPT_ASSERT((WireEncodable<Link>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Link>));
BOOST_CONCEPT_ASSERT((WireDecodable<Link>));
static_assert(std::is_base_of<Data::Error, Link::Error>::value,
              "Link::Error should inherit from Data::Error");

Link::Link(const Block& block)
{
  wireDecode(block);
}

Link::Link(const Name& name)
  : Data(name)
{
}

Link::Link(const Name& name, std::initializer_list<std::pair<uint32_t, Name>> links)
  : Data(name)
{
  m_delegations.insert(links);
  encodeContent();
}

void
Link::addDelegation(uint32_t preference, const Name& name)
{
  this->removeDelegationNoEncode(name);
  m_delegations.insert({preference, name});
  encodeContent();
}

bool
Link::removeDelegation(const Name& name)
{
  bool hasRemovedDelegation = this->removeDelegationNoEncode(name);
  if (hasRemovedDelegation) {
    encodeContent();
  }
  return hasRemovedDelegation;
}

const Link::DelegationSet&
Link::getDelegations() const
{
  return m_delegations;
}

template<encoding::Tag TAG>
size_t
Link::encodeContent(EncodingImpl<TAG>& encoder) const
{
  // LinkContent ::= CONTENT-TYPE TLV-LENGTH
  //                    Delegation+

  // Delegation ::= LINK-DELEGATION-TYPE TLV-LENGTH
  //              Preference
  //              Name

  // Preference ::= LINK-PREFERENCE-TYPE TLV-LENGTH
  //       nonNegativeInteger

  size_t totalLength = 0;
  for (const auto& delegation : m_delegations |  boost::adaptors::reversed) {
    size_t delegationLength = 0;
    delegationLength += std::get<1>(delegation).wireEncode(encoder);
    delegationLength += prependNonNegativeIntegerBlock(encoder, tlv::LinkPreference,
                                                       std::get<0>(delegation));
    delegationLength += encoder.prependVarNumber(delegationLength);
    delegationLength += encoder.prependVarNumber(tlv::LinkDelegation);
    totalLength += delegationLength;
  }
  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Content);
  return totalLength;
}

template size_t
Link::encodeContent<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Link::encodeContent<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

void
Link::encodeContent()
{
  onChanged();

  EncodingEstimator estimator;
  size_t estimatedSize = encodeContent(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  encodeContent(buffer);

  setContentType(tlv::ContentType_Link);
  setContent(buffer.block());
}

void
Link::decodeContent()
{
  // LinkContent ::= CONTENT-TYPE TLV-LENGTH
  //                    Delegation+

  // Delegation ::= LINK-DELEGATION-TYPE TLV-LENGTH
  //              Preference
  //              Name

  // Preference ::= LINK-PREFERENCE-TYPE TLV-LENGTH
  //       nonNegativeInteger

  if (getContentType() != tlv::ContentType_Link)
    {
      BOOST_THROW_EXCEPTION(Error("Expected Content Type Link"));
    }

  const Block& content = getContent();
  content.parse();

  for (auto& delegation : content.elements()) {
    delegation.parse();
    Block::element_const_iterator val = delegation.elements_begin();
    if (val == delegation.elements_end()) {
      BOOST_THROW_EXCEPTION(Error("Unexpected Link Encoding"));
    }
    uint32_t preference;
    try {
      preference = static_cast<uint32_t>(readNonNegativeInteger(*val));
    }
    catch (tlv::Error&) {
      BOOST_THROW_EXCEPTION(Error("Missing preference field in Link Encoding"));
    }
    ++val;
    if (val == delegation.elements_end()) {
      BOOST_THROW_EXCEPTION(Error("Missing name field in Link Encoding"));
    }
    Name name(*val);
    m_delegations.insert({preference, name});
  }
}

void
Link::wireDecode(const Block& wire)
{
  Data::wireDecode(wire);
  decodeContent();
}

std::tuple<uint32_t, Name>
Link::getDelegationFromWire(const Block& block, size_t index)
{
  block.parse();
  const Block& contentBlock = block.get(tlv::Content);
  contentBlock.parse();
  const Block& delegationBlock = contentBlock.elements().at(index);
  delegationBlock.parse();
  if (delegationBlock.type() != tlv::LinkDelegation) {
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV-TYPE, expecting LinkDelegation"));
  }
  return std::make_tuple(
    static_cast<uint32_t>(
      readNonNegativeInteger(delegationBlock.get(tlv::LinkPreference))),
    Name(delegationBlock.get(tlv::Name)));
}

ssize_t
Link::findDelegationFromWire(const Block& block, const Name& delegationName)
{
  block.parse();
  const Block& contentBlock = block.get(tlv::Content);
  contentBlock.parse();
  size_t counter = 0;
  for (auto&& delegationBlock : contentBlock.elements()) {
    delegationBlock.parse();
    if (delegationBlock.type() != tlv::LinkDelegation) {
      BOOST_THROW_EXCEPTION(Error("Unexpected TLV-TYPE, expecting LinkDelegation"));
    }
    Name name(delegationBlock.get(tlv::Name));
    if (name == delegationName) {
      return counter;
    }
    ++counter;
  }
  return INVALID_SELECTED_DELEGATION_INDEX;
}

ssize_t
Link::countDelegationsFromWire(const Block& block)
{
  block.parse();
  const Block& contentBlock = block.get(tlv::Content);
  contentBlock.parse();
  return contentBlock.elements_size();
}

bool
Link::removeDelegationNoEncode(const Name& name)
{
  bool hasRemoved = false;
  auto i = m_delegations.begin();
  while (i != m_delegations.end()) {
    if (i->second == name) {
      hasRemoved = true;
      i = m_delegations.erase(i);
    }
    else {
      ++i;
    }
  }
  return hasRemoved;
}

} // namespace ndn
