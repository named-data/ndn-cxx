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

#include "interest.hpp"
#include "util/random.hpp"
#include "util/crypto.hpp"
#include "data.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Interest>));
BOOST_CONCEPT_ASSERT((WireDecodable<Interest>));
static_assert(std::is_base_of<tlv::Error, Interest::Error>::value,
              "Interest::Error must inherit from tlv::Error");

Interest::Interest()
  : m_interestLifetime(time::milliseconds::min())
  , m_selectedDelegationIndex(INVALID_SELECTED_DELEGATION_INDEX)
{
}

Interest::Interest(const Name& name)
  : m_name(name)
  , m_interestLifetime(time::milliseconds::min())
  , m_selectedDelegationIndex(INVALID_SELECTED_DELEGATION_INDEX)
{
}

Interest::Interest(const Name& name, const time::milliseconds& interestLifetime)
  : m_name(name)
  , m_interestLifetime(interestLifetime)
  , m_selectedDelegationIndex(INVALID_SELECTED_DELEGATION_INDEX)
{
}

Interest::Interest(const Block& wire)
{
  wireDecode(wire);
}

uint32_t
Interest::getNonce() const
{
  if (!m_nonce.hasWire())
    const_cast<Interest*>(this)->setNonce(random::generateWord32());

  if (m_nonce.value_size() == sizeof(uint32_t))
    return *reinterpret_cast<const uint32_t*>(m_nonce.value());
  else {
    // for compatibility reasons.  Should be removed eventually
    return readNonNegativeInteger(m_nonce);
  }
}

Interest&
Interest::setNonce(uint32_t nonce)
{
  if (m_wire.hasWire() && m_nonce.value_size() == sizeof(uint32_t)) {
    std::memcpy(const_cast<uint8_t*>(m_nonce.value()), &nonce, sizeof(nonce));
  }
  else {
    m_nonce = makeBinaryBlock(tlv::Nonce,
                              reinterpret_cast<const uint8_t*>(&nonce),
                              sizeof(nonce));
    m_wire.reset();
  }
  return *this;
}

void
Interest::refreshNonce()
{
  if (!hasNonce())
    return;

  uint32_t oldNonce = getNonce();
  uint32_t newNonce = oldNonce;
  while (newNonce == oldNonce)
    newNonce = random::generateWord32();

  setNonce(newNonce);
}

bool
Interest::matchesName(const Name& name) const
{
  if (name.size() < m_name.size())
    return false;

  if (!m_name.isPrefixOf(name))
    return false;

  if (getMinSuffixComponents() >= 0 &&
      // name must include implicit digest
      !(name.size() - m_name.size() >= static_cast<size_t>(getMinSuffixComponents())))
    return false;

  if (getMaxSuffixComponents() >= 0 &&
      // name must include implicit digest
      !(name.size() - m_name.size() <= static_cast<size_t>(getMaxSuffixComponents())))
    return false;

  if (!getExclude().empty() &&
      name.size() > m_name.size() &&
      getExclude().isExcluded(name[m_name.size()]))
    return false;

  return true;
}

bool
Interest::matchesData(const Data& data) const
{
  size_t interestNameLength = m_name.size();
  const Name& dataName = data.getName();
  size_t fullNameLength = dataName.size() + 1;

  // check MinSuffixComponents
  bool hasMinSuffixComponents = getMinSuffixComponents() >= 0;
  size_t minSuffixComponents = hasMinSuffixComponents ?
                               static_cast<size_t>(getMinSuffixComponents()) : 0;
  if (!(interestNameLength + minSuffixComponents <= fullNameLength))
    return false;

  // check MaxSuffixComponents
  bool hasMaxSuffixComponents = getMaxSuffixComponents() >= 0;
  if (hasMaxSuffixComponents &&
      !(interestNameLength + getMaxSuffixComponents() >= fullNameLength))
    return false;

  // check prefix
  if (interestNameLength == fullNameLength) {
    if (m_name.get(-1).isImplicitSha256Digest()) {
      if (m_name != data.getFullName())
        return false;
    }
    else {
      // Interest Name is same length as Data full Name, but last component isn't digest
      // so there's no possibility of matching
      return false;
    }
  }
  else {
    // Interest Name is a strict prefix of Data full Name
    if (!m_name.isPrefixOf(dataName))
      return false;
  }

  // check Exclude
  // Exclude won't be violated if Interest Name is same as Data full Name
  if (!getExclude().empty() && fullNameLength > interestNameLength) {
    if (interestNameLength == fullNameLength - 1) {
      // component to exclude is the digest
      if (getExclude().isExcluded(data.getFullName().get(interestNameLength)))
        return false;
      // There's opportunity to inspect the Exclude filter and determine whether
      // the digest would make a difference.
      // eg. "<NameComponent>AA</NameComponent><Any/>" doesn't exclude any digest -
      //     fullName not needed;
      //     "<Any/><NameComponent>AA</NameComponent>" and
      //     "<Any/><ImplicitSha256DigestComponent>ffffffffffffffffffffffffffffffff
      //      </ImplicitSha256DigestComponent>"
      //     excludes all digests - fullName not needed;
      //     "<Any/><ImplicitSha256DigestComponent>80000000000000000000000000000000
      //      </ImplicitSha256DigestComponent>"
      //     excludes some digests - fullName required
      // But Interests that contain the exact Data Name before digest and also
      // contain Exclude filter is too rare to optimize for, so we request
      // fullName no mater what's in the Exclude filter.
    }
    else {
      // component to exclude is not the digest
      if (getExclude().isExcluded(dataName.get(interestNameLength)))
        return false;
    }
  }

  // check PublisherPublicKeyLocator
  const KeyLocator& publisherPublicKeyLocator = this->getPublisherPublicKeyLocator();
  if (!publisherPublicKeyLocator.empty()) {
    const Signature& signature = data.getSignature();
    const Block& signatureInfo = signature.getInfo();
    Block::element_const_iterator it = signatureInfo.find(tlv::KeyLocator);
    if (it == signatureInfo.elements_end()) {
      return false;
    }
    if (publisherPublicKeyLocator.wireEncode() != *it) {
      return false;
    }
  }

  return true;
}

template<encoding::Tag TAG>
size_t
Interest::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                InterestLifetime?
  //                Link?
  //                SelectedDelegation?

  // (reverse encoding)

  if (hasLink()) {
    if (hasSelectedDelegation()) {
      totalLength += prependNonNegativeIntegerBlock(encoder,
                                                    tlv::SelectedDelegation,
                                                    m_selectedDelegationIndex);
    }
    totalLength += encoder.prependBlock(m_link);
  }
  else {
    BOOST_ASSERT(!hasSelectedDelegation());
  }

  // InterestLifetime
  if (getInterestLifetime() >= time::milliseconds::zero() &&
      getInterestLifetime() != DEFAULT_INTEREST_LIFETIME)
    {
      totalLength += prependNonNegativeIntegerBlock(encoder,
                                                    tlv::InterestLifetime,
                                                    getInterestLifetime().count());
    }

  // Nonce
  getNonce(); // to ensure that Nonce is properly set
  totalLength += encoder.prependBlock(m_nonce);

  // Selectors
  if (hasSelectors())
    {
      totalLength += getSelectors().wireEncode(encoder);
    }

  // Name
  totalLength += getName().wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Interest);
  return totalLength;
}

template size_t
Interest::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Interest::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
Interest::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  // to ensure that Nonce block points to the right memory location
  const_cast<Interest*>(this)->wireDecode(buffer.block());

  return m_wire;
}

void
Interest::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                InterestLifetime?
  //                Link?
  //                SelectedDelegation?

  if (m_wire.type() != tlv::Interest)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV number when decoding Interest"));

  // Name
  m_name.wireDecode(m_wire.get(tlv::Name));

  // Selectors
  Block::element_const_iterator val = m_wire.find(tlv::Selectors);
  if (val != m_wire.elements_end())
    {
      m_selectors.wireDecode(*val);
    }
  else
    m_selectors = Selectors();

  // Nonce
  m_nonce = m_wire.get(tlv::Nonce);

  // InterestLifetime
  val = m_wire.find(tlv::InterestLifetime);
  if (val != m_wire.elements_end())
    {
      m_interestLifetime = time::milliseconds(readNonNegativeInteger(*val));
    }
  else
    {
      m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
    }

  // Link object
  val = m_wire.find(tlv::Data);
  if (val != m_wire.elements_end())
    {
      m_link = (*val);
    }

  // SelectedDelegation
  val = m_wire.find(tlv::SelectedDelegation);
  if (val != m_wire.elements_end()) {
    if (!this->hasLink()) {
      BOOST_THROW_EXCEPTION(Error("Interest contains selectedDelegation, but no LINK object"));
    }
    uint64_t selectedDelegation = readNonNegativeInteger(*val);
    if (selectedDelegation < uint64_t(Link::countDelegationsFromWire(m_link))) {
      m_selectedDelegationIndex = static_cast<size_t>(selectedDelegation);
    }
    else {
      BOOST_THROW_EXCEPTION(Error("Invalid selected delegation index when decoding Interest"));
    }
  }
}

bool
Interest::hasLink() const
{
  if (m_link.hasWire())
    return true;
  return false;
}

Link
Interest::getLink() const
{
  if (hasLink())
    {
      return Link(m_link);
    }
  BOOST_THROW_EXCEPTION(Error("There is no encapsulated link object"));
}

void
Interest::setLink(const Block& link)
{
  m_link = link;
  if (!link.hasWire()) {
    BOOST_THROW_EXCEPTION(Error("The given link does not have a wire format"));
  }
  m_wire.reset();
  this->unsetSelectedDelegation();
}

void
Interest::unsetLink()
{
  m_link.reset();
  m_wire.reset();
  this->unsetSelectedDelegation();
}

bool
Interest::hasSelectedDelegation() const
{
  if (m_selectedDelegationIndex != INVALID_SELECTED_DELEGATION_INDEX)
    {
      return true;
    }
  return false;
}

Name
Interest::getSelectedDelegation() const
{
  if (!hasSelectedDelegation()) {
    BOOST_THROW_EXCEPTION(Error("There is no encapsulated selected delegation"));
  }
  return std::get<1>(Link::getDelegationFromWire(m_link, m_selectedDelegationIndex));
}

void
Interest::setSelectedDelegation(const Name& delegationName)
{
  size_t delegationIndex = Link::findDelegationFromWire(m_link, delegationName);
  if (delegationIndex != INVALID_SELECTED_DELEGATION_INDEX) {
    m_selectedDelegationIndex = delegationIndex;
  }
  else {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Invalid selected delegation name"));
  }
  m_wire.reset();
}

void
Interest::setSelectedDelegation(size_t delegationIndex)
{
  if (delegationIndex >= Link(m_link).getDelegations().size()) {
    BOOST_THROW_EXCEPTION(Error("Invalid selected delegation index"));
  }
  m_selectedDelegationIndex = delegationIndex;
  m_wire.reset();
}

void
Interest::unsetSelectedDelegation()
{
  m_selectedDelegationIndex = INVALID_SELECTED_DELEGATION_INDEX;
  m_wire.reset();
}

std::ostream&
operator<<(std::ostream& os, const Interest& interest)
{
  os << interest.getName();

  char delim = '?';

  if (interest.getMinSuffixComponents() >= 0) {
    os << delim << "ndn.MinSuffixComponents=" << interest.getMinSuffixComponents();
    delim = '&';
  }
  if (interest.getMaxSuffixComponents() >= 0) {
    os << delim << "ndn.MaxSuffixComponents=" << interest.getMaxSuffixComponents();
    delim = '&';
  }
  if (interest.getChildSelector() >= 0) {
    os << delim << "ndn.ChildSelector=" << interest.getChildSelector();
    delim = '&';
  }
  if (interest.getMustBeFresh()) {
    os << delim << "ndn.MustBeFresh=" << interest.getMustBeFresh();
    delim = '&';
  }
  if (interest.getInterestLifetime() >= time::milliseconds::zero()
      && interest.getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    os << delim << "ndn.InterestLifetime=" << interest.getInterestLifetime().count();
    delim = '&';
  }

  if (interest.hasNonce()) {
    os << delim << "ndn.Nonce=" << interest.getNonce();
    delim = '&';
  }
  if (!interest.getExclude().empty()) {
    os << delim << "ndn.Exclude=" << interest.getExclude();
    delim = '&';
  }

  return os;
}

} // namespace ndn
