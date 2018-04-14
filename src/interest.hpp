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

#ifndef NDN_INTEREST_HPP
#define NDN_INTEREST_HPP

#include "delegation-list.hpp"
#include "name.hpp"
#include "packet-base.hpp"
#include "selectors.hpp"
#include "util/time.hpp"

namespace ndn {

class Data;

/** @var const unspecified_duration_type DEFAULT_INTEREST_LIFETIME;
 *  @brief default value for InterestLifetime
 */
const time::milliseconds DEFAULT_INTEREST_LIFETIME = 4_s;

/** @brief Represents an Interest packet.
 */
class Interest : public PacketBase, public enable_shared_from_this<Interest>
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  /** @brief Construct an Interest with given @p name and @p lifetime.
   *  @throw std::invalid_argument @p lifetime is negative
   *  @warning In certain contexts that use `Interest::shared_from_this()`, Interest must be created
   *           using `make_shared`. Otherwise, `shared_from_this()` will trigger undefined behavior.
   */
  explicit
  Interest(const Name& name = Name(), time::milliseconds lifetime = DEFAULT_INTEREST_LIFETIME);

  /** @brief Construct an Interest by decoding from @p wire.
   *  @warning In certain contexts that use `Interest::shared_from_this()`, Interest must be created
   *           using `make_shared`. Otherwise, `shared_from_this()` will trigger undefined behavior.
   */
  explicit
  Interest(const Block& wire);

  /** @brief Prepend wire encoding to @p encoder in NDN Packet Format v0.2.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Encode to a @c Block.
   *
   *  Normally, this function encodes to NDN Packet Format v0.2. However, if this instance has
   *  cached wire encoding (@c hasWire() is true), the cached encoding is returned and it might
   *  be in v0.3 format.
   */
  const Block&
  wireEncode() const;

  /** @brief Decode from @p wire in NDN Packet Format v0.2 or v0.3.
   */
  void
  wireDecode(const Block& wire);

  /** @brief Check if this instance has cached wire encoding.
   */
  bool
  hasWire() const
  {
    return m_wire.hasWire();
  }

  /** @brief Return a URI-like string that represents the Interest.
   *
   *  The string starts with `getName().toUri()`.
   *  If the Interest contains selectors, they are included as a query string.
   *  Example: "/test/name?ndn.MustBeFresh=1"
   */
  std::string
  toUri() const;

public: // matching
  /** @brief Check if Interest, including selectors, matches the given @p name
   *  @param name The name to be matched. If this is a Data name, it shall contain the
   *              implicit digest component
   */
  bool
  matchesName(const Name& name) const;

  /** @brief Check if Interest can be satisfied by @p data.
   *
   *  This method considers Name, MinSuffixComponents, MaxSuffixComponents,
   *  PublisherPublicKeyLocator, and Exclude.
   *  This method does not consider ChildSelector and MustBeFresh.
   */
  bool
  matchesData(const Data& data) const;

  /** @brief Check if Interest matches @p other interest
   *
   *  Interest matches @p other if both have the same name, selectors, and link.  Other fields
   *  (e.g., Nonce) may be different.
   *
   *  @todo Implement distinguishing Interests by forwarding hint. The current implementation
   *        checks only name+selectors (Issue #3162).
   */
  bool
  matchesInterest(const Interest& other) const;

public: // element access
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

  /** @brief Check whether the CanBePrefix element is present.
   *
   *  This is a getter for the CanBePrefix element as defined in NDN Packet Format v0.3.
   *  In this implementation, it is mapped to the closest v0.2 semantics:
   *  MaxSuffixComponents=1 means CanBePrefix is absent.
   */
  bool
  getCanBePrefix() const
  {
    return m_selectors.getMaxSuffixComponents() != 1;
  }

  /** @brief Add or remove CanBePrefix element.
   *  @param canBePrefix whether CanBePrefix element should be present.
   *
   *  This is a setter for the CanBePrefix element as defined in NDN Packet Format v0.3.
   *  In this implementation, it is mapped to the closest v0.2 semantics:
   *  MaxSuffixComponents=1 means CanBePrefix is absent.
   */
  Interest&
  setCanBePrefix(bool canBePrefix)
  {
    m_selectors.setMaxSuffixComponents(canBePrefix ? -1 : 1);
    m_wire.reset();
    return *this;
  }

  /** @brief Check whether the MustBeFresh element is present.
   *
   *  This is a getter for the MustBeFresh element as defined in NDN Packet Format v0.3.
   *  In this implementation, it is mapped to the closest v0.2 semantics and appears as
   *  MustBeFresh element under Selectors.
   */
  bool
  getMustBeFresh() const
  {
    return m_selectors.getMustBeFresh();
  }

  /** @brief Add or remove MustBeFresh element.
   *  @param mustBeFresh whether MustBeFresh element should be present.
   *
   *  This is a setter for the MustBeFresh element as defined in NDN Packet Format v0.3.
   *  In this implementation, it is mapped to the closest v0.2 semantics and appears as
   *  MustBeFresh element under Selectors.
   */
  Interest&
  setMustBeFresh(bool mustBeFresh)
  {
    m_selectors.setMustBeFresh(mustBeFresh);
    m_wire.reset();
    return *this;
  }

  const DelegationList&
  getForwardingHint() const
  {
    return m_forwardingHint;
  }

  Interest&
  setForwardingHint(const DelegationList& value);

  /** @brief Modify ForwardingHint in-place.
   *  @tparam Modifier a unary function that accepts DelegationList&
   *
   *  This is equivalent to, but more efficient (avoids copying) than:
   *  @code
   *  auto fh = interest.getForwardingHint();
   *  modifier(fh);
   *  interest.setForwardingHint(fh);
   *  @endcode
   */
  template<typename Modifier>
  Interest&
  modifyForwardingHint(const Modifier& modifier)
  {
    modifier(m_forwardingHint);
    m_wire.reset();
    return *this;
  }

  /** @brief Check if the Nonce element is present.
   */
  bool
  hasNonce() const
  {
    return static_cast<bool>(m_nonce);
  }

  /** @brief Get nonce value.
   *
   *  If nonce was not present, it is added and assigned a random value.
   */
  uint32_t
  getNonce() const;

  /** @brief Set nonce value.
   */
  Interest&
  setNonce(uint32_t nonce);

  /** @brief Change nonce value.
   *
   *  If the Nonce element is present, the new nonce value will differ from the old value.
   *  If the Nonce element is not present, this method does nothing.
   */
  void
  refreshNonce();

  time::milliseconds
  getInterestLifetime() const
  {
    return m_interestLifetime;
  }

  /** @brief Set Interest's lifetime
   *  @throw std::invalid_argument @p lifetime is negative
   */
  Interest&
  setInterestLifetime(time::milliseconds lifetime);

public: // Selectors (deprecated)
  /** @brief Check if Interest has any selector present.
   */
  NDN_CXX_DEPRECATED
  bool
  hasSelectors() const
  {
    return !m_selectors.empty();
  }

  NDN_CXX_DEPRECATED
  const Selectors&
  getSelectors() const
  {
    return m_selectors;
  }

  NDN_CXX_DEPRECATED
  Interest&
  setSelectors(const Selectors& selectors)
  {
    m_selectors = selectors;
    m_wire.reset();
    return *this;
  }

  NDN_CXX_DEPRECATED
  int
  getMinSuffixComponents() const
  {
    return m_selectors.getMinSuffixComponents();
  }

  NDN_CXX_DEPRECATED
  Interest&
  setMinSuffixComponents(int minSuffixComponents)
  {
    m_selectors.setMinSuffixComponents(minSuffixComponents);
    m_wire.reset();
    return *this;
  }

  NDN_CXX_DEPRECATED
  int
  getMaxSuffixComponents() const
  {
    return m_selectors.getMaxSuffixComponents();
  }

  NDN_CXX_DEPRECATED
  Interest&
  setMaxSuffixComponents(int maxSuffixComponents)
  {
    m_selectors.setMaxSuffixComponents(maxSuffixComponents);
    m_wire.reset();
    return *this;
  }

  NDN_CXX_DEPRECATED
  const KeyLocator&
  getPublisherPublicKeyLocator() const
  {
    return m_selectors.getPublisherPublicKeyLocator();
  }

  NDN_CXX_DEPRECATED
  Interest&
  setPublisherPublicKeyLocator(const KeyLocator& keyLocator)
  {
    m_selectors.setPublisherPublicKeyLocator(keyLocator);
    m_wire.reset();
    return *this;
  }

  NDN_CXX_DEPRECATED
  const Exclude&
  getExclude() const
  {
    return m_selectors.getExclude();
  }

  NDN_CXX_DEPRECATED
  Interest&
  setExclude(const Exclude& exclude)
  {
    m_selectors.setExclude(exclude);
    m_wire.reset();
    return *this;
  }

  NDN_CXX_DEPRECATED
  int
  getChildSelector() const
  {
    return m_selectors.getChildSelector();
  }

  NDN_CXX_DEPRECATED
  Interest&
  setChildSelector(int childSelector)
  {
    m_selectors.setChildSelector(childSelector);
    m_wire.reset();
    return *this;
  }

private:
  /** @brief Decode @c m_wire as NDN Packet Format v0.2.
   *  @retval true decoding successful.
   *  @retval false decoding failed due to structural error.
   *  @throw tlv::Error decoding error within a sub-element.
   */
  bool
  decode02();

  /** @brief Decode @c m_wire as NDN Packet Format v0.3.
   *  @throw tlv::Error decoding error.
   */
  void
  decode03();

private:
  Name m_name;
  Selectors m_selectors;
  mutable optional<uint32_t> m_nonce;
  time::milliseconds m_interestLifetime;
  DelegationList m_forwardingHint;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Interest);

std::ostream&
operator<<(std::ostream& os, const Interest& interest);

inline bool
operator==(const Interest& lhs, const Interest& rhs)
{
  return lhs.wireEncode() == rhs.wireEncode();
}

inline bool
operator!=(const Interest& lhs, const Interest& rhs)
{
  return !(lhs == rhs);
}

} // namespace ndn

#endif // NDN_INTEREST_HPP
