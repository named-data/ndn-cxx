/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_SIGNATURE_INFO_HPP
#define NDN_SIGNATURE_INFO_HPP

#include "ndn-cxx/key-locator.hpp"
#include "ndn-cxx/security/validity-period.hpp"

#include <list>

namespace ndn {

/** @brief Represents a SignatureInfo TLV element
 */
class SignatureInfo
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  /** @brief Create an invalid SignatureInfo
   */
  SignatureInfo();

  /** @brief Create with the specified type and KeyLocator
   */
  explicit
  SignatureInfo(tlv::SignatureTypeValue type, optional<KeyLocator> keyLocator = nullopt);

  /** @brief Create from wire encoding
   *  @throw tlv::Error decode error
   */
  explicit
  SignatureInfo(const Block& wire);

  /** @brief Determine whether SignatureInfo is valid
   */
  explicit
  operator bool() const noexcept
  {
    return m_type != -1;
  }

  /** @brief Fast encoding or block size estimation
   *  @param encoder EncodingEstimator or EncodingBuffer instance
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Encode to wire format
   */
  const Block&
  wireEncode() const;

  /** @brief Decode from wire format
   *  @throw tlv::Error decode error
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

public: // field access
  /** @brief Get SignatureType
   *  @return tlv::SignatureTypeValue, or -1 to indicate invalid SignatureInfo
   */
  int32_t
  getSignatureType() const noexcept
  {
    return m_type;
  }

  /** @brief Set SignatureType
   *  @return a reference to this SignatureInfo, to allow chaining
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
   *  @return a reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the KeyLocator.
   */
  SignatureInfo&
  setKeyLocator(optional<KeyLocator> keyLocator);

  /** @brief Remove KeyLocator
   *  @deprecated Use `setKeyLocator(nullopt)`
   */
  [[deprecated("use setKeyLocator(nullopt)")]]
  void
  unsetKeyLocator();

  /** @brief Check if ValidityPeriod is present
   */
  bool
  hasValidityPeriod() const noexcept
  {
    return !m_otherTlvs.empty() && m_otherTlvs.front().type() == tlv::ValidityPeriod;
  }

  /** @brief Get ValidityPeriod
   *  @throw Error This SignatureInfo does not contain a ValidityPeriod
   */
  security::ValidityPeriod
  getValidityPeriod() const;

  /** @brief Set ValidityPeriod
   *  @return a reference to this SignatureInfo, to allow chaining
   *
   *  Passing `nullopt` will remove the ValidityPeriod.
   */
  SignatureInfo&
  setValidityPeriod(optional<security::ValidityPeriod> validityPeriod);

  /** @brief Remove ValidityPeriod
   *  @deprecated Use `setValidityPeriod(nullopt)`
   */
  [[deprecated("use setValidityPeriod(nullopt)")]]
  void
  unsetValidityPeriod();

  /** @brief Get SignatureType-specific sub-element
   *  @param type TLV-TYPE of sub-element
   *  @throw Error sub-element of specified type does not exist
   */
  const Block&
  getTypeSpecificTlv(uint32_t type) const;

  /** @brief Append SignatureType-specific sub-element
   */
  void
  appendTypeSpecificTlv(const Block& element);

private:
  int32_t m_type = -1;
  optional<KeyLocator> m_keyLocator;
  std::list<Block> m_otherTlvs;

  mutable Block m_wire;

  friend bool
  operator==(const SignatureInfo& lhs, const SignatureInfo& rhs);

  friend std::ostream&
  operator<<(std::ostream& os, const SignatureInfo& info);
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(SignatureInfo);

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

#endif // NDN_SIGNATURE_INFO_HPP
