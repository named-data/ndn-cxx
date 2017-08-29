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

#ifndef NDN_SECURITY_V2_CERTIFICATE_STORAGE_HPP
#define NDN_SECURITY_V2_CERTIFICATE_STORAGE_HPP

#include "certificate.hpp"
#include "certificate-cache.hpp"
#include "trust-anchor-container.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief Storage for trusted anchors, verified certificate cache, and unverified certificate cache.
 */
class CertificateStorage : noncopyable
{
public:
  CertificateStorage();

  /**
   * @brief Find a trusted certificate in trust anchor container or in verified cache
   * @param interestForCert Interest for certificate
   * @return found certificate, nullptr if not found.
   *
   * @note The returned pointer may get invalidated after next findTrustedCert or findCert calls.
   */
  const Certificate*
  findTrustedCert(const Interest& interestForCert) const;

  /**
   * @brief Check if certificate exists in verified, unverified cache, or in the set of trust
   *        anchors
   */
  bool
  isCertKnown(const Name& certPrefix) const;

  /**
   * @brief Cache unverified certificate for a period of time (5 minutes)
   * @param cert  The certificate packet
   *
   * @todo Add ability to customize time period
   */
  void
  cacheUnverifiedCert(Certificate&& cert);

  /**
   * @return Trust anchor container
   */
  const TrustAnchorContainer&
  getTrustAnchors() const;

  /**
   * @return Verified certificate cache
   */
  const CertificateCache&
  getVerifiedCertCache() const;

  /**
   * @return Unverified certificate cache
   */
  const CertificateCache&
  getUnverifiedCertCache() const;

protected:
  /**
   * @brief load static trust anchor.
   *
   * Static trust anchors are permanently associated with the validator and never expire.
   *
   * @param groupId  Certificate group id.
   * @param cert     Certificate to load as a trust anchor.
   */
  void
  loadAnchor(const std::string& groupId, Certificate&& cert);

  /**
   * @brief load dynamic trust anchors.
   *
   * Dynamic trust anchors are associated with the validator for as long as the underlying
   * trust anchor file (set of files) exist(s).
   *
   * @param groupId          Certificate group id, must not be empty.
   * @param certfilePath     Specifies the path to load the trust anchors.
   * @param refreshPeriod    Refresh period for the trust anchors, must be positive.
   * @param isDir            Tells whether the path is a directory or a single file.
   */
  void
  loadAnchor(const std::string& groupId, const std::string& certfilePath,
             time::nanoseconds refreshPeriod, bool isDir = false);

  /**
   * @brief remove any previously loaded static or dynamic trust anchor
   */
  void
  resetAnchors();

  /**
   * @brief Cache verified certificate a period of time (1 hour)
   * @param cert  The certificate packet
   *
   * @todo Add ability to customize time period
   */
  void
  cacheVerifiedCert(Certificate&& cert);

  /**
   * @brief Remove any cached verified certificates
   */
  void
  resetVerifiedCerts();

protected:
  TrustAnchorContainer m_trustAnchors;
  CertificateCache m_verifiedCertCache;
  CertificateCache m_unverifiedCertCache;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_STORAGE_HPP
