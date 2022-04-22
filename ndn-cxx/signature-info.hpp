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

#ifndef NDN_CXX_SIGNATURE_INFO_HPP
#define NDN_CXX_SIGNATURE_INFO_HPP

#include "ndn-cxx/key-locator.hpp"
#include "ndn-cxx/security/validity-period.hpp"

namespace ndn {

/** @brief Represents a SignatureInfo or InterestSignatureInfo TLV element
 */
class SignatureInfo
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  enum class Type : uint32_t {
    Data = tlv::SignatureInfo,
    Interest = tlv::InterestSignatureInfo
  };

public:
  SignatureInfo();

  /** @brief Create with the specified type and KeyLocator
   */
  explicit
  SignatureInfo(tlv::SignatureTypeValue type, optional<KeyLocator> keyLocator = nullopt);

  /** @brief Create from wire encoding
   *  @param wire Wire to decode from
   *  @param type Which type of SignatureInfo block decoding should expect
   *  @throw tlv::Error Decode error
   */
  explicit
  SignatureInfo(const Block& wire, Type type = Type::Data);

  /** @brief Determine whether SignatureInfo is valid
   */
  explicit
  operator bool() const noexcept
  {
    return m_type != -1;
  }

  /** @brief Fast encoding or block size estimation
   *  @param encoder EncodingEstimator or EncodingBuffer instance
   *  @param type Which type of SignatureInfo block to encode
   *
   *  Elements are encoded in the following order: SignatureType, KeyLocator (if present), and
   *  other elements in the order they were set (changing the value of an already present element
   *  will not change that element's encoding order).
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder, Type type = Type::Data) const;

  /** @brief Encode to wire format
   *  @param type Which type of SignatureInfo block to encode
   *
   *  Elements are encoded in the following order: SignatureType, KeyLocator (if present), and
   *  other elements in the order they were set (changing the value of an already present element
   *  will not change that element's encoding order).
   */
  const Block&
  wireEncode(Type type = Type::Data) const;

  /** @brief Decode from wire format
   *  @param wire Wire to decode from
   *  @param type Which type of SignatureInfo block decoding should expect
   *  @throw tlv::Error Decode error
   */
  void
  wireDecode(const Block& wire, Type type = Type::Data);

  /** @brief Check if this instance has cached wire encoding.
   */
  bool
  hasWire() const noexcept
  {
    return m_wire.hasWire();
  }

public: // field access
  /** @brief Get SignatureType
   *  @return tlv::SignatureTypeValue, or -1 to indicate an invalid SignatureInfo
   */
  int32_t
  getSignatureType() const noexcept
  {
    return m_type;
  }

  /** @brief Set SignatureType
   *  @return A reference to this SignatureInfo, to allow chaining
   */
  SignatureInfo&
  setSignatureType(tlv::SignatureTypeValue type);

  /** @brief Check if KeyLocator is present
   */
  bool
  hasKeyLocator() const noexcept
  {
    return m_keyLocator.has_value();
  }

  /** @brief Get KeyLocator
   *  @throw Error This SignatureInfo does not contain a KeyLocator
   */
  const KeyLocator&
  getKeyLocator() const;

  /** @brief Set KeyLocator
   *  @return A reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the KeyLocator.
   */
  SignatureInfo&
  setKeyLocator(optional<KeyLocator> keyLocator);

  /** @brief Get ValidityPeriod
   *  @throw Error This SignatureInfo does not contain a ValidityPeriod
   */
  security::ValidityPeriod
  getValidityPeriod() const;

  /** @brief Append or replace ValidityPeriod
   *  @return A reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the ValidityPeriod.
   */
  SignatureInfo&
  setValidityPeriod(optional<security::ValidityPeriod> validityPeriod);

  /** @brief Get SignatureNonce
   *  @retval nullopt SignatureNonce is not set
   */
  optional<std::vector<uint8_t>>
  getNonce() const;

  /** @brief Append or replace SignatureNonce
   *  @return A reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the SignatureNonce.
   */
  SignatureInfo&
  setNonce(optional<span<const uint8_t>> nonce);

  /** @brief Get SignatureTime
   *  @retval nullopt SignatureTime is not set
   */
  optional<time::system_clock::time_point>
  getTime() const;

  /** @brief Append or replace SignatureTime
   *  @return A reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the SignatureTime.
   */
  SignatureInfo&
  setTime(optional<time::system_clock::time_point> time = time::system_clock::now());

  /** @brief Get SignatureSeqNum
   *  @retval nullopt SignatureSeqNum is not set
   */
  optional<uint64_t>
  getSeqNum() const;

  /** @brief Append or replace SignatureSeqNum
   *  @return A reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the SignatureSeqNum.
   */
  SignatureInfo&
  setSeqNum(optional<uint64_t> seqNum);

  /** @brief Get first custom TLV element with the specified TLV-TYPE
   *  @param type TLV-TYPE of element to get
   *  @retval nullopt No custom TLV elements with the specified TLV-TYPE exist
   */
  optional<Block>
  getCustomTlv(uint32_t type) const;

  /** @brief Append an arbitrary TLV element to this SignatureInfo
   *
   *  If an element of the same TLV-TYPE already exists, it will be replaced by the new element.
   */
  void
  addCustomTlv(Block block);

  /** @brief Remove all arbitrary TLV elements with the specified TLV-TYPE from this SignatureInfo
   *  @param type TLV-TYPE of elements to remove
   */
  void
  removeCustomTlv(uint32_t type);

private:
  std::vector<Block>::const_iterator
  findOtherTlv(uint32_t type) const;

private:
  int32_t m_type = -1;
  optional<KeyLocator> m_keyLocator;
  std::vector<Block> m_otherTlvs;

  mutable Block m_wire;

  friend bool
  operator==(const SignatureInfo& lhs, const SignatureInfo& rhs);

  friend std::ostream&
  operator<<(std::ostream& os, const SignatureInfo& info);
};

#ifndef DOXYGEN
extern template size_t
SignatureInfo::wireEncode<encoding::EncoderTag>(EncodingBuffer&, SignatureInfo::Type) const;

extern template size_t
SignatureInfo::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, SignatureInfo::Type) const;
#endif

bool
operator==(const SignatureInfo& lhs, const SignatureInfo& rhs);

inline bool
operator!=(const SignatureInfo& lhs, const SignatureInfo& rhs)
{
  return !(lhs == rhs);
}

std::ostream&
operator<<(std::ostream& os, const SignatureInfo& info);

} // namespace ndn

#endif // NDN_CXX_SIGNATURE_INFO_HPP
