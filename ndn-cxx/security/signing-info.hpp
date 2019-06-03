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

#ifndef NDN_SECURITY_SIGNING_INFO_HPP
#define NDN_SECURITY_SIGNING_INFO_HPP

#include "ndn-cxx/name.hpp"
#include "ndn-cxx/signature-info.hpp"
#include "ndn-cxx/security/pib/identity.hpp"
#include "ndn-cxx/security/pib/key.hpp"
#include "ndn-cxx/security/security-common.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"

namespace ndn {
namespace security {

/**
 * @brief Signing parameters passed to KeyChain
 *
 * A SigningInfo is invalid if the specified identity/key/certificate does not exist,
 * or the PIB Identity or Key instance is not valid.
 */
class SigningInfo
{
public:
  class Error : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  enum SignerType {
    /// No signer is specified, use default setting or follow the trust schema.
    SIGNER_TYPE_NULL = 0,
    /// Signer is an identity, use its default key and default certificate.
    SIGNER_TYPE_ID = 1,
    /// Signer is a key, use its default certificate.
    SIGNER_TYPE_KEY = 2,
    /// Signer is a certificate, use it directly.
    SIGNER_TYPE_CERT = 3,
    /// Use a SHA-256 digest only, no signer needs to be specified.
    SIGNER_TYPE_SHA256 = 4,
    /// Signer is a HMAC key.
    SIGNER_TYPE_HMAC = 5,
  };

public:
  /**
   * @brief Constructor.
   *
   * @param signerType The type of signer
   * @param signerName The name of signer; interpretation differs per signerType
   * @param signatureInfo A semi-prepared SignatureInfo which contains other information except
   *                      SignatureType and KeyLocator.  If SignatureType and KeyLocator are
   *                      specified, they may be overwritten by KeyChain.
   */
  explicit
  SigningInfo(SignerType signerType = SIGNER_TYPE_NULL,
              const Name& signerName = getEmptyName(),
              const SignatureInfo& signatureInfo = getEmptySignatureInfo());

  /**
   * @brief Construct from a PIB identity.
   */
  explicit
  SigningInfo(const Identity& identity);

  /**
   * @brief Construct from a PIB key.
   */
  explicit
  SigningInfo(const Key& key);

  /**
   * @brief Construct SigningInfo from its string representation.
   *
   * @param signingStr The representative signing string for SigningInfo signing method
   *
   * Syntax of the representative string is as follows:
   * - default signing: "" (empty string)
   * - sign with the default certificate of the default key of an identity: `id:/<my-identity>`
   * - sign with the default certificate of a specific key: `key:/<my-identity>/ksk-1`
   * - sign with a specific certificate: `cert:/<my-identity>/KEY/ksk-1/ID-CERT/%FD%01`
   * - sign with HMAC-SHA-256: `hmac-sha256:<base64-encoded-key>`
   * - sign with SHA-256 (digest only): `id:/localhost/identity/digest-sha256`
   */
  explicit
  SigningInfo(const std::string& signingStr);

  /**
   * @brief Set signer as an identity with name @p identity
   * @post Change the signerType to SIGNER_TYPE_ID
   */
  SigningInfo&
  setSigningIdentity(const Name& identity);

  /**
   * @brief Set signer as a key with name @p keyName
   * @post Change the signerType to SIGNER_TYPE_KEY
   */
  SigningInfo&
  setSigningKeyName(const Name& keyName);

  /**
   * @brief Set signer as a certificate with name @p certificateName
   * @post Change the signerType to SIGNER_TYPE_CERT
   */
  SigningInfo&
  setSigningCertName(const Name& certificateName);

  /**
   * @brief Set signer to a base64-encoded HMAC key
   * @post Change the signerType to SIGNER_TYPE_HMAC
   */
  SigningInfo&
  setSigningHmacKey(const std::string& hmacKey);

  /**
   * @brief Set SHA-256 as the signing method
   * @post Reset signerName, also change the signerType to SIGNER_TYPE_SHA256
   */
  SigningInfo&
  setSha256Signing();

  /**
   * @brief Set signer as a PIB identity handler @p identity
   * @post Change the signerType to SIGNER_TYPE_ID
   */
  SigningInfo&
  setPibIdentity(const Identity& identity);

  /**
   * @brief Set signer as a PIB key handler @p key
   * @post Change the signerType to SIGNER_TYPE_KEY
   */
  SigningInfo&
  setPibKey(const Key& key);

  /**
   * @return Type of the signer
   */
  SignerType
  getSignerType() const
  {
    return m_type;
  }

  /**
   * @return Name of signer; interpretation differs per signerType
   */
  const Name&
  getSignerName() const
  {
    return m_name;
  }

  /**
   * @pre signerType must be SIGNER_TYPE_ID
   * @return the identity handler of signer, or Identity() if getSignerName() should be used
   *         to find the identity
   */
  const Identity&
  getPibIdentity() const
  {
    BOOST_ASSERT(m_type == SIGNER_TYPE_ID);
    return m_identity;
  }

  /**
   * @pre signerType must be SIGNER_TYPE_KEY
   * @return the key handler of signer, or Key() if getSignerName() should be used to find the key
   */
  const Key&
  getPibKey() const
  {
    BOOST_ASSERT(m_type == SIGNER_TYPE_KEY);
    return m_key;
  }

  shared_ptr<transform::PrivateKey>
  getHmacKey() const
  {
    BOOST_ASSERT(m_type == SIGNER_TYPE_HMAC);
    return m_hmacKey;
  }

  /**
   * @brief Set the digest algorithm for signing operations
   */
  SigningInfo&
  setDigestAlgorithm(const DigestAlgorithm& algorithm)
  {
    m_digestAlgorithm = algorithm;
    return *this;
  }

  /**
   * @return The digest algorithm for signing operations
   */
  DigestAlgorithm
  getDigestAlgorithm() const
  {
    return m_digestAlgorithm;
  }

  /**
   * @brief Set a semi-prepared SignatureInfo;
   */
  SigningInfo&
  setSignatureInfo(const SignatureInfo& signatureInfo);

  /**
   * @return Semi-prepared SignatureInfo
   */
  const SignatureInfo&
  getSignatureInfo() const
  {
    return m_info;
  }

public:
  static const Name&
  getEmptyName();

  static const SignatureInfo&
  getEmptySignatureInfo();

  /**
   * @brief A localhost identity to indicate that the signature is generated using SHA-256.
   */
  static const Name&
  getDigestSha256Identity();

  /**
   * @brief A localhost identity to indicate that the signature is generated using an HMAC key.
   */
  static const Name&
  getHmacIdentity();

private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const SigningInfo& lhs, const SigningInfo& rhs)
  {
    return !(lhs != rhs);
  }

  friend bool
  operator!=(const SigningInfo& lhs, const SigningInfo& rhs)
  {
    return lhs.m_type != rhs.m_type ||
           lhs.m_name != rhs.m_name ||
           lhs.m_digestAlgorithm != rhs.m_digestAlgorithm ||
           lhs.m_info != rhs.m_info;
  }

private:
  SignerType m_type;
  Name m_name;
  Identity m_identity;
  Key m_key;
  shared_ptr<transform::PrivateKey> m_hmacKey;
  DigestAlgorithm m_digestAlgorithm;
  SignatureInfo m_info;
};

std::ostream&
operator<<(std::ostream& os, const SigningInfo& si);

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_SIGNING_INFO_HPP
