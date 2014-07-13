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

#ifndef NDN_SIGNATURE_HPP
#define NDN_SIGNATURE_HPP

#include "common.hpp"
#include "signature-info.hpp"

namespace ndn {

/**
 * A Signature is storage for the signature-related information (info and value) in a Data packet.
 */
class Signature
{
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  /// @deprecated use Tlv::SignatureTypeValue instead.
  enum {
    Sha256 = Tlv::DigestSha256,
    Sha256WithRsa = Tlv::SignatureSha256WithRsa,
    Sha256WithEcdsa = Tlv::SignatureSha256WithEcdsa
  };

  Signature()
  {
  }

  explicit
  Signature(const Block& info, const Block& value = Block());

  explicit
  Signature(const SignatureInfo& info, const Block& value = Block());

  operator bool() const
  {
    return m_info.getSignatureType() != -1;
  }

  /// @brief Get SignatureInfo in the wire format
  const Block&
  getInfo() const
  {
    return m_info.wireEncode(); // will do nothing if wire already exists
  }

  /**
   * @brief Set SignatureInfo from a block
   *
   * @throws Tlv::Error if supplied block is not formatted correctly
   */
  void
  setInfo(const Block& info);

  /// @brief Set SignatureInfo
  void
  setInfo(const SignatureInfo& info)
  {
    m_info = info;
  }

  const Block&
  getValue() const
  {
    m_value.encode(); // will do nothing if wire already exists
    return m_value;
  }

  void
  setValue(const Block& value)
  {
    m_value = value;
  }

  uint32_t
  getType() const
  {
    return m_info.getSignatureType();
  }

  bool
  hasKeyLocator() const
  {
    return m_info.hasKeyLocator();
  }

  /**
   * @brief Get KeyLocator
   *
   * @throws Signature::Error if keyLocator does not exist
   */
  const KeyLocator&
  getKeyLocator() const
  {
    return m_info.getKeyLocator();
  }

  /// @brief Set KeyLocator
  void
  setKeyLocator(const KeyLocator& keyLocator)
  {
    m_info.setKeyLocator(keyLocator);
  }

public: // EqualityComparable concept
  bool
  operator==(const Signature& other) const
  {
    return getInfo() == other.getInfo() &&
      getValue() == other.getValue();
  }

  bool
  operator!=(const Signature& other) const
  {
    return !(*this == other);
  }

protected:
  SignatureInfo m_info;
  mutable Block m_value;
};

} // namespace ndn

#endif // NDN_SIGNATURE_HPP
