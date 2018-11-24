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

#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/data.hpp"
#include "ndn-cxx/util/random.hpp"

#include <boost/scope_exit.hpp>

#include <cstring>
#include <iostream>
#include <sstream>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Interest>));
BOOST_CONCEPT_ASSERT((WireDecodable<Interest>));
static_assert(std::is_base_of<tlv::Error, Interest::Error>::value,
              "Interest::Error must inherit from tlv::Error");

#ifdef NDN_CXX_HAVE_TESTS
bool Interest::s_errorIfCanBePrefixUnset = true;
#endif // NDN_CXX_HAVE_TESTS
boost::logic::tribool Interest::s_defaultCanBePrefix = boost::logic::indeterminate;

Interest::Interest(const Name& name, time::milliseconds lifetime)
  : m_name(name)
  , m_isCanBePrefixSet(false)
  , m_interestLifetime(lifetime)
{
  if (lifetime < time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("InterestLifetime must be >= 0"));
  }

  if (!boost::logic::indeterminate(s_defaultCanBePrefix)) {
    setCanBePrefix(static_cast<bool>(s_defaultCanBePrefix));
  }
}

Interest::Interest(const Block& wire)
  : m_isCanBePrefixSet(true)
{
  wireDecode(wire);
}

// ---- encode and decode ----

template<encoding::Tag TAG>
size_t
Interest::wireEncode(EncodingImpl<TAG>& encoder) const
{
  static bool hasDefaultCanBePrefixWarning = false;
  if (!m_isCanBePrefixSet) {
    if (!hasDefaultCanBePrefixWarning) {
      std::cerr << "WARNING: Interest.CanBePrefix will be set to 0 in the near future. "
                << "Please declare a preferred setting via Interest::setDefaultCanBePrefix.";
      hasDefaultCanBePrefixWarning = true;
    }
#ifdef NDN_CXX_HAVE_TESTS
    if (s_errorIfCanBePrefixUnset) {
      BOOST_THROW_EXCEPTION(std::logic_error("Interest.CanBePrefix is unset"));
    }
#endif // NDN_CXX_HAVE_TESTS
  }

  if (hasParameters()) {
    return encode03(encoder);
  }
  else {
    return encode02(encoder);
  }
}

template<encoding::Tag TAG>
size_t
Interest::encode02(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Encode as NDN Packet Format v0.2
  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                InterestLifetime?
  //                ForwardingHint?

  // (reverse encoding)

  // ForwardingHint
  if (getForwardingHint().size() > 0) {
    totalLength += getForwardingHint().wireEncode(encoder);
  }

  // InterestLifetime
  if (getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                                                  tlv::InterestLifetime,
                                                  getInterestLifetime().count());
  }

  // Nonce
  uint32_t nonce = getNonce(); // if nonce was unset, getNonce generates a random nonce
  totalLength += encoder.prependByteArrayBlock(tlv::Nonce, reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));

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

template<encoding::Tag TAG>
size_t
Interest::encode03(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Encode as NDN Packet Format v0.3
  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                CanBePrefix?
  //                MustBeFresh?
  //                ForwardingHint?
  //                Nonce?
  //                InterestLifetime?
  //                HopLimit?
  //                Parameters?

  // (reverse encoding)

  // Parameters
  if (hasParameters()) {
    totalLength += encoder.prependBlock(getParameters());
  }

  // HopLimit: not yet supported

  // InterestLifetime
  if (getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                                                  tlv::InterestLifetime,
                                                  getInterestLifetime().count());
  }

  // Nonce
  uint32_t nonce = getNonce(); // if nonce was unset, getNonce generates a random nonce
  totalLength += encoder.prependByteArrayBlock(tlv::Nonce, reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));

  // ForwardingHint
  if (getForwardingHint().size() > 0) {
    totalLength += getForwardingHint().wireEncode(encoder);
  }

  // MustBeFresh
  if (getMustBeFresh()) {
    totalLength += prependEmptyBlock(encoder, tlv::MustBeFresh);
  }

  // CanBePrefix
  if (getCanBePrefix()) {
    totalLength += prependEmptyBlock(encoder, tlv::CanBePrefix);
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

  m_isCanBePrefixSet = true; // don't trigger warning from decoded packet
}

bool
Interest::decode02()
{
  auto element = m_wire.elements_begin();

  // Name
  if (element != m_wire.elements_end() && element->type() == tlv::Name) {
    m_name.wireDecode(*element);
    ++element;
  }
  else {
    return false;
  }

  // Selectors?
  if (element != m_wire.elements_end() && element->type() == tlv::Selectors) {
    m_selectors.wireDecode(*element);
    ++element;
  }
  else {
    m_selectors = Selectors();
  }

  // Nonce
  if (element != m_wire.elements_end() && element->type() == tlv::Nonce) {
    uint32_t nonce = 0;
    if (element->value_size() != sizeof(nonce)) {
      BOOST_THROW_EXCEPTION(Error("Nonce element is malformed"));
    }
    std::memcpy(&nonce, element->value(), sizeof(nonce));
    m_nonce = nonce;
    ++element;
  }
  else {
    return false;
  }

  // InterestLifetime?
  if (element != m_wire.elements_end() && element->type() == tlv::InterestLifetime) {
    m_interestLifetime = time::milliseconds(readNonNegativeInteger(*element));
    ++element;
  }
  else {
    m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  }

  // ForwardingHint?
  if (element != m_wire.elements_end() && element->type() == tlv::ForwardingHint) {
    m_forwardingHint.wireDecode(*element, false);
    ++element;
  }
  else {
    m_forwardingHint = DelegationList();
  }

  return element == m_wire.elements_end();
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

  auto element = m_wire.elements_begin();
  if (element == m_wire.elements_end() || element->type() != tlv::Name) {
    BOOST_THROW_EXCEPTION(Error("Name element is missing or out of order"));
  }
  m_name.wireDecode(*element);
  if (m_name.empty()) {
    BOOST_THROW_EXCEPTION(Error("Name has zero name components"));
  }
  int lastElement = 1; // last recognized element index, in spec order

  m_selectors = Selectors().setMaxSuffixComponents(1); // CanBePrefix=0
  m_nonce.reset();
  m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  m_forwardingHint = DelegationList();
  m_parameters = Block();

  for (++element; element != m_wire.elements_end(); ++element) {
    switch (element->type()) {
      case tlv::CanBePrefix: {
        if (lastElement >= 2) {
          BOOST_THROW_EXCEPTION(Error("CanBePrefix element is out of order"));
        }
        if (element->value_size() != 0) {
          BOOST_THROW_EXCEPTION(Error("CanBePrefix element has non-zero TLV-LENGTH"));
        }
        m_selectors.setMaxSuffixComponents(-1);
        lastElement = 2;
        break;
      }
      case tlv::MustBeFresh: {
        if (lastElement >= 3) {
          BOOST_THROW_EXCEPTION(Error("MustBeFresh element is out of order"));
        }
        if (element->value_size() != 0) {
          BOOST_THROW_EXCEPTION(Error("MustBeFresh element has non-zero TLV-LENGTH"));
        }
        m_selectors.setMustBeFresh(true);
        lastElement = 3;
        break;
      }
      case tlv::ForwardingHint: {
        if (lastElement >= 4) {
          BOOST_THROW_EXCEPTION(Error("ForwardingHint element is out of order"));
        }
        m_forwardingHint.wireDecode(*element);
        lastElement = 4;
        break;
      }
      case tlv::Nonce: {
        if (lastElement >= 5) {
          BOOST_THROW_EXCEPTION(Error("Nonce element is out of order"));
        }
        uint32_t nonce = 0;
        if (element->value_size() != sizeof(nonce)) {
          BOOST_THROW_EXCEPTION(Error("Nonce element is malformed"));
        }
        std::memcpy(&nonce, element->value(), sizeof(nonce));
        m_nonce = nonce;
        lastElement = 5;
        break;
      }
      case tlv::InterestLifetime: {
        if (lastElement >= 6) {
          BOOST_THROW_EXCEPTION(Error("InterestLifetime element is out of order"));
        }
        m_interestLifetime = time::milliseconds(readNonNegativeInteger(*element));
        lastElement = 6;
        break;
      }
      case tlv::HopLimit: {
        if (lastElement >= 7) {
          break; // HopLimit is non-critical, ignore out-of-order appearance
        }
        if (element->value_size() != 1) {
          BOOST_THROW_EXCEPTION(Error("HopLimit element is malformed"));
        }
        // TLV-VALUE is ignored
        lastElement = 7;
        break;
      }
      case tlv::Parameters: {
        if (lastElement >= 8) {
          BOOST_THROW_EXCEPTION(Error("Parameters element is out of order"));
        }
        m_parameters = *element;
        lastElement = 8;
        break;
      }
      default: {
        if (tlv::isCriticalType(element->type())) {
          BOOST_THROW_EXCEPTION(Error("unrecognized element of critical type " +
                                      to_string(element->type())));
        }
        break;
      }
    }
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

Interest&
Interest::setParameters(const Block& parameters)
{
  if (parameters.type() == tlv::Parameters) {
    m_parameters = parameters;
  }
  else {
    m_parameters = Block(tlv::Parameters, parameters);
  }
  m_wire.reset();
  return *this;
}

Interest&
Interest::setParameters(const uint8_t* buffer, size_t bufferSize)
{
  m_parameters = makeBinaryBlock(tlv::Parameters, buffer, bufferSize);
  m_wire.reset();
  return *this;
}

Interest&
Interest::setParameters(ConstBufferPtr buffer)
{
  m_parameters = Block(tlv::Parameters, std::move(buffer));
  m_wire.reset();
  return *this;
}

Interest&
Interest::unsetParameters()
{
  m_parameters = Block();
  m_wire.reset();
  return *this;
}

// ---- operators ----

bool
operator==(const Interest& lhs, const Interest& rhs)
{
  bool wasCanBePrefixSetOnLhs = lhs.m_isCanBePrefixSet;
  bool wasCanBePrefixSetOnRhs = rhs.m_isCanBePrefixSet;
  lhs.m_isCanBePrefixSet = true;
  rhs.m_isCanBePrefixSet = true;
  BOOST_SCOPE_EXIT_ALL(&) {
    lhs.m_isCanBePrefixSet = wasCanBePrefixSetOnLhs;
    rhs.m_isCanBePrefixSet = wasCanBePrefixSetOnRhs;
  };

  return lhs.wireEncode() == rhs.wireEncode();
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
