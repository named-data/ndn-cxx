/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/delegation-list.hpp"
#include "ndn-cxx/name.hpp"
#include "ndn-cxx/detail/packet-base.hpp"
#include "ndn-cxx/util/time.hpp"

#include <boost/logic/tribool.hpp>

namespace ndn {

class Data;

/** @var const unspecified_duration_type DEFAULT_INTEREST_LIFETIME;
 *  @brief default value for InterestLifetime
 */
const time::milliseconds DEFAULT_INTEREST_LIFETIME = 4_s;

/** @brief Represents an Interest packet.
 */
class Interest : public PacketBase, public std::enable_shared_from_this<Interest>
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  /** @brief Construct an Interest with given @p name and @p lifetime.
   *
   *  @throw std::invalid_argument @p name is invalid or @p lifetime is negative
   *  @warning In certain contexts that use `Interest::shared_from_this()`, Interest must be created
   *           using `make_shared`. Otherwise, `shared_from_this()` will trigger undefined behavior.
   */
  explicit
  Interest(const Name& name = Name(), time::milliseconds lifetime = DEFAULT_INTEREST_LIFETIME);

  /** @brief Construct an Interest by decoding from @p wire.
   *
   *  @warning In certain contexts that use `Interest::shared_from_this()`, Interest must be created
   *           using `make_shared`. Otherwise, `shared_from_this()` will trigger undefined behavior.
   */
  explicit
  Interest(const Block& wire);

  /** @brief Prepend wire encoding to @p encoder according to NDN Packet Format v0.3.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Encode into a Block according to NDN Packet Format v0.3.
   */
  const Block&
  wireEncode() const;

  /** @brief Decode from @p wire according to NDN Packet Format v0.3.
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
   *  The string always starts with `getName().toUri()`. After the name, if any of the
   *  Interest's CanBePrefix, MustBeFresh, Nonce, InterestLifetime, or HopLimit fields
   *  are present, their textual representation is appended as a query string.
   *  Example: "/test/name?MustBeFresh&Nonce=123456"
   */
  std::string
  toUri() const;

public: // matching
  /** @brief Check if Interest can be satisfied by @p data.
   *
   *  This method considers Name, CanBePrefix, and MustBeFresh. However, MustBeFresh processing
   *  is limited to rejecting Data with zero/omitted FreshnessPeriod.
   */
  bool
  matchesData(const Data& data) const;

  /** @brief Check if this Interest matches @p other
   *
   *  Two Interests match if both have the same Name, CanBePrefix, and MustBeFresh.
   */
  bool
  matchesInterest(const Interest& other) const;

public: // element access
  const Name&
  getName() const noexcept
  {
    return m_name;
  }

  /** @brief Set the Interest's name.
   *  @throw std::invalid_argument @p name is invalid
   */
  Interest&
  setName(const Name& name);

  /** @brief Declare the default CanBePrefix setting of the application.
   *
   *  As part of transitioning to NDN Packet Format v0.3, the default setting for CanBePrefix
   *  will be changed from "true" to "false". Application developers are advised to review all
   *  Interests expressed by their application and decide what CanBePrefix setting is appropriate
   *  for each Interest, to avoid breaking changes when the transition occurs. Application may
   *  either set CanBePrefix on a per-Interest basis, or declare a default CanBePrefix setting for
   *  all Interests expressed by the application using this function. If an application neither
   *  declares a default nor sets CanBePrefix on every Interest, Interest::wireEncode will print a
   *  one-time warning message.
   *
   *  @note This function should not be used in libraries or in ndn-cxx unit tests.
   *  @sa https://redmine.named-data.net/projects/nfd/wiki/Packet03Transition
   */
  static void
  setDefaultCanBePrefix(bool canBePrefix)
  {
    s_defaultCanBePrefix = canBePrefix;
  }

  /** @brief Check whether the CanBePrefix element is present.
   */
  bool
  getCanBePrefix() const noexcept
  {
    return m_canBePrefix;
  }

  /** @brief Add or remove CanBePrefix element.
   *  @param canBePrefix whether CanBePrefix element should be present.
   */
  Interest&
  setCanBePrefix(bool canBePrefix)
  {
    m_canBePrefix = canBePrefix;
    m_wire.reset();
    m_isCanBePrefixSet = true;
    return *this;
  }

  /** @brief Check whether the MustBeFresh element is present.
   */
  bool
  getMustBeFresh() const noexcept
  {
    return m_mustBeFresh;
  }

  /** @brief Add or remove MustBeFresh element.
   *  @param mustBeFresh whether MustBeFresh element should be present.
   */
  Interest&
  setMustBeFresh(bool mustBeFresh)
  {
    m_mustBeFresh = mustBeFresh;
    m_wire.reset();
    return *this;
  }

  const DelegationList&
  getForwardingHint() const noexcept
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
  hasNonce() const noexcept
  {
    return m_nonce.has_value();
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
  getInterestLifetime() const noexcept
  {
    return m_interestLifetime;
  }

  /** @brief Set the Interest's lifetime.
   *  @throw std::invalid_argument @p lifetime is negative
   */
  Interest&
  setInterestLifetime(time::milliseconds lifetime);

  optional<uint8_t>
  getHopLimit() const noexcept
  {
    return m_hopLimit;
  }

  /** @brief Set the Interest's hop limit.
   *
   *  Use `setHopLimit(nullopt)` to remove any hop limit from the Interest.
   */
  Interest&
  setHopLimit(optional<uint8_t> hopLimit);

  bool
  hasApplicationParameters() const noexcept
  {
    return !m_parameters.empty();
  }

  Block
  getApplicationParameters() const
  {
    if (m_parameters.empty())
      return {};
    else
      return m_parameters.front();
  }

  /** @brief Set ApplicationParameters from a Block.
   *  @return a reference to this Interest
   *
   *  If the block is default-constructed, this will set a zero-length ApplicationParameters
   *  element. Else, if the block's TLV-TYPE is ApplicationParameters, it will be used directly
   *  as this Interest's ApplicationParameters element. Else, the block will be nested into an
   *  ApplicationParameters element.
   *
   *  This function will also recompute the value of the ParametersSha256DigestComponent in the
   *  Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   *  be appended to it.
   */
  Interest&
  setApplicationParameters(const Block& parameters);

  /** @brief Set ApplicationParameters by copying from a raw buffer.
   *  @param value points to a buffer from which the TLV-VALUE of the parameters will be copied;
   *               may be nullptr if @p length is zero
   *  @param length size of the buffer
   *  @return a reference to this Interest
   *
   *  This function will also recompute the value of the ParametersSha256DigestComponent in the
   *  Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   *  be appended to it.
   */
  Interest&
  setApplicationParameters(const uint8_t* value, size_t length);

  /** @brief Set ApplicationParameters from a shared buffer.
   *  @param value buffer containing the TLV-VALUE of the parameters; must not be nullptr
   *  @return a reference to this Interest
   *
   *  This function will also recompute the value of the ParametersSha256DigestComponent in the
   *  Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   *  be appended to it.
   */
  Interest&
  setApplicationParameters(ConstBufferPtr value);

  /** @brief Remove the ApplicationParameters element from this Interest.
   *  @post hasApplicationParameters() == false
   *
   *  This function will also remove any ParametersSha256DigestComponents from the Interest's name.
   */
  Interest&
  unsetApplicationParameters();

public: // ParametersSha256DigestComponent support
  static bool
  getAutoCheckParametersDigest()
  {
    return s_autoCheckParametersDigest;
  }

  static void
  setAutoCheckParametersDigest(bool b)
  {
    s_autoCheckParametersDigest = b;
  }

  /** @brief Check if the ParametersSha256DigestComponent in the name is valid.
   *
   *  Returns true if there is a single ParametersSha256DigestComponent in the name and the digest
   *  value is correct, or if there is no ParametersSha256DigestComponent in the name and the
   *  Interest does not contain any parameters.
   *  Returns false otherwise.
   */
  bool
  isParametersDigestValid() const;

private:
  void
  setApplicationParametersInternal(Block parameters);

  NDN_CXX_NODISCARD shared_ptr<Buffer>
  computeParametersDigest() const;

  /** @brief Append a ParametersSha256DigestComponent to the Interest's name
   *         or update the digest value in the existing component.
   *
   *  @pre The name is assumed to be valid, i.e., it must not contain more than one
   *       ParametersSha256DigestComponent.
   *  @pre hasApplicationParameters() == true
   */
  void
  addOrReplaceParametersDigestComponent();

  /** @brief Return the index of the ParametersSha256DigestComponent in @p name.
   *
   *  @retval pos The name contains exactly one ParametersSha256DigestComponent at index `pos`.
   *  @retval -1  The name contains zero ParametersSha256DigestComponents.
   *  @retval -2  The name contains more than one ParametersSha256DigestComponents.
   */
  static ssize_t
  findParametersDigestComponent(const Name& name);

#ifdef NDN_CXX_HAVE_TESTS
public:
  /// If true, not setting CanBePrefix results in an error in wireEncode().
  static bool s_errorIfCanBePrefixUnset;
#endif // NDN_CXX_HAVE_TESTS

private:
  static boost::logic::tribool s_defaultCanBePrefix;
  static bool s_autoCheckParametersDigest;

  Name m_name;
  DelegationList m_forwardingHint;
  mutable optional<uint32_t> m_nonce;
  time::milliseconds m_interestLifetime;
  optional<uint8_t> m_hopLimit;
  mutable bool m_isCanBePrefixSet = false;
  bool m_canBePrefix = true;
  bool m_mustBeFresh = false;

  // Stores the "Interest parameters", i.e., all maybe-unrecognized non-critical TLV
  // elements that appear at the end of the Interest, starting from ApplicationParameters.
  // If the Interest does not contain any ApplicationParameters TLV, this vector will
  // be empty. Conversely, if this vector is not empty, the first element will always
  // be an ApplicationParameters block. All blocks in this vector are covered by the
  // digest in the ParametersSha256DigestComponent.
  std::vector<Block> m_parameters;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(Interest);

std::ostream&
operator<<(std::ostream& os, const Interest& interest);

} // namespace ndn

#endif // NDN_INTEREST_HPP
