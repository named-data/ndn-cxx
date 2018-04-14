/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
#include "data.hpp"

#include <cstring>
#include <sstream>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Interest>));
BOOST_CONCEPT_ASSERT((WireDecodable<Interest>));
static_assert(std::is_base_of<tlv::Error, Interest::Error>::value,
              "Interest::Error must inherit from tlv::Error");

Interest::Interest(const Name& name, time::milliseconds lifetime)
  : m_name(name)
  , m_interestLifetime(lifetime)
{
  if (lifetime < time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("InterestLifetime must be >= 0"));
  }
}

Interest::Interest(const Block& wire)
{
  wireDecode(wire);
}

// ---- encode and decode ----

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
  //                ForwardingHint?

  // (reverse encoding)

  // ForwardingHint
  if (m_forwardingHint.size() > 0) {
    totalLength += m_forwardingHint.wireEncode(encoder);
  }

  // InterestLifetime
  if (getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                                                  tlv::InterestLifetime,
                                                  getInterestLifetime().count());
  }

  // Nonce
  uint32_t nonce = this->getNonce(); // assigns random Nonce if needed
  totalLength += encoder.prependByteArray(reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));
  totalLength += encoder.prependVarNumber(sizeof(nonce));
  totalLength += encoder.prependVarNumber(tlv::Nonce);

  // Selectors
  if (hasSelectors()) {
    totalLength += getSelectors().wireEncode(encoder);
  }

  // Name
  totalLength += getName().wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Interest);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(Interest);

const Block&
Interest::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Interest*>(this)->wireDecode(buffer.block());
  return m_wire;
}

void
Interest::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::Interest) {
    BOOST_THROW_EXCEPTION(Error("expecting Interest element, got " + to_string(m_wire.type())));
  }

  if (!decode02()) {
    decode03();
    if (!hasNonce()) {
      setNonce(getNonce());
    }
  }
}

bool
Interest::decode02()
{
  auto ele = m_wire.elements_begin();

  // Name
  if (ele != m_wire.elements_end() && ele->type() == tlv::Name) {
    m_name.wireDecode(*ele);
    ++ele;
  }
  else {
    return false;
  }

  // Selectors?
  if (ele != m_wire.elements_end() && ele->type() == tlv::Selectors) {
    m_selectors.wireDecode(*ele);
    ++ele;
  }
  else {
    m_selectors = Selectors();
  }

  // Nonce
  if (ele != m_wire.elements_end() && ele->type() == tlv::Nonce) {
    uint32_t nonce = 0;
    if (ele->value_size() != sizeof(nonce)) {
      BOOST_THROW_EXCEPTION(Error("Nonce element is malformed"));
    }
    std::memcpy(&nonce, ele->value(), sizeof(nonce));
    m_nonce = nonce;
    ++ele;
  }
  else {
    return false;
  }

  // InterestLifetime?
  if (ele != m_wire.elements_end() && ele->type() == tlv::InterestLifetime) {
    m_interestLifetime = time::milliseconds(readNonNegativeInteger(*ele));
    ++ele;
  }
  else {
    m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  }

  // ForwardingHint?
  if (ele != m_wire.elements_end() && ele->type() == tlv::ForwardingHint) {
    m_forwardingHint.wireDecode(*ele, false);
    ++ele;
  }
  else {
    m_forwardingHint = DelegationList();
  }

  return ele == m_wire.elements_end();
}

void
Interest::decode03()
{
  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                CanBePrefix?
  //                MustBeFresh?
  //                ForwardingHint?
  //                Nonce?
  //                InterestLifetime?
  //                HopLimit?
  //                Parameters?

  bool hasName = false;
  m_selectors = Selectors().setMaxSuffixComponents(1); // CanBePrefix=0
  m_nonce.reset();
  m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  m_forwardingHint = DelegationList();

  int lastEle = 0; // last recognized element index, in spec order
  for (const Block& ele : m_wire.elements()) {
    switch (ele.type()) {
      case tlv::Name: {
        if (lastEle >= 1) {
          BOOST_THROW_EXCEPTION(Error("Name element is out of order"));
        }
        hasName = true;
        m_name.wireDecode(ele);
        if (m_name.empty()) {
          BOOST_THROW_EXCEPTION(Error("Name has zero name components"));
        }
        lastEle = 1;
        break;
      }
      case tlv::CanBePrefix: {
        if (lastEle >= 2) {
          BOOST_THROW_EXCEPTION(Error("CanBePrefix element is out of order"));
        }
        if (ele.value_size() != 0) {
          BOOST_THROW_EXCEPTION(Error("CanBePrefix element has non-zero TLV-LENGTH"));
        }
        m_selectors.setMaxSuffixComponents(-1);
        lastEle = 2;
        break;
      }
      case tlv::MustBeFresh: {
        if (lastEle >= 3) {
          BOOST_THROW_EXCEPTION(Error("MustBeFresh element is out of order"));
        }
        if (ele.value_size() != 0) {
          BOOST_THROW_EXCEPTION(Error("MustBeFresh element has non-zero TLV-LENGTH"));
        }
        m_selectors.setMustBeFresh(true);
        lastEle = 3;
        break;
      }
      case tlv::ForwardingHint: {
        if (lastEle >= 4) {
          BOOST_THROW_EXCEPTION(Error("ForwardingHint element is out of order"));
        }
        m_forwardingHint.wireDecode(ele);
        lastEle = 4;
        break;
      }
      case tlv::Nonce: {
        if (lastEle >= 5) {
          BOOST_THROW_EXCEPTION(Error("Nonce element is out of order"));
        }
        uint32_t nonce = 0;
        if (ele.value_size() != sizeof(nonce)) {
          BOOST_THROW_EXCEPTION(Error("Nonce element is malformed"));
        }
        std::memcpy(&nonce, ele.value(), sizeof(nonce));
        m_nonce = nonce;
        lastEle = 5;
        break;
      }
      case tlv::InterestLifetime: {
        if (lastEle >= 6) {
          BOOST_THROW_EXCEPTION(Error("InterestLifetime element is out of order"));
        }
        m_interestLifetime = time::milliseconds(readNonNegativeInteger(ele));
        lastEle = 6;
        break;
      }
      case tlv::HopLimit: {
        if (lastEle >= 7) {
          break; // HopLimit is non-critical, ignore out-of-order appearance
        }
        if (ele.value_size() != 1) {
          BOOST_THROW_EXCEPTION(Error("HopLimit element is malformed"));
        }
        // TLV-VALUE is ignored
        lastEle = 7;
        break;
      }
      case tlv::Parameters: {
        if (lastEle >= 8) {
          BOOST_THROW_EXCEPTION(Error("Parameters element is out of order"));
        }
        // TLV-VALUE is ignored
        lastEle = 8;
        break;
      }
      default: {
        if (tlv::isCriticalType(ele.type())) {
          BOOST_THROW_EXCEPTION(Error("unrecognized element of critical type " +
                                      to_string(ele.type())));
        }
        break;
      }
    }
  }

  if (!hasName) {
    BOOST_THROW_EXCEPTION(Error("Name element is missing"));
  }
}

std::string
Interest::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

// ---- matching ----

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
      // eg. "<GenericNameComponent>AA</GenericNameComponent><Any/>" doesn't exclude
      //     any digest - fullName not needed;
      //     "<Any/><GenericNameComponent>AA</GenericNameComponent>" and
      //     "<Any/><ImplicitSha256DigestComponent>ffffffffffffffffffffffffffffffff
      //      </ImplicitSha256DigestComponent>"
      //     excludes all digests - fullName not needed;
      //     "<Any/><ImplicitSha256DigestComponent>80000000000000000000000000000000
      //      </ImplicitSha256DigestComponent>"
      //     excludes some digests - fullName required
      // But Interests that contain the exact Data Name before digest and also
      // contain Exclude filter is too rare to optimize for, so we request
      // fullName no matter what's in the Exclude filter.
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

bool
Interest::matchesInterest(const Interest& other) const
{
  /// @todo #3162 match ForwardingHint field
  return (this->getName() == other.getName() &&
          this->getSelectors() == other.getSelectors());
}

// ---- field accessors ----

uint32_t
Interest::getNonce() const
{
  if (!m_nonce) {
    m_nonce = random::generateWord32();
  }
  return *m_nonce;
}

Interest&
Interest::setNonce(uint32_t nonce)
{
  m_nonce = nonce;
  m_wire.reset();
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

Interest&
Interest::setInterestLifetime(time::milliseconds lifetime)
{
  if (lifetime < time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("InterestLifetime must be >= 0"));
  }
  m_interestLifetime = lifetime;
  m_wire.reset();
  return *this;
}

Interest&
Interest::setForwardingHint(const DelegationList& value)
{
  m_forwardingHint = value;
  m_wire.reset();
  return *this;
}

// ---- operators ----

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
  if (interest.getChildSelector() != DEFAULT_CHILD_SELECTOR) {
    os << delim << "ndn.ChildSelector=" << interest.getChildSelector();
    delim = '&';
  }
  if (interest.getMustBeFresh()) {
    os << delim << "ndn.MustBeFresh=" << interest.getMustBeFresh();
    delim = '&';
  }
  if (interest.getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
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
