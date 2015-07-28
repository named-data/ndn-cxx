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

#include "selectors.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/block-helpers.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Selectors>));
BOOST_CONCEPT_ASSERT((WireEncodable<Selectors>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Selectors>));
BOOST_CONCEPT_ASSERT((WireDecodable<Selectors>));
static_assert(std::is_base_of<tlv::Error, Selectors::Error>::value,
              "Selectors::Error must inherit from tlv::Error");

Selectors::Selectors()
  : m_minSuffixComponents(-1)
  , m_maxSuffixComponents(-1)
  , m_childSelector(-1)
  , m_mustBeFresh(false)
{
}

Selectors::Selectors(const Block& wire)
{
  wireDecode(wire);
}

bool
Selectors::empty() const
{
  return m_minSuffixComponents < 0 &&
         m_maxSuffixComponents < 0 &&
         m_publisherPublicKeyLocator.empty() &&
         m_exclude.empty() &&
         m_childSelector < 0 &&
         !m_mustBeFresh;
}

template<encoding::Tag TAG>
size_t
Selectors::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Selectors ::= SELECTORS-TYPE TLV-LENGTH
  //                 MinSuffixComponents?
  //                 MaxSuffixComponents?
  //                 PublisherPublicKeyLocator?
  //                 Exclude?
  //                 ChildSelector?
  //                 MustBeFresh?

  // (reverse encoding)

  // MustBeFresh
  if (getMustBeFresh()) {
    totalLength += prependEmptyBlock(encoder, tlv::MustBeFresh);
  }

  // ChildSelector
  if (getChildSelector() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::ChildSelector, getChildSelector());
  }

  // Exclude
  if (!getExclude().empty()) {
    totalLength += getExclude().wireEncode(encoder);
  }

  // PublisherPublicKeyLocator
  if (!getPublisherPublicKeyLocator().empty()) {
    totalLength += getPublisherPublicKeyLocator().wireEncode(encoder);
  }

  // MaxSuffixComponents
  if (getMaxSuffixComponents() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::MaxSuffixComponents,
                                                  getMaxSuffixComponents());
  }

  // MinSuffixComponents
  if (getMinSuffixComponents() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::MinSuffixComponents,
                                                  getMinSuffixComponents());
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Selectors);
  return totalLength;
}

template size_t
Selectors::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Selectors::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
Selectors::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
Selectors::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::Selectors)
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV type when decoding Selectors"));

  *this = Selectors();

  m_wire = wire;
  m_wire.parse();

  // MinSuffixComponents
  Block::element_const_iterator val = m_wire.find(tlv::MinSuffixComponents);
  if (val != m_wire.elements_end()) {
    m_minSuffixComponents = readNonNegativeInteger(*val);
  }

  // MaxSuffixComponents
  val = m_wire.find(tlv::MaxSuffixComponents);
  if (val != m_wire.elements_end()) {
    m_maxSuffixComponents = readNonNegativeInteger(*val);
  }

  // PublisherPublicKeyLocator
  val = m_wire.find(tlv::KeyLocator);
  if (val != m_wire.elements_end()) {
    m_publisherPublicKeyLocator.wireDecode(*val);
  }

  // Exclude
  val = m_wire.find(tlv::Exclude);
  if (val != m_wire.elements_end()) {
    m_exclude.wireDecode(*val);
  }

  // ChildSelector
  val = m_wire.find(tlv::ChildSelector);
  if (val != m_wire.elements_end()) {
    m_childSelector = readNonNegativeInteger(*val);
  }

  // MustBeFresh
  val = m_wire.find(tlv::MustBeFresh);
  if (val != m_wire.elements_end()) {
    m_mustBeFresh = true;
  }
}

Selectors&
Selectors::setMinSuffixComponents(int minSuffixComponents)
{
  m_minSuffixComponents = minSuffixComponents;
  m_wire.reset();
  return *this;
}

Selectors&
Selectors::setMaxSuffixComponents(int maxSuffixComponents)
{
  m_maxSuffixComponents = maxSuffixComponents;
  m_wire.reset();
  return *this;
}

Selectors&
Selectors::setPublisherPublicKeyLocator(const KeyLocator& keyLocator)
{
  m_publisherPublicKeyLocator = keyLocator;
  m_wire.reset();
  return *this;
}

Selectors&
Selectors::setExclude(const Exclude& exclude)
{
  m_exclude = exclude;
  m_wire.reset();
  return *this;
}

Selectors&
Selectors::setChildSelector(int childSelector)
{
  m_childSelector = childSelector;
  m_wire.reset();
  return *this;
}

Selectors&
Selectors::setMustBeFresh(bool mustBeFresh)
{
  m_mustBeFresh = mustBeFresh;
  m_wire.reset();
  return *this;
}

bool
Selectors::operator==(const Selectors& other) const
{
  return wireEncode() == other.wireEncode();
}

} // namespace ndn
