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

#ifndef NDN_SECURITY_V2_VALIDATOR_HPP
#define NDN_SECURITY_V2_VALIDATOR_HPP

#include "certificate-fetcher.hpp"
#include "certificate-request.hpp"
#include "certificate-storage.hpp"
#include "validation-callback.hpp"
#include "validation-policy.hpp"
#include "validation-state.hpp"

namespace ndn {

class Face;

namespace security {
namespace v2 {

/**
 * @brief Interface for validating data and interest packets.
 *
 * Every time a validation process initiated, it creates a ValidationState that exist until
 * validation finishes with either success or failure.  This state serves several purposes:
 * - record Interest or Data packet being validated
 * - record failure callback
 * - record certificates in the certification chain for the Interest or Data packet being validated
 * - record names of the requested certificates to detect loops in the certificate chain
 * - keep track of the validation chain size (aka validation "depth")
 *
 * During validation, policy and/or key fetcher can augment validation state with policy- and
 * fetcher-specific information using ndn::Tag's.
 *
 * A validator has a trust anchor cache to save static and dynamic trust anchors, a verified
 * certificate cache for saving certificates that are already verified and an unverified
 * certificate cache for saving prefetched but not yet verified certificates.
 *
 * @todo Limit the maximum time the validation process is allowed to run before declaring failure
 * @todo Ability to customize maximum lifetime for trusted and untrusted certificate caches.
 *       Current implementation hard-codes them to be 1 hour and 5 minutes.
 */
class Validator : public CertificateStorage
{
public:
  /**
   * @brief Validator constructor.
   *
   * @param policy      Validation policy to be associated with the validator
   * @param certFetcher Certificate fetcher implementation.
   */
  Validator(unique_ptr<ValidationPolicy> policy, unique_ptr<CertificateFetcher> certFetcher);

  ~Validator();

  ValidationPolicy&
  getPolicy();

  CertificateFetcher&
  getFetcher();

  /**
   * @brief Set the maximum depth of the certificate chain
   */
  void
  setMaxDepth(size_t depth);

  /**
   * @return The maximum depth of the certificate chain
   */
  size_t
  getMaxDepth() const;

  /**
   * @brief Asynchronously validate @p data
   *
   * @note @p successCb and @p failureCb must not be nullptr
   */
  void
  validate(const Data& data,
           const DataValidationSuccessCallback& successCb,
           const DataValidationFailureCallback& failureCb);

  /**
   * @brief Asynchronously validate @p interest
   *
   * @note @p successCb and @p failureCb must not be nullptr
   */
  void
  validate(const Interest& interest,
           const InterestValidationSuccessCallback& successCb,
           const InterestValidationFailureCallback& failureCb);

public: // anchor management
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
   * @brief Cache verified @p cert a period of time (1 hour)
   *
   * @todo Add ability to customize time period
   */
  void
  cacheVerifiedCertificate(Certificate&& cert);

  /**
   * @brief Remove any cached verified certificates
   */
  void
  resetVerifiedCertificates();

private: // Common validator operations
  /**
   * @brief Recursive validation of the certificate in the certification chain
   *
   * @param cert   The certificate to check.
   * @param state  The current validation state.
   */
  void
  validate(const Certificate& cert, const shared_ptr<ValidationState>& state);

  /**
   * @brief Request certificate for further validation.
   *
   * @param certRequest  Certificate request.
   * @param state        The current validation state.
   */
  void
  requestCertificate(const shared_ptr<CertificateRequest>& certRequest,
                     const shared_ptr<ValidationState>& state);

private:
  unique_ptr<ValidationPolicy> m_policy;
  unique_ptr<CertificateFetcher> m_certFetcher;
  size_t m_maxDepth;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATOR_HPP
