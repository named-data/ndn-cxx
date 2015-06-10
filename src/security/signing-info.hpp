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

#ifndef NDN_SECURITY_SIGNING_INFO_HPP
#define NDN_SECURITY_SIGNING_INFO_HPP

#include "../name.hpp"
#include "../signature-info.hpp"
#include "security-common.hpp"


namespace ndn {
namespace security {

/**
 * @brief Signing parameters passed to KeyChain
 */
class SigningInfo
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  enum SignerType {
    /// @brief no signer is specified, use default setting or follow the trust schema
    SIGNER_TYPE_NULL = 0,
    /// @brief signer is an identity, use its default key and default certificate
    SIGNER_TYPE_ID = 1,
    /// @brief signer is a key, use its default certificate
    SIGNER_TYPE_KEY = 2,
    /// @brief signer is a certificate, use it directly
    SIGNER_TYPE_CERT = 3,
    /// @brief use sha256 digest, no signer needs to be specified
    SIGNER_TYPE_SHA256 = 4
  };

public:
  /**
   * @brief Constructor
   *
   * @param signerType The type of signer
   * @param signerName The name of signer; interpretation differs per signerType
   * @param signatureInfo A semi-prepared SignatureInfo which contains other information except
   *                      SignatureType and KeyLocator.  If SignatureType and KeyLocator are
   *                      specified, they may be overwritten by KeyChain.
   */
  explicit
  SigningInfo(SignerType signerType = SIGNER_TYPE_NULL,
              const Name& signerName = EMPTY_NAME,
              const SignatureInfo& signatureInfo = EMPTY_SIGNATURE_INFO);

  /**
   * @brief Set signer as an identity with name @p identity
   * @post Change the signerType to SIGNER_TYPE_ID
   */
  void
  setSigningIdentity(const Name& identity);

  /**
   * @brief Set signer as a key with name @p keyName
   * @post Change the signerType to SIGNER_TYPE_KEY
   */
  void
  setSigningKeyName(const Name& keyName);

  /**
   * @brief Set signer as a certificate with name @p certificateName
   * @post Change the signerType to SIGNER_TYPE_CERT
   */
  void
  setSigningCertName(const Name& certificateName);

  /**
   * @brief Set Sha256 as the signing method
   * @post Reset signerName, also change the signerType to SIGNER_TYPE_SHA256
   */
  void
  setSha256Signing();

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
   * @brief Set the digest algorithm for public key operations
   */
  void
  setDigestAlgorithm(const DigestAlgorithm& algorithm)
  {
    m_digestAlgorithm = algorithm;
  }

  /**
   * @return The digest algorithm for public key operations
   */
  DigestAlgorithm
  getDigestAlgorithm() const
  {
    return m_digestAlgorithm;
  }

  /**
   * @brief Set a semi-prepared SignatureInfo;
   */
  void
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
  static const Name EMPTY_NAME;
  static const SignatureInfo EMPTY_SIGNATURE_INFO;

private:
  SignerType m_type;
  Name m_name;

  DigestAlgorithm m_digestAlgorithm;

  SignatureInfo m_info;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_SIGNING_INFO_HPP
