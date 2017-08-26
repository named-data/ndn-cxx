/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_SIGNATURE_HPP
#define NDN_SIGNATURE_HPP

#include "signature-info.hpp"

namespace ndn {

/** @brief Holds SignatureInfo and SignatureValue in a Data packet
 *
 *  A Signature is not a TLV element itself. It collects SignatureInfo and SignatureValue TLV
 *  elements together for easy access.
 *  In most cases, an application should use a subclass of Signature such as @p DigestSha256 , @p
 *  SignatureSha256WithRsa , or @p SignatureSha256WithEcdsa instead of using @p Signature type
 *  directly.
 */
class Signature
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

  Signature() = default;

  explicit
  Signature(const Block& info, const Block& value = Block());

  explicit
  Signature(const SignatureInfo& info, const Block& value = Block());

  /** @brief Determine whether SignatureInfo is valid
   */
  explicit
  operator bool() const
  {
    return m_info.getSignatureType() != -1;
  }

  /** @brief Get SignatureInfo
   */
  const SignatureInfo&
  getSignatureInfo() const
  {
    return m_info;
  }

  /** @brief Get SignatureInfo as wire format
   */
  const Block&
  getInfo() const
  {
    return m_info.wireEncode();
  }

  /** @brief Decode SignatureInfo from wire format
   *  @throw tlv::Error decode error
   */
  void
  setInfo(const Block& info);

  /** @brief Set SignatureInfo
   */
  void
  setInfo(const SignatureInfo& info)
  {
    m_info = info;
  }

  /** @brief Get SignatureValue
   */
  const Block&
  getValue() const
  {
    return m_value;
  }

  /** @brief Set SignatureValue
   *  @throws tlv::Error TLV-TYPE of supplied block is not SignatureValue, or the block does not have TLV-VALUE
   */
  void
  setValue(const Block& value);

public: // SignatureInfo fields
  /** @brief Get SignatureType
   *  @throw Error signature is invalid
   */
  tlv::SignatureTypeValue
  getType() const;

  /** @brief Check if KeyLocator exists in SignatureInfo
   */
  bool
  hasKeyLocator() const
  {
    return m_info.hasKeyLocator();
  }

  /** @brief Get KeyLocator
   *  @throw tlv::Error KeyLocator does not exist in SignatureInfo
   */
  const KeyLocator&
  getKeyLocator() const
  {
    return m_info.getKeyLocator();
  }

  /** @brief Set KeyLocator
   */
  void
  setKeyLocator(const KeyLocator& keyLocator)
  {
    m_info.setKeyLocator(keyLocator);
  }

  /** @brief Unset KeyLocator
   *
   *  @note Subclasses of Signature may provide advisory (non-virtual) override to prevent unsetting
   *        KeyLocator if it is required by the specification.
   */
  void
  unsetKeyLocator()
  {
    m_info.unsetKeyLocator();
  }

protected:
  SignatureInfo m_info;
  mutable Block m_value;
};

bool
operator==(const Signature& lhs, const Signature& rhs);

inline bool
operator!=(const Signature& lhs, const Signature& rhs)
{
  return !(lhs == rhs);
}

} // namespace ndn

#endif // NDN_SIGNATURE_HPP
