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

#ifndef NDN_SELECTORS_HPP
#define NDN_SELECTORS_HPP

#include "common.hpp"
#include "key-locator.hpp"
#include "exclude.hpp"
#include "encoding/encoding-buffer.hpp"

namespace ndn {

/**
 * @brief Abstraction implementing Interest selectors
 */
class Selectors
{
public:
  Selectors()
  : m_minSuffixComponents(-1)
  , m_maxSuffixComponents(-1)
  , m_childSelector(-1)
  , m_mustBeFresh(false)
  {
  }

  /** @deprecated Selectors().setX(...).setY(...)
   */
  DEPRECATED(
  Selectors(int minSuffixComponents, int maxSuffixComponents,
            const Exclude& exclude,
            int childSelector,
            bool mustBeFresh))
    : m_minSuffixComponents(minSuffixComponents)
    , m_maxSuffixComponents(maxSuffixComponents)
    , m_exclude(exclude)
    , m_childSelector(childSelector)
    , m_mustBeFresh(mustBeFresh)
  {
  }

  /**
   * @brief Create from wire encoding
   */
  Selectors(const Block& wire)
  {
    wireDecode(wire);
  }

  bool
  empty() const;

  /**
   * @brief Fast encoding or block size estimation
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode the input from wire format
   */
  void
  wireDecode(const Block& wire);

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  int
  getMinSuffixComponents() const
  {
    return m_minSuffixComponents;
  }

  Selectors&
  setMinSuffixComponents(int minSuffixComponents)
  {
    m_minSuffixComponents = minSuffixComponents;
    m_wire.reset();
    return *this;
  }

  //

  int
  getMaxSuffixComponents() const
  {
    return m_maxSuffixComponents;
  }

  Selectors&
  setMaxSuffixComponents(int maxSuffixComponents)
  {
    m_maxSuffixComponents = maxSuffixComponents;
    m_wire.reset();
    return *this;
  }

  //

  const KeyLocator&
  getPublisherPublicKeyLocator() const
  {
    return m_publisherPublicKeyLocator;
  }

  Selectors&
  setPublisherPublicKeyLocator(const KeyLocator& keyLocator)
  {
    m_publisherPublicKeyLocator = keyLocator;
    m_wire.reset();
    return *this;
  }

  //

  const Exclude&
  getExclude() const
  {
    return m_exclude;
  }

  Selectors&
  setExclude(const Exclude& exclude)
  {
    m_exclude = exclude;
    m_wire.reset();
    return *this;
  }

  //

  int
  getChildSelector() const
  {
    return m_childSelector;
  }

  Selectors&
  setChildSelector(int childSelector)
  {
    m_childSelector = childSelector;
    m_wire.reset();
    return *this;
  }

  //

  int
  getMustBeFresh() const
  {
    return m_mustBeFresh;
  }

  Selectors&
  setMustBeFresh(bool mustBeFresh)
  {
    m_mustBeFresh = mustBeFresh;
    m_wire.reset();
    return *this;
  }

public: // EqualityComparable concept
  bool
  operator==(const Selectors& other) const
  {
    return wireEncode() == other.wireEncode();
  }

  bool
  operator!=(const Selectors& other) const
  {
    return !(*this == other);
  }

private:
  int m_minSuffixComponents;
  int m_maxSuffixComponents;
  KeyLocator m_publisherPublicKeyLocator;
  Exclude m_exclude;
  int m_childSelector;
  bool m_mustBeFresh;

  mutable Block m_wire;
};

inline bool
Selectors::empty() const
{
  return
    (m_minSuffixComponents < 0 &&
     m_maxSuffixComponents < 0 &&
     m_publisherPublicKeyLocator.empty() &&
     m_exclude.empty() &&
     m_childSelector < 0 &&
     !m_mustBeFresh);
}

template<bool T>
inline size_t
Selectors::wireEncode(EncodingImpl<T>& block) const
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
  if (getMustBeFresh())
    {
      totalLength += prependBooleanBlock(block, Tlv::MustBeFresh);
    }

  // ChildSelector
  if (getChildSelector() >= 0)
    {
      totalLength += prependNonNegativeIntegerBlock(block, Tlv::ChildSelector, getChildSelector());
    }

  // Exclude
  if (!getExclude().empty())
    {
      totalLength += getExclude().wireEncode(block);
    }

  // PublisherPublicKeyLocator
  if (!getPublisherPublicKeyLocator().empty())
    {
      totalLength += getPublisherPublicKeyLocator().wireEncode(block);
    }

  // MaxSuffixComponents
  if (getMaxSuffixComponents() >= 0)
    {
      totalLength += prependNonNegativeIntegerBlock(block, Tlv::MaxSuffixComponents,
                                                    getMaxSuffixComponents());
    }

  // MinSuffixComponents
  if (getMinSuffixComponents() >= 0)
    {
      totalLength += prependNonNegativeIntegerBlock(block, Tlv::MinSuffixComponents,
                                                    getMinSuffixComponents());
    }

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(Tlv::Selectors);
  return totalLength;
}

inline const Block&
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

inline void
Selectors::wireDecode(const Block& wire)
{
  if (wire.type() != Tlv::Selectors)
    throw Tlv::Error("Unexpected TLV type when decoding Selectors");

  *this = Selectors();

  m_wire = wire;
  m_wire.parse();

  // MinSuffixComponents
  Block::element_const_iterator val = m_wire.find(Tlv::MinSuffixComponents);
  if (val != m_wire.elements_end())
    {
      m_minSuffixComponents = readNonNegativeInteger(*val);
    }

  // MaxSuffixComponents
  val = m_wire.find(Tlv::MaxSuffixComponents);
  if (val != m_wire.elements_end())
    {
      m_maxSuffixComponents = readNonNegativeInteger(*val);
    }

  // PublisherPublicKeyLocator
  val = m_wire.find(Tlv::KeyLocator);
  if (val != m_wire.elements_end())
    {
      m_publisherPublicKeyLocator.wireDecode(*val);
    }

  // Exclude
  val = m_wire.find(Tlv::Exclude);
  if (val != m_wire.elements_end())
    {
      m_exclude.wireDecode(*val);
    }

  // ChildSelector
  val = m_wire.find(Tlv::ChildSelector);
  if (val != m_wire.elements_end())
    {
      m_childSelector = readNonNegativeInteger(*val);
    }

  //MustBeFresh aka AnswerOriginKind
  val = m_wire.find(Tlv::MustBeFresh);
  if (val != m_wire.elements_end())
    {
      m_mustBeFresh = true;
    }
}

} // namespace ndn

#endif // NDN_SELECTORS_HPP
