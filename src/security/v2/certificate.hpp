/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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
 *
 * @author Zhiyi Zhang <dreamerbarrychang@gmail.com>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_SECURITY_V2_CERTIFICATE_HPP
#define NDN_SECURITY_V2_CERTIFICATE_HPP

#include "../../data.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief The certificate following the certificate format naming convention
 *
 * Overview of NDN certificate format:
 *
 *     CertificateV2 ::= DATA-TLV TLV-LENGTH
 *                         Name      (= /<NameSpace>/KEY/[KeyId]/[IssuerId]/[Version])
 *                         MetaInfo  (.ContentType = KEY)
 *                         Content   (= X509PublicKeyContent)
 *                         SignatureInfo (= CertificateV2SignatureInfo)
 *                         SignatureValue
 *
 *     X509PublicKeyContent ::= CONTENT-TLV TLV-LENGTH
 *                                BYTE+ (= public key bits in PKCS#8 format)
 *
 *     CertificateV2SignatureInfo ::= SIGNATURE-INFO-TYPE TLV-LENGTH
 *                                      SignatureType
 *                                      KeyLocator
 *                                      ValidityPeriod
 *                                      ... optional critical or non-critical extension blocks ...
 *
 * An example of NDN certificate name:
 *
 *     /edu/ucla/cs/yingdi/KEY/%03%CD...%F1/%9F%D3...%B7/%FD%d2...%8E
 *     \_________________/    \___________/ \___________/\___________/
 *    Certificate Namespace      Key Id       Issuer Id     Version
 *         (Identity)
 *     \__________________________________/
 *                   Key Name
 *
 * Notes:
 *
 * - `Key Id` is opaque name component to identify an instance of the public key for the
 *   certificate namespace.  The value of `Key ID` is controlled by the namespace owner.  The
 *   library includes helpers for generation of key IDs using 8-byte random number, SHA-256
 *   digest of the public key, timestamp, and the specified numerical identifiers.
 *
 * - `Issuer Id` is opaque name component to identify issuer of the certificate.  The value is
 *   controlled by the issuer.  The library includes helpers to set issuer ID to a 8-byte
 *   random number, SHA-256 digest of the issuer's public key, and the specified numerical
 *   identifiers.
 *
 * - `Key Name` is a logical name of the key used for management pursposes. Key Name includes
 *   the certificate namespace, keyword `KEY`, and `KeyId` components.
 *
 * @see doc/specs/certificate-format.rst
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
   * @throw ndn::SignatureInfo::Error if the specified block type does not exist
   */
  const Block&
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

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_HPP
