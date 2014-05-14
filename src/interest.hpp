/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * Based on code originally written by Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_INTEREST_HPP
#define NDN_INTEREST_HPP

#include "common.hpp"
#include "name.hpp"
#include "selectors.hpp"
#include "interest-filter.hpp"
#include "management/nfd-local-control-header.hpp"

namespace ndn {

class Data;

const time::seconds DEFAULT_INTEREST_LIFETIME = time::seconds(4);

/**
 * An Interest holds a Name and other fields for an interest.
 */
class Interest : public enable_shared_from_this<Interest>
{
public:
  /**
   * @brief Create a new Interest with an empty name (`ndn:/`)
   *
   * Note that in certain contexts that use Interest::shared_from_this(), Interest must be
   * created using `make_shared`:
   *
   *     shared_ptr<Interest> interest = make_shared<Interest>();
   *
   * Otherwise, Interest::shared_from_this() will throw an exception.
   */
  Interest()
    : m_nonce(0)
    , m_scope(-1)
    , m_interestLifetime(time::milliseconds::min())
  {
  }

  /**
   * @brief Create a new Interest with the given name
   *
   * @param name The name for the interest.
   *
   * Note that in certain contexts that use Interest::shared_from_this(), Interest must be
   * created using `make_shared`:
   *
   *     shared_ptr<Interest> interest = make_shared<Interest>(name);
   *
   * Otherwise, Interest::shared_from_this() will throw an exception.
   */
  Interest(const Name& name)
    : m_name(name)
    , m_nonce(0)
    , m_scope(-1)
    , m_interestLifetime(time::milliseconds::min())
  {
  }

  /**
   * @brief Create a new Interest with the given name and interest lifetime
   *
   * @param name             The name for the interest.
   * @param interestLifetime The interest lifetime in time::milliseconds, or -1 for none.
   *
   * Note that in certain contexts that use Interest::shared_from_this(), Interest must be
   * created using `make_shared`:
   *
   *     shared_ptr<Interest> interest = make_shared<Interest>(name, time::seconds(1));
   *
   * Otherwise, Interest::shared_from_this() will throw an exception.
   */
  Interest(const Name& name, const time::milliseconds& interestLifetime)
    : m_name(name)
    , m_nonce(0)
    , m_scope(-1)
    , m_interestLifetime(interestLifetime)
  {
  }

  /**
   * @brief Create a new Interest for the given name, selectors, and guiders
   *
   * Note that in certain contexts that use Interest::shared_from_this(), Interest must be
   * created using `make_shared`:
   *
   *     shared_ptr<Interest> interest = make_shared<Interest>(...);
   *
   * Otherwise, Interest::shared_from_this() will throw an exception.
   */
  Interest(const Name& name,
           const Selectors& selectors,
           int scope,
           const time::milliseconds& interestLifetime,
           uint32_t nonce = 0)
    : m_name(name)
    , m_selectors(selectors)
    , m_nonce(nonce)
    , m_scope(scope)
    , m_interestLifetime(interestLifetime)
  {
  }

  /**
   * @brief Create a new Interest for the given name and parameters
   *
   * @deprecated Interest().setX(...).setY(...)
   *             or use the overload taking Selectors
   *
   * Note that in certain contexts that use Interest::shared_from_this(), Interest must be
   * created using `make_shared`:
   *
   *     shared_ptr<Interest> interest = make_shared<Interest>(...);
   *
   * Otherwise, Interest::shared_from_this() will throw an exception.
   */
  Interest(const Name& name,
           int minSuffixComponents, int maxSuffixComponents,
           const Exclude& exclude,
           int childSelector,
           bool mustBeFresh,
           int scope,
           const time::milliseconds& interestLifetime,
           uint32_t nonce = 0)
    : m_name(name)
    , m_selectors(minSuffixComponents, maxSuffixComponents, exclude, childSelector, mustBeFresh)
    , m_nonce(nonce)
    , m_scope(scope)
    , m_interestLifetime(interestLifetime)
  {
  }

  /**
   * @brief Create from wire encoding
   *
   * Note that in certain contexts that use Interest::shared_from_this(), Interest must be
   * created using `make_shared`:
   *
   *     shared_ptr<Interest> interest = make_shared<Interest>(wire);
   *
   * Otherwise, Interest::shared_from_this() will throw an exception.
   */
  explicit
  Interest(const Block& wire)
  {
    wireDecode(wire);
  }

  /**
   * @brief Fast encoding or block size estimation
   */
  template<bool T>
  inline size_t
  wireEncode(EncodingImpl<T>& block) const;

  /**
   * @brief Encode to a wire format
   */
  inline const Block&
  wireEncode() const;

  /**
   * @brief Decode from the wire format
   */
  inline void
  wireDecode(const Block& wire);

  /**
   * @brief Check if already has wire
   */
  inline bool
  hasWire() const;

  /**
   * @brief Encode the name according to the NDN URI Scheme
   *
   * If there are interest selectors, this method will append "?" and add the selectors as
   * a query string.  For example, "/test/name?ndn.ChildSelector=1"
   */
  inline std::string
  toUri() const;

  /**
   * @brief Check if Interest has any selectors present
   */
  inline bool
  hasSelectors() const;

  /**
   * @brief Check if Interest, including selectors, matches the given @p name
   *
   * @param name The name to be matched. If this is a Data name, it shall contain the
   *             implicit digest component
   */
  bool
  matchesName(const Name& name) const;

  /**
   * @brief Check if Interest can be satisfied by @p data.
   *
   * This method considers Name, MinSuffixComponents, MaxSuffixComponents,
   * PublisherPublicKeyLocator, and Exclude.
   * This method does not consider ChildSelector and MustBeFresh.
   *
   * @todo recognize implicit digest component
   */
  bool
  matchesData(const Data& data) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // Getters/setters

  const Name&
  getName() const
  {
    return m_name;
  }

  Interest&
  setName(const Name& name)
  {
    m_name = name;
    m_wire.reset();
    return *this;
  }

  //

  const Selectors&
  getSelectors() const
  {
    return m_selectors;
  }

  Interest&
  setSelectors(const Selectors& selectors)
  {
    m_selectors = selectors;
    m_wire.reset();
    return *this;
  }

  //

  int
  getScope() const
  {
    return m_scope;
  }

  Interest&
  setScope(int scope)
  {
    m_scope = scope;
    m_wire.reset();
    return *this;
  }

  //

  const time::milliseconds&
  getInterestLifetime() const
  {
    return m_interestLifetime;
  }

  Interest&
  setInterestLifetime(const time::milliseconds& interestLifetime)
  {
    m_interestLifetime = interestLifetime;
    m_wire.reset();
    return *this;
  }

  //

  /**
   * @brief Get Interest's nonce
   *
   * If nonce was not set before this call, it will be automatically assigned to a random value
   *
   * Const reference needed for C decoding
   */
  const uint32_t&
  getNonce() const;

  Interest&
  setNonce(uint32_t nonce)
  {
    m_nonce = nonce;
    m_wire.reset();
    return *this;
  }

  //

  nfd::LocalControlHeader&
  getLocalControlHeader()
  {
    return m_localControlHeader;
  }

  const nfd::LocalControlHeader&
  getLocalControlHeader() const
  {
    return m_localControlHeader;
  }

  // helper methods for LocalControlHeader

  uint64_t
  getIncomingFaceId() const
  {
    return getLocalControlHeader().getIncomingFaceId();
  }

  Interest&
  setIncomingFaceId(uint64_t incomingFaceId)
  {
    getLocalControlHeader().setIncomingFaceId(incomingFaceId);
    // ! do not reset Interest's wire !
    return *this;
  }

  //

  // NextHopFaceId helpers make sense only for Interests

  uint64_t
  getNextHopFaceId() const
  {
    return getLocalControlHeader().getNextHopFaceId();
  }

  Interest&
  setNextHopFaceId(uint64_t nextHopFaceId)
  {
    getLocalControlHeader().setNextHopFaceId(nextHopFaceId);
    // ! do not reset Interest's wire !
    return *this;
  }

  //

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // Wrappers for Selectors
  //

  int
  getMinSuffixComponents() const
  {
    return m_selectors.getMinSuffixComponents();
  }

  Interest&
  setMinSuffixComponents(int minSuffixComponents)
  {
    m_selectors.setMinSuffixComponents(minSuffixComponents);
    m_wire.reset();
    return *this;
  }

  //

  int
  getMaxSuffixComponents() const
  {
    return m_selectors.getMaxSuffixComponents();
  }

  Interest&
  setMaxSuffixComponents(int maxSuffixComponents)
  {
    m_selectors.setMaxSuffixComponents(maxSuffixComponents);
    m_wire.reset();
    return *this;
  }

  //

  const KeyLocator&
  getPublisherPublicKeyLocator() const
  {
    return m_selectors.getPublisherPublicKeyLocator();
  }

  Interest&
  setPublisherPublicKeyLocator(const KeyLocator& keyLocator)
  {
    m_selectors.setPublisherPublicKeyLocator(keyLocator);
    m_wire.reset();
    return *this;
  }

  //

  const Exclude&
  getExclude() const
  {
    return m_selectors.getExclude();
  }

  Interest&
  setExclude(const Exclude& exclude)
  {
    m_selectors.setExclude(exclude);
    m_wire.reset();
    return *this;
  }

  //

  int
  getChildSelector() const
  {
    return m_selectors.getChildSelector();
  }

  Interest&
  setChildSelector(int childSelector)
  {
    m_selectors.setChildSelector(childSelector);
    m_wire.reset();
    return *this;
  }

  //

  int
  getMustBeFresh() const
  {
    return m_selectors.getMustBeFresh();
  }

  Interest&
  setMustBeFresh(bool mustBeFresh)
  {
    m_selectors.setMustBeFresh(mustBeFresh);
    m_wire.reset();
    return *this;
  }

public: // EqualityComparable concept
  bool
  operator==(const Interest& other) const
  {
    return wireEncode() == other.wireEncode();
  }

  bool
  operator!=(const Interest& other) const
  {
    return !(*this == other);
  }

private:
  Name m_name;
  Selectors m_selectors;
  mutable uint32_t m_nonce;
  int m_scope;
  time::milliseconds m_interestLifetime;

  mutable Block m_wire;

  nfd::LocalControlHeader m_localControlHeader;
  friend class nfd::LocalControlHeader;
};

std::ostream&
operator<<(std::ostream& os, const Interest& interest);

inline std::string
Interest::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

inline bool
Interest::hasSelectors() const
{
  return !m_selectors.empty();
}

template<bool T>
inline size_t
Interest::wireEncode(EncodingImpl<T>& block) const
{
  size_t totalLength = 0;

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                Scope?
  //                InterestLifetime?

  // (reverse encoding)

  // InterestLifetime
  if (getInterestLifetime() >= time::milliseconds::zero() &&
      getInterestLifetime() != DEFAULT_INTEREST_LIFETIME)
    {
      totalLength += prependNonNegativeIntegerBlock(block,
                                                    Tlv::InterestLifetime,
                                                    getInterestLifetime().count());
    }

  // Scope
  if (getScope() >= 0)
    {
      totalLength += prependNonNegativeIntegerBlock(block, Tlv::Scope, getScope());
    }

  // Nonce
  totalLength += prependNonNegativeIntegerBlock(block, Tlv::Nonce, getNonce());

  // Selectors
  if (!getSelectors().empty())
    {
      totalLength += getSelectors().wireEncode(block);
    }

  // Name
  totalLength += getName().wireEncode(block);

  totalLength += block.prependVarNumber (totalLength);
  totalLength += block.prependVarNumber (Tlv::Interest);
  return totalLength;
}

inline const Block&
Interest::wireEncode() const
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
Interest::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                Scope?
  //                InterestLifetime?

  if (m_wire.type() != Tlv::Interest)
    throw Tlv::Error("Unexpected TLV number when decoding Interest");

  // Name
  m_name.wireDecode(m_wire.get(Tlv::Name));

  // Selectors
  Block::element_const_iterator val = m_wire.find(Tlv::Selectors);
  if (val != m_wire.elements_end())
    {
      m_selectors.wireDecode(*val);
    }
  else
    m_selectors = Selectors();

  // Nonce
  val = m_wire.find(Tlv::Nonce);
  if (val != m_wire.elements_end())
    {
      m_nonce = readNonNegativeInteger(*val);
    }
  else
    m_nonce = 0;

  // Scope
  val = m_wire.find(Tlv::Scope);
  if (val != m_wire.elements_end())
    {
      m_scope = readNonNegativeInteger(*val);
    }
  else
    m_scope = -1;

  // InterestLifetime
  val = m_wire.find(Tlv::InterestLifetime);
  if (val != m_wire.elements_end())
    {
      m_interestLifetime = time::milliseconds(readNonNegativeInteger(*val));
    }
  else
    {
      m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
    }
}

inline bool
Interest::hasWire() const
{
  return m_wire.hasWire();
}


} // namespace ndn

#endif // NDN_INTEREST_HPP
