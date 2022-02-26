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

#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/data.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform/digest-filter.hpp"
#include "ndn-cxx/security/transform/step-source.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/util/random.hpp"

#include <boost/range/adaptor/reversed.hpp>

#include <cstring>
#include <sstream>

namespace ndn {

BOOST_CONCEPT_ASSERT((WireEncodable<Interest>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Interest>));
BOOST_CONCEPT_ASSERT((WireDecodable<Interest>));
static_assert(std::is_base_of<tlv::Error, Interest::Error>::value,
              "Interest::Error must inherit from tlv::Error");

bool Interest::s_autoCheckParametersDigest = true;

Interest::Interest(const Name& name, time::milliseconds lifetime)
{
  setName(name);
  setInterestLifetime(lifetime);
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
  // Interest = INTEREST-TYPE TLV-LENGTH
  //              Name
  //              [CanBePrefix]
  //              [MustBeFresh]
  //              [ForwardingHint]
  //              [Nonce]
  //              [InterestLifetime]
  //              [HopLimit]
  //              [ApplicationParameters [InterestSignature]]
  // (elements are encoded in reverse order)

  // sanity check of ApplicationParameters and ParametersSha256DigestComponent
  ssize_t digestIndex = findParametersDigestComponent(getName());
  BOOST_ASSERT(digestIndex != -2); // guaranteed by the checks in setName() and wireDecode()
  if (digestIndex == -1) {
    if (hasApplicationParameters())
      NDN_THROW(Error("Interest with parameters must have a ParametersSha256DigestComponent"));
  }
  else if (!hasApplicationParameters()) {
    NDN_THROW(Error("Interest without parameters must not have a ParametersSha256DigestComponent"));
  }

  size_t totalLength = 0;

  // ApplicationParameters and following elements (in reverse order)
  for (const auto& block : m_parameters | boost::adaptors::reversed) {
    totalLength += prependBlock(encoder, block);
  }

  // HopLimit
  if (getHopLimit()) {
    totalLength += prependBinaryBlock(encoder, tlv::HopLimit, {*m_hopLimit});
  }

  // InterestLifetime
  if (getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::InterestLifetime,
                                                  static_cast<uint64_t>(getInterestLifetime().count()));
  }

  // Nonce
  getNonce(); // if nonce was unset, this generates a fresh nonce
  BOOST_ASSERT(hasNonce());
  totalLength += prependBinaryBlock(encoder, tlv::Nonce, *m_nonce);

  // ForwardingHint
  if (!m_forwardingHint.empty()) {
    totalLength += prependNestedBlock(encoder, tlv::ForwardingHint,
                                      m_forwardingHint.begin(), m_forwardingHint.end());
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

  EncodingBuffer encoder(estimatedSize, 0);
  wireEncode(encoder);

  const_cast<Interest*>(this)->wireDecode(encoder.block());
  return m_wire;
}

void
Interest::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::Interest) {
    NDN_THROW(Error("Interest", wire.type()));
  }
  m_wire = wire;
  m_wire.parse();

  // Interest = INTEREST-TYPE TLV-LENGTH
  //              Name
  //              [CanBePrefix]
  //              [MustBeFresh]
  //              [ForwardingHint]
  //              [Nonce]
  //              [InterestLifetime]
  //              [HopLimit]
  //              [ApplicationParameters [InterestSignature]]

  auto element = m_wire.elements_begin();
  if (element == m_wire.elements_end() || element->type() != tlv::Name) {
    NDN_THROW(Error("Name element is missing or out of order"));
  }
  // decode into a temporary object until we determine that the name is valid, in order
  // to maintain class invariants and thus provide a basic form of exception safety
  Name tempName(*element);
  if (tempName.empty()) {
    NDN_THROW(Error("Name has zero name components"));
  }
  ssize_t digestIndex = findParametersDigestComponent(tempName);
  if (digestIndex == -2) {
    NDN_THROW(Error("Name has more than one ParametersSha256DigestComponent"));
  }
  m_name = std::move(tempName);

  m_canBePrefix = m_mustBeFresh = false;
  m_forwardingHint.clear();
  m_nonce.reset();
  m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  m_hopLimit.reset();
  m_parameters.clear();

  int lastElement = 1; // last recognized element index, in spec order
  for (++element; element != m_wire.elements_end(); ++element) {
    switch (element->type()) {
      case tlv::CanBePrefix: {
        if (lastElement >= 2) {
          NDN_THROW(Error("CanBePrefix element is out of order"));
        }
        if (element->value_size() != 0) {
          NDN_THROW(Error("CanBePrefix element has non-zero TLV-LENGTH"));
        }
        m_canBePrefix = true;
        lastElement = 2;
        break;
      }
      case tlv::MustBeFresh: {
        if (lastElement >= 3) {
          NDN_THROW(Error("MustBeFresh element is out of order"));
        }
        if (element->value_size() != 0) {
          NDN_THROW(Error("MustBeFresh element has non-zero TLV-LENGTH"));
        }
        m_mustBeFresh = true;
        lastElement = 3;
        break;
      }
      case tlv::ForwardingHint: {
        if (lastElement >= 4) {
          NDN_THROW(Error("ForwardingHint element is out of order"));
        }
        // ForwardingHint = FORWARDING-HINT-TYPE TLV-LENGTH 1*Name
        // [previous format]
        // ForwardingHint = FORWARDING-HINT-TYPE TLV-LENGTH 1*Delegation
        // Delegation = DELEGATION-TYPE TLV-LENGTH Preference Name
        element->parse();
        for (const auto& del : element->elements()) {
          switch (del.type()) {
            case tlv::Name:
              try {
                m_forwardingHint.emplace_back(del);
              }
              catch (const tlv::Error&) {
                NDN_THROW_NESTED(Error("Invalid Name in ForwardingHint"));
              }
              break;
            case tlv::LinkDelegation:
              try {
                del.parse();
                m_forwardingHint.emplace_back(del.get(tlv::Name));
              }
              catch (const tlv::Error&) {
                NDN_THROW_NESTED(Error("Invalid Name in ForwardingHint.Delegation"));
              }
              break;
            default:
              if (tlv::isCriticalType(del.type())) {
                NDN_THROW(Error("Unexpected TLV-TYPE " + to_string(del.type()) + " while decoding ForwardingHint"));
              }
              break;
          }
        }
        lastElement = 4;
        break;
      }
      case tlv::Nonce: {
        if (lastElement >= 5) {
          NDN_THROW(Error("Nonce element is out of order"));
        }
        if (element->value_size() != Nonce().size()) {
          NDN_THROW(Error("Nonce element is malformed"));
        }
        m_nonce.emplace();
        std::memcpy(m_nonce->data(), element->value(), m_nonce->size());
        lastElement = 5;
        break;
      }
      case tlv::InterestLifetime: {
        if (lastElement >= 6) {
          NDN_THROW(Error("InterestLifetime element is out of order"));
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
          NDN_THROW(Error("HopLimit element is malformed"));
        }
        m_hopLimit = *element->value();
        lastElement = 7;
        break;
      }
      case tlv::ApplicationParameters: {
        if (lastElement >= 8) {
          break; // ApplicationParameters is non-critical, ignore out-of-order appearance
        }
        BOOST_ASSERT(!hasApplicationParameters());
        m_parameters.push_back(*element);
        lastElement = 8;
        break;
      }
      default: { // unrecognized element
        // if the TLV-TYPE is critical, abort decoding
        if (tlv::isCriticalType(element->type())) {
          NDN_THROW(Error("Unrecognized element of critical type " + to_string(element->type())));
        }
        // if we already encountered ApplicationParameters, store this element as parameter
        if (hasApplicationParameters()) {
          m_parameters.push_back(*element);
        }
        // otherwise, ignore it
        break;
      }
    }
  }

  if (s_autoCheckParametersDigest && !isParametersDigestValid()) {
    NDN_THROW(Error("ParametersSha256DigestComponent does not match the SHA-256 of Interest parameters"));
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
Interest::matchesData(const Data& data) const
{
  size_t interestNameLength = m_name.size();
  const Name& dataName = data.getName();
  size_t fullNameLength = dataName.size() + 1;

  // check Name and CanBePrefix
  if (interestNameLength == fullNameLength) {
    if (m_name.get(-1).isImplicitSha256Digest()) {
      if (m_name != data.getFullName()) {
        return false;
      }
    }
    else {
      // Interest Name is same length as Data full Name, but last component isn't digest
      // so there's no possibility of matching
      return false;
    }
  }
  else if (getCanBePrefix() ? !m_name.isPrefixOf(dataName) : (m_name != dataName)) {
    return false;
  }

  // check MustBeFresh
  if (getMustBeFresh() && data.getFreshnessPeriod() <= 0_ms) {
    return false;
  }

  return true;
}

bool
Interest::matchesInterest(const Interest& other) const
{
  return getName() == other.getName() &&
         getCanBePrefix() == other.getCanBePrefix() &&
         getMustBeFresh() == other.getMustBeFresh();
}

// ---- field accessors and modifiers ----

Interest&
Interest::setName(const Name& name)
{
  ssize_t digestIndex = findParametersDigestComponent(name);
  if (digestIndex == -2) {
    NDN_THROW(std::invalid_argument("Name cannot have more than one ParametersSha256DigestComponent"));
  }

  if (name != m_name) {
    m_name = name;
    if (hasApplicationParameters()) {
      addOrReplaceParametersDigestComponent();
    }
    m_wire.reset();
  }
  return *this;
}

Interest&
Interest::setForwardingHint(std::vector<Name> value)
{
  m_forwardingHint = std::move(value);
  m_wire.reset();
  return *this;
}

static auto
generateNonce()
{
  uint32_t r = random::generateWord32();
  Interest::Nonce n;
  std::memcpy(n.data(), &r, sizeof(r));
  return n;
}

Interest::Nonce
Interest::getNonce() const
{
  if (!hasNonce()) {
    m_nonce = generateNonce();
    m_wire.reset();
  }
  return *m_nonce;
}

Interest&
Interest::setNonce(optional<Interest::Nonce> nonce)
{
  if (nonce != m_nonce) {
    m_nonce = nonce;
    m_wire.reset();
  }
  return *this;
}

void
Interest::refreshNonce()
{
  if (!hasNonce())
    return;

  auto oldNonce = *m_nonce;
  while (m_nonce == oldNonce)
    m_nonce = generateNonce();

  m_wire.reset();
}

Interest&
Interest::setInterestLifetime(time::milliseconds lifetime)
{
  if (lifetime < 0_ms) {
    NDN_THROW(std::invalid_argument("InterestLifetime must be >= 0"));
  }

  if (lifetime != m_interestLifetime) {
    m_interestLifetime = lifetime;
    m_wire.reset();
  }
  return *this;
}

Interest&
Interest::setHopLimit(optional<uint8_t> hopLimit)
{
  if (hopLimit != m_hopLimit) {
    m_hopLimit = hopLimit;
    m_wire.reset();
  }
  return *this;
}

void
Interest::setApplicationParametersInternal(Block parameters)
{
  parameters.encode(); // ensure we have wire encoding needed by computeParametersDigest()
  if (m_parameters.empty()) {
    m_parameters.push_back(std::move(parameters));
  }
  else {
    BOOST_ASSERT(m_parameters[0].type() == tlv::ApplicationParameters);
    m_parameters[0] = std::move(parameters);
  }
}

Interest&
Interest::setApplicationParameters(const Block& parameters)
{
  if (!parameters.isValid()) {
    NDN_THROW(std::invalid_argument("ApplicationParameters block must be valid"));
  }

  if (parameters.type() == tlv::ApplicationParameters) {
    setApplicationParametersInternal(parameters);
  }
  else {
    setApplicationParametersInternal(Block(tlv::ApplicationParameters, parameters));
  }
  addOrReplaceParametersDigestComponent();
  m_wire.reset();
  return *this;
}

Interest&
Interest::setApplicationParameters(span<const uint8_t> value)
{
  setApplicationParametersInternal(makeBinaryBlock(tlv::ApplicationParameters, value));
  addOrReplaceParametersDigestComponent();
  m_wire.reset();
  return *this;
}

Interest&
Interest::setApplicationParameters(const uint8_t* value, size_t length)
{
  if (value == nullptr && length != 0) {
    NDN_THROW(std::invalid_argument("ApplicationParameters buffer cannot be nullptr"));
  }

  return setApplicationParameters(make_span(value, length));
}

Interest&
Interest::setApplicationParameters(ConstBufferPtr value)
{
  if (value == nullptr) {
    NDN_THROW(std::invalid_argument("ApplicationParameters buffer cannot be nullptr"));
  }

  setApplicationParametersInternal(Block(tlv::ApplicationParameters, std::move(value)));
  addOrReplaceParametersDigestComponent();
  m_wire.reset();
  return *this;
}

Interest&
Interest::unsetApplicationParameters()
{
  m_parameters.clear();
  ssize_t digestIndex = findParametersDigestComponent(getName());
  if (digestIndex >= 0) {
    m_name.erase(digestIndex);
  }
  m_wire.reset();
  return *this;
}

bool
Interest::isSigned() const noexcept
{
  return m_parameters.size() >= 3 &&
         getSignatureInfo().has_value() &&
         getSignatureValue().isValid() &&
         !m_name.empty() &&
         m_name[-1].type() == tlv::ParametersSha256DigestComponent;
}

optional<SignatureInfo>
Interest::getSignatureInfo() const
{
  auto blockIt = findFirstParameter(tlv::InterestSignatureInfo);
  if (blockIt != m_parameters.end()) {
    return make_optional<SignatureInfo>(*blockIt, SignatureInfo::Type::Interest);
  }
  return nullopt;
}

Interest&
Interest::setSignatureInfo(const SignatureInfo& info)
{
  // Prepend empty ApplicationParameters element if none present
  if (m_parameters.empty()) {
    m_parameters.push_back(makeEmptyBlock(tlv::ApplicationParameters));
  }

  // Find first existing InterestSignatureInfo (if any)
  auto infoIt = std::find_if(m_parameters.begin(), m_parameters.end(), [] (const Block& block) {
    return block.type() == tlv::InterestSignatureInfo;
  });

  Block encodedInfo = info.wireEncode(SignatureInfo::Type::Interest);
  if (infoIt != m_parameters.end()) {
    if (*infoIt == encodedInfo) {
      // New InterestSignatureInfo is the same as the old InterestSignatureInfo
      return *this;
    }

    // Replace existing InterestSignatureInfo
    *infoIt = std::move(encodedInfo);
  }
  else {
    // Place before first InterestSignatureValue element (if any), else at end
    auto valueIt = findFirstParameter(tlv::InterestSignatureValue);
    m_parameters.insert(valueIt, std::move(encodedInfo));
  }

  addOrReplaceParametersDigestComponent();
  m_wire.reset();
  return *this;
}

Block
Interest::getSignatureValue() const
{
  auto blockIt = findFirstParameter(tlv::InterestSignatureValue);
  if (blockIt != m_parameters.end()) {
    return *blockIt;
  }
  return {};
}

Interest&
Interest::setSignatureValue(ConstBufferPtr value)
{
  if (value == nullptr) {
    NDN_THROW(std::invalid_argument("InterestSignatureValue buffer cannot be nullptr"));
  }

  // Ensure presence of InterestSignatureInfo
  auto infoIt = findFirstParameter(tlv::InterestSignatureInfo);
  if (infoIt == m_parameters.end()) {
    NDN_THROW(Error("InterestSignatureInfo must be present to set InterestSignatureValue"));
  }

  auto valueIt = std::find_if(m_parameters.begin(), m_parameters.end(), [] (const Block& block) {
    return block.type() == tlv::InterestSignatureValue;
  });

  Block valueBlock(tlv::InterestSignatureValue, std::move(value));
  if (valueIt != m_parameters.end()) {
    if (*valueIt == valueBlock) {
      // New InterestSignatureValue is the same as the old InterestSignatureValue
      return *this;
    }

    // Replace existing InterestSignatureValue
    *valueIt = std::move(valueBlock);
  }
  else {
    // Place after first InterestSignatureInfo element
    valueIt = m_parameters.insert(std::next(infoIt), std::move(valueBlock));
  }

  // computeParametersDigest needs encoded SignatureValue
  valueIt->encode();

  addOrReplaceParametersDigestComponent();
  m_wire.reset();
  return *this;
}

InputBuffers
Interest::extractSignedRanges() const
{
  InputBuffers bufs;
  bufs.reserve(2); // For Name range and parameters range

  wireEncode();

  // Get Interest name minus any ParametersSha256DigestComponent
  // Name is guaranteed to be non-empty if wireEncode() does not throw
  BOOST_ASSERT(!m_name.empty());
  if (m_name[-1].type() != tlv::ParametersSha256DigestComponent) {
    NDN_THROW(Error("Interest Name must end with a ParametersSha256DigestComponent"));
  }

  bufs.emplace_back(m_name[0].data(), m_name[-1].data());

  // Ensure InterestSignatureInfo element is present
  auto sigInfoIt = findFirstParameter(tlv::InterestSignatureInfo);
  if (sigInfoIt == m_parameters.end()) {
    NDN_THROW(Error("Interest missing InterestSignatureInfo"));
  }

  // Get range from ApplicationParameters to InterestSignatureValue
  // or end of parameters (whichever is first)
  BOOST_ASSERT(!m_parameters.empty() && m_parameters.begin()->type() == tlv::ApplicationParameters);
  auto lastSignedIt = std::prev(findFirstParameter(tlv::InterestSignatureValue));
  // Note: we assume that both iterators point to the same underlying buffer
  bufs.emplace_back(m_parameters.front().begin(), lastSignedIt->end());

  return bufs;
}

// ---- ParametersSha256DigestComponent support ----

bool
Interest::isParametersDigestValid() const
{
  ssize_t digestIndex = findParametersDigestComponent(getName());
  if (digestIndex == -1) {
    return !hasApplicationParameters();
  }
  // cannot be -2 because of the checks in setName() and wireDecode()
  BOOST_ASSERT(digestIndex >= 0);

  if (!hasApplicationParameters()) {
    return false;
  }

  const auto& digestComponent = getName()[digestIndex];
  auto digest = computeParametersDigest();

  return std::equal(digestComponent.value_begin(), digestComponent.value_end(),
                    digest->begin(), digest->end());
}

shared_ptr<Buffer>
Interest::computeParametersDigest() const
{
  using namespace security::transform;

  StepSource in;
  OBufferStream out;
  in >> digestFilter(DigestAlgorithm::SHA256) >> streamSink(out);

  for (const auto& block : m_parameters) {
    in.write(block);
  }
  in.end();

  return out.buf();
}

void
Interest::addOrReplaceParametersDigestComponent()
{
  BOOST_ASSERT(hasApplicationParameters());

  ssize_t digestIndex = findParametersDigestComponent(getName());
  name::Component digestComponent(tlv::ParametersSha256DigestComponent, computeParametersDigest());

  if (digestIndex == -1) {
    // no existing digest components, append one
    m_name.append(std::move(digestComponent));
  }
  else {
    // cannot be -2 because of the checks in setName() and wireDecode()
    BOOST_ASSERT(digestIndex >= 0);
    // replace the existing digest component
    m_name.set(digestIndex, std::move(digestComponent));
  }
}

ssize_t
Interest::findParametersDigestComponent(const Name& name)
{
  ssize_t pos = -1;
  for (ssize_t i = 0; i < static_cast<ssize_t>(name.size()); i++) {
    if (name[i].isParametersSha256Digest()) {
      if (pos != -1)
        return -2;
      pos = i;
    }
  }
  return pos;
}

std::vector<Block>::const_iterator
Interest::findFirstParameter(uint32_t type) const
{
  return std::find_if(m_parameters.begin(), m_parameters.end(), [type] (const Block& block) {
    return block.type() == type;
  });
}

// ---- operators ----

std::ostream&
operator<<(std::ostream& os, const Interest& interest)
{
  os << interest.getName();

  char delim = '?';
  auto printOne = [&] (const auto&... args) {
    os << delim;
    delim = '&';
    using expand = int[];
    (void)expand{(os << args, 0)...}; // use a fold expression when we switch to C++17
  };

  if (interest.getCanBePrefix()) {
    printOne("CanBePrefix");
  }
  if (interest.getMustBeFresh()) {
    printOne("MustBeFresh");
  }
  if (interest.hasNonce()) {
    printOne("Nonce=", interest.getNonce());
  }
  if (interest.getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    printOne("Lifetime=", interest.getInterestLifetime().count());
  }
  if (interest.getHopLimit()) {
    printOne("HopLimit=", static_cast<unsigned>(*interest.getHopLimit()));
  }

  return os;
}

} // namespace ndn
