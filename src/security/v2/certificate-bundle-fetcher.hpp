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
 */

#ifndef NDN_SECURITY_V2_CERTIFICATE_BUNDLE_FETCHER_HPP
#define NDN_SECURITY_V2_CERTIFICATE_BUNDLE_FETCHER_HPP

#include "certificate-fetcher-from-network.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief Fetch certificate bundle from the network
 *
 * Currently bundle fetching is attempted only for Data validation. This may change in the
 * future. Bundle fetching always goes to the infrastructure regardless of the inner
 * fetcher. Inner fetcher is used when the bundle interest times out or returns a Nack or when
 * additional certificates are needed for validation.
 *
 * @sa https://redmine.named-data.net/projects/ndn-cxx/wiki/Certificate_Bundle_Packet_Format
 */
class CertificateBundleFetcher : public CertificateFetcher
{
public:
  explicit
  CertificateBundleFetcher(unique_ptr<CertificateFetcher> inner,
                           Face& face);

  /**
   * @brief Set the lifetime of certificate bundle interest
   */
  void
  setBundleInterestLifetime(time::milliseconds time);

  /**
   * @return The lifetime of certificate bundle interest
   */
  time::milliseconds
  getBundleInterestLifetime() const;

  /**
   * Set the storage for this and inner certificate fetcher
   */
  void
  setCertificateStorage(CertificateStorage& certStorage) override;

protected:
  void
  doFetch(const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
          const ValidationContinuation& continueValidation) override;

private:
  /**
   * @brief Fetch the first bundle segment.
   *
   * After deriving the bundle name prefix, the exact version of the bundle is not yet known.
   * This method express Interest for the bundle prefix to (1) retrieve first segment of the bundle and
   * (2) discover bundle version. The bundle version will be recorded in the validation state as BundleNameTag
   * and will be used in subsequent @p fetchNextBundleSegment calls to fetch further bundle segments if needed.
   */
  void
  fetchFirstBundleSegment(const Name& bundleNamePrefix,
                          const shared_ptr<CertificateRequest>& certRequest,
                          const shared_ptr<ValidationState>& state,
                          const ValidationContinuation& continueValidation);

  /**
   * @brief Fetch the specified bundle segment.
   */
  void
  fetchNextBundleSegment(const Name& fullBundleName, const name::Component& segmentNo,
                         const shared_ptr<CertificateRequest>& certRequest,
                         const shared_ptr<ValidationState>& state,
                         const ValidationContinuation& continueValidation);

  /**
   * @brief Derive bundle name from data name.
   *
   * Current naming conventions are as follows:
   * /<derived(data_name)>/BUNDLE/<trust-model>/<version>/<seg>
   *
   * Current rules for derived(data_name):
   * (1) If the last component is Implicit Digest AND the second last component is Segment number
   * then derived(data_name) = data_name.getPrefix(-2)
   * (2) If the last component is Implicit Digest
   * then derived(data_name) = data_name.getPrefix(-1)
   * (3) If the last component is Segment number
   * then derived(data_name) = data_name.getPrefix(-1)
   *
   * <trust-model> component is "00" for single hierarchy trust models.
   */
  static Name
  deriveBundleName(const Name& name);

  /**
   * @brief Callback invoked when certificate bundle is retrieved.
   */
  void
  dataCallback(const Data& data, bool isSegmentZeroExpected,
               const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
               const ValidationContinuation& continueValidation);

  /**
   * @brief Callback invoked when interest for fetching certificate bundle gets NACKed.
   */
  void
  nackCallback(const lp::Nack& nack,
               const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
               const ValidationContinuation& continueValidation, const Name& bundleName);

  /**
   * @brief Callback invoked when interest for fetching certificate times out.
   */
  void
  timeoutCallback(const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
                  const ValidationContinuation& continueValidation, const Name& bundleName);

private:
  unique_ptr<CertificateFetcher> m_inner;
  Face& m_face;
  using BundleNameTag = SimpleTag<Name, 1000>;
  using FinalBlockIdTag = SimpleTag<name::Component, 1001>;
  time::milliseconds m_bundleInterestLifetime;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_BUNDLE_FETCHER_HPP
