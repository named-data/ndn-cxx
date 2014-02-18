/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SELECTORS_HPP
#define NDN_SELECTORS_HPP

#include "common.hpp"
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

  Selectors(int minSuffixComponents, int maxSuffixComponents, 
            const Exclude& exclude,
            int childSelector,
            bool mustBeFresh) 
    : m_minSuffixComponents(minSuffixComponents)
    , m_maxSuffixComponents(maxSuffixComponents)
    , m_exclude(exclude)
    , m_childSelector(childSelector)
    , m_mustBeFresh(mustBeFresh)
  {
  }
  
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
  wireEncode(EncodingImpl<T> &block) const;
  
  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode the input from wire format
   */
  void 
  wireDecode(const Block &wire);

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
    wire_.reset();
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
    wire_.reset();
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
    wire_.reset();
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
    wire_.reset();
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
    wire_.reset();
    return *this;
  }

private:
  int m_minSuffixComponents;
  int m_maxSuffixComponents;  
  Exclude m_exclude;
  int m_childSelector;
  bool m_mustBeFresh;

  mutable Block wire_;
};

inline bool
Selectors::empty() const
{
  return
    (m_minSuffixComponents < 0 &&
     m_maxSuffixComponents < 0 &&
     m_exclude.empty() &&
     m_childSelector < 0 &&
     !m_mustBeFresh);
}

template<bool T>
inline size_t
Selectors::wireEncode(EncodingImpl<T> &block) const
{
  size_t total_len = 0;

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
      total_len += prependBooleanBlock(block, Tlv::MustBeFresh);
    }

  // ChildSelector
  if (getChildSelector() >= 0)
    {
      total_len += prependNonNegativeIntegerBlock(block, Tlv::ChildSelector, getChildSelector());
    }

  // Exclude
  if (!getExclude().empty())
    {
      total_len += getExclude().wireEncode(block);
    }
  
  // PublisherPublicKeyLocator
  /// @todo Implement PublisherPublicKeyLocator selector
  
  // MaxSuffixComponents
  if (getMaxSuffixComponents() >= 0)
    {
      total_len += prependNonNegativeIntegerBlock(block, Tlv::MaxSuffixComponents,
                                                  getMaxSuffixComponents());
    }

  // MinSuffixComponents
  if (getMinSuffixComponents() >= 0)
    {
      total_len += prependNonNegativeIntegerBlock(block, Tlv::MinSuffixComponents,
                                                  getMinSuffixComponents());
    }
  
  total_len += block.prependVarNumber(total_len);
  total_len += block.prependVarNumber(Tlv::Selectors);
  return total_len;
}

inline const Block &
Selectors::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);
  
  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  wire_ = buffer.block();
  return wire_;
}

inline void
Selectors::wireDecode(const Block &wire) 
{
  if (wire.type() != Tlv::Selectors)
    throw Tlv::Error("Unexpected TLV type when decoding Selectors");

  *this = Selectors();
  
  wire_ = wire;
  wire_.parse();

  // MinSuffixComponents
  Block::element_const_iterator val = wire_.find(Tlv::MinSuffixComponents);
  if (val != wire_.elements_end())
    {
      m_minSuffixComponents = readNonNegativeInteger(*val);
    }

  // MaxSuffixComponents
  val = wire_.find(Tlv::MaxSuffixComponents);
  if (val != wire_.elements_end())
    {
      m_maxSuffixComponents = readNonNegativeInteger(*val);
    }

  // Exclude
  val = wire_.find(Tlv::Exclude);
  if (val != wire_.elements_end())
    {
      m_exclude.wireDecode(*val);
    }

  // ChildSelector
  val = wire_.find(Tlv::ChildSelector);
  if (val != wire_.elements_end())
    {
      m_childSelector = readNonNegativeInteger(*val);
    }

  //MustBeFresh aka AnswerOriginKind
  val = wire_.find(Tlv::MustBeFresh);
  if (val != wire_.elements_end())
    {
      m_mustBeFresh = true;
    }
}
  
} // namespace ndn

#endif // NDN_SELECTORS_HPP
