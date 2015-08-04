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

#ifndef NDN_SIGNATURE_INFO_HPP
#define NDN_SIGNATURE_INFO_HPP

#include "encoding/tlv.hpp"
#include "key-locator.hpp"
#include "security/validity-period.hpp"
#include <list>

namespace ndn {

class SignatureInfo
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

  SignatureInfo();

  explicit
  SignatureInfo(tlv::SignatureTypeValue type);

  SignatureInfo(tlv::SignatureTypeValue type, const KeyLocator& keyLocator);

  /**
   * @brief Generate SignatureInfo from a block
   *
   * @throws tlv::Error if supplied block is not formatted correctly
   */
  explicit
  SignatureInfo(const Block& block);

  /// @brief Set SignatureType
  void
  setSignatureType(tlv::SignatureTypeValue type);

  /// @brief Get SignatureType
  int32_t
  getSignatureType() const
  {
    return m_type;
  }

  /// @brief Check if KeyLocator is set
  bool
  hasKeyLocator() const
  {
    return m_hasKeyLocator;
  }

  /// @brief Set KeyLocator
  void
  setKeyLocator(const KeyLocator& keyLocator);

  /// @brief Unset KeyLocator
  void
  unsetKeyLocator();

  /**
   * @brief Get KeyLocator
   *
   * @throws SignatureInfo::Error if keyLocator does not exist
   */
  const KeyLocator&
  getKeyLocator() const;

  /// @brief Set ValidityPeriod
  void
  setValidityPeriod(const security::ValidityPeriod& validityPeriod);

  /// @brief Unset ValidityPeriod
  void
  unsetValidityPeriod();

  /// @brief Get ValidityPeriod
  security::ValidityPeriod
  getValidityPeriod() const;

  /// @brief Append signature type specific tlv block
  void
  appendTypeSpecificTlv(const Block& block);

  /**
   * @brief Get signature type specific tlv block
   *
   * @throws SignatureInfo::Error if the block does not exist
   */
  const Block&
  getTypeSpecificTlv(uint32_t type) const;

  /// @brief Encode to a wire format or estimate wire format
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /// @brief Encode to a wire format
  const Block&
  wireEncode() const;

  /// @brief Decode from a wire format
  void
  wireDecode(const Block& wire);

public: // EqualityComparable concept
  bool
  operator==(const SignatureInfo& rhs) const;

  bool
  operator!=(const SignatureInfo& rhs) const
  {
    return !(*this == rhs);
  }

private:
  int32_t m_type;
  bool m_hasKeyLocator;
  KeyLocator m_keyLocator;
  std::list<Block> m_otherTlvs;

  mutable Block m_wire;
};

} // namespace ndn

#endif // NDN_SIGNATURE_INFO_HPP
