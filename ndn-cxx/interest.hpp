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

#ifndef NDN_CXX_INTEREST_HPP
#define NDN_CXX_INTEREST_HPP

#include "ndn-cxx/detail/packet-base.hpp"
#include "ndn-cxx/name.hpp"
#include "ndn-cxx/security/security-common.hpp"
#include "ndn-cxx/signature-info.hpp"
#include "ndn-cxx/util/string-helper.hpp"
#include "ndn-cxx/util/time.hpp"

#include <array>

#include <boost/endian/conversion.hpp>

namespace ndn {

class Data;

/** @var const unspecified_duration_type DEFAULT_INTEREST_LIFETIME;
 *  @brief default value for InterestLifetime
 */
const time::milliseconds DEFAULT_INTEREST_LIFETIME = 4_s;

/** @brief Represents an %Interest packet.
 *  @sa https://named-data.net/doc/NDN-packet-spec/0.3/interest.html
 */
class Interest : public PacketBase, public std::enable_shared_from_this<Interest>
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  class Nonce final : public std::array<uint8_t, 4>
  {
    using Base = std::array<uint8_t, 4>;

  public:
    Nonce() = default;

    // implicit conversion from uint32_t
    Nonce(uint32_t n) noexcept
    {
      boost::endian::native_to_big_inplace(n);
      std::memcpy(data(), &n, sizeof(n));
    }

    Nonce(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4) noexcept
    {
      data()[0] = n1;
      data()[1] = n2;
      data()[2] = n3;
      data()[3] = n4;
    }

  private: // non-member operators
    // NOTE: the following "hidden friend" operators are available via
    //       argument-dependent lookup only and must be defined inline.

    friend bool
    operator==(const Nonce& lhs, const Nonce& rhs) noexcept
    {
      return static_cast<const Base&>(lhs) == static_cast<const Base&>(rhs);
    }

    friend bool
    operator!=(const Nonce& lhs, const Nonce& rhs) noexcept
    {
      return static_cast<const Base&>(lhs) != static_cast<const Base&>(rhs);
    }

    friend std::ostream&
    operator<<(std::ostream& os, const Nonce& nonce)
    {
      printHex(os, nonce, false);
      return os;
    }
  };

  /** @brief Construct an Interest with given @p name and @p lifetime.
   *
   *  @throw std::invalid_argument @p name is invalid or @p lifetime is negative
   *  @warning In certain contexts that use `Interest::shared_from_this()`, Interest must be created
   *           using `make_shared`. Otherwise, `shared_from_this()` will trigger undefined behavior.
   */
  explicit
  Interest(const Name& name = {}, time::milliseconds lifetime = DEFAULT_INTEREST_LIFETIME);

  /** @brief Construct an Interest by decoding from @p wire.
   *
   *  @warning In certain contexts that use `Interest::shared_from_this()`, Interest must be created
   *           using `make_shared`. Otherwise, `shared_from_this()` will trigger undefined behavior.
   */
  explicit
  Interest(const Block& wire);

  /** @brief Prepend wire encoding to @p encoder.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Encode into a Block.
   */
  const Block&
  wireEncode() const;

  /** @brief Decode from @p wire.
   */
  void
  wireDecode(const Block& wire);

  /** @brief Check if this instance has cached wire encoding.
   */
  bool
  hasWire() const noexcept
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

  span<const Name>
  getForwardingHint() const noexcept
  {
    return m_forwardingHint;
  }

  Interest&
  setForwardingHint(std::vector<Name> value);

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
  Nonce
  getNonce() const;

  /** @brief Set the Interest's nonce.
   *
   *  Use `setNonce(nullopt)` to remove any nonce from the Interest.
   */
  Interest&
  setNonce(optional<Nonce> nonce);

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

  /**
   * @brief Return whether this Interest has any ApplicationParameters.
   */
  bool
  hasApplicationParameters() const noexcept
  {
    return !m_parameters.empty();
  }

  /**
   * @brief Get the ApplicationParameters.
   *
   * If the element is not present, an invalid Block will be returned.
   *
   * @sa hasApplicationParameters()
   */
  Block
  getApplicationParameters() const
  {
    if (m_parameters.empty())
      return {};
    else
      return m_parameters.front();
  }

  /**
   * @brief Set ApplicationParameters from a Block.
   * @param block TLV block to be used as ApplicationParameters; must be valid
   * @return a reference to this Interest
   *
   * If the block's TLV-TYPE is tlv::ApplicationParameters, it will be used directly as
   * this Interest's ApplicationParameters element. Otherwise, the block will be nested
   * into an ApplicationParameters element.
   *
   * This function will also recompute the value of the ParametersSha256DigestComponent in the
   * Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   * be appended to it.
   */
  Interest&
  setApplicationParameters(const Block& block);

  /**
   * @brief Set ApplicationParameters by copying from a contiguous sequence of bytes.
   * @param value buffer from which the TLV-VALUE of the parameters will be copied
   * @return a reference to this Interest
   *
   * This function will also recompute the value of the ParametersSha256DigestComponent in the
   * Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   * be appended to it.
   */
  Interest&
  setApplicationParameters(span<const uint8_t> value);

  /**
   * @brief Set ApplicationParameters by copying from a raw buffer.
   * @param value points to a buffer from which the TLV-VALUE of the parameters will be copied;
   *              may be nullptr if @p length is zero
   * @param length size of the buffer
   * @return a reference to this Interest
   * @deprecated Use setApplicationParameters(span<const uint8_t>)
   *
   * This function will also recompute the value of the ParametersSha256DigestComponent in the
   * Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   * be appended to it.
   */
  [[deprecated("use the overload that takes a span<>")]]
  Interest&
  setApplicationParameters(const uint8_t* value, size_t length);

  /**
   * @brief Set ApplicationParameters from a shared buffer.
   * @param value buffer containing the TLV-VALUE of the parameters; must not be nullptr
   * @return a reference to this Interest
   *
   * This function will also recompute the value of the ParametersSha256DigestComponent in the
   * Interest's name. If the name does not contain a ParametersSha256DigestComponent, one will
   * be appended to it.
   */
  Interest&
  setApplicationParameters(ConstBufferPtr value);

  /**
   * @brief Remove the ApplicationParameters element from this Interest.
   * @return a reference to this Interest
   * @post hasApplicationParameters() == false
   *
   * This function will also remove any InterestSignatureInfo and InterestSignatureValue elements
   * in the Interest, as well as any ParametersSha256DigestComponents in the Interest's name.
   */
  Interest&
  unsetApplicationParameters();

  /** @brief Return whether the Interest is signed
   *  @warning This function only determines whether signature information is present in the
   *           Interest and does not verify that the signature is valid.
   */
  bool
  isSigned() const noexcept;

  /** @brief Get the InterestSignatureInfo
   *  @retval nullopt InterestSignatureInfo is not present
   */
  optional<SignatureInfo>
  getSignatureInfo() const;

  /** @brief Set the InterestSignatureInfo
   */
  Interest&
  setSignatureInfo(const SignatureInfo& info);

  /** @brief Get the InterestSignatureValue
   *
   *  If the element is not present, an invalid Block will be returned.
   */
  Block
  getSignatureValue() const;

  /** @brief Set the InterestSignatureValue
   *  @param value Buffer containing the TLV-VALUE of the InterestSignatureValue; must not be nullptr
   *  @throw Error InterestSignatureInfo is unset
   *
   *  InterestSignatureInfo must be set before setting InterestSignatureValue
   */
  Interest&
  setSignatureValue(ConstBufferPtr value);

  /** @brief Extract ranges of Interest covered by the signature in Packet Specification v0.3
   *  @throw Error Interest cannot be encoded or is missing ranges necessary for signing
   *  @warning The returned pointers will be invalidated if wireDecode() or wireEncode() are called.
   */
  InputBuffers
  extractSignedRanges() const;

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

  std::vector<Block>::const_iterator
  findFirstParameter(uint32_t type) const;

private:
  static bool s_autoCheckParametersDigest;

  Name m_name;
  std::vector<Name> m_forwardingHint;
  mutable optional<Nonce> m_nonce;
  time::milliseconds m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
  optional<uint8_t> m_hopLimit;
  bool m_canBePrefix = false;
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

#endif // NDN_CXX_INTEREST_HPP
