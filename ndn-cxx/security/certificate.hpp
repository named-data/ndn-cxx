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
 *
 * @author Zhiyi Zhang <dreamerbarrychang@gmail.com>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_CXX_SECURITY_CERTIFICATE_HPP
#define NDN_CXX_SECURITY_CERTIFICATE_HPP

#include "ndn-cxx/data.hpp"

namespace ndn {
namespace security {
inline namespace v2 {

/**
 * @brief Represents an NDN certificate following the version 2.0 format.
 *
 * Example of an NDN certificate name:
 * @code{.unparsed}
 *     /edu/ucla/cs/yingdi/KEY/%03%CD...%F1/%9F%D3...%B7/v=1617592200702
 *     \_________________/    \___________/\___________/\______________/
 *        Identity Name           KeyId      IssuerId       Version
 *     \__________________________________/
 *                   Key Name
 * @endcode
 *
 * Notes:
 * - `KeyId` is an opaque name component to identify an instance of the public key for the
 *   certificate namespace.  The value of KeyId is controlled by the namespace owner.  The
 *   library includes helpers for generation of key IDs using 8-byte random number, SHA-256
 *   digest of the public key, timestamp, or a specified numerical identifier.
 * - `IssuerId` is an opaque name component to identify the issuer of the certificate.  The
 *   value is controlled by the issuer.  The library includes helpers to set issuer ID to a
 *   8-byte random number, SHA-256 digest of the issuer's public key, or a specified numerical
 *   identifier.
 * - `Key %Name` is a logical name of the key used for management purposes.  The key name
 *   includes the identity name, the keyword `KEY`, and the `KeyId` component.
 *
 * @see doc/specs/certificate.rst
 */
class Certificate : public Data
{
public:
  Certificate();

  /**
   * @brief Construct certificate from a data object
   * @throw tlv::Error if data does not follow certificate format
   */
  explicit
  Certificate(Data&& data);

  /**
   * @brief Construct certificate from a data object
   * @throw tlv::Error if data does not follow certificate format
   */
  explicit
  Certificate(const Data& data);

  /**
   * @brief Construct certificate from a wire encoding
   * @throw tlv::Error if wire encoding is invalid or does not follow certificate format
   */
  explicit
  Certificate(const Block& block);

  /**
   * @brief Get key name
   */
  Name
  getKeyName() const;

  /**
   * @brief Get identity name
   */
  Name
  getIdentity() const;

  /**
   * @brief Get key ID
   */
  name::Component
  getKeyId() const;

  /**
   * @brief Get issuer ID
   */
  name::Component
  getIssuerId() const;

  /**
   * @brief Get public key bits (in PKCS#8 format)
   * @throw Error If content is empty
   */
  Buffer
  getPublicKey() const;

  /**
   * @brief Get validity period of the certificate
   */
  ValidityPeriod
  getValidityPeriod() const;

  /**
   * @brief Check if the certificate is valid at @p ts.
   */
  bool
  isValid(const time::system_clock::TimePoint& ts = time::system_clock::now()) const;

  /**
   * @brief Get extension with TLV @p type
   * @throw Error The specified block type does not exist
   */
  Block
  getExtension(uint32_t type) const;

  // @TODO Implement extension enumeration (Issue #3907)
public:
  /**
   * @brief Check if the specified name follows the naming convention for the certificate
   */
  static bool
  isValidName(const Name& certName);

public:
  static const ssize_t VERSION_OFFSET;
  static const ssize_t ISSUER_ID_OFFSET;
  static const ssize_t KEY_COMPONENT_OFFSET;
  static const ssize_t KEY_ID_OFFSET;
  static const size_t MIN_CERT_NAME_LENGTH;
  static const size_t MIN_KEY_NAME_LENGTH;
  static const name::Component KEY_COMPONENT;
  static const name::Component DEFAULT_ISSUER_ID;
};

std::ostream&
operator<<(std::ostream& os, const Certificate& cert);

/**
 * @brief Extract identity namespace from the certificate name @p certName
 */
Name
extractIdentityFromCertName(const Name& certName);

/**
 * @brief Extract key name from the certificate name @p certName
 */
Name
extractKeyNameFromCertName(const Name& certName);

} // inline namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_CERTIFICATE_HPP
