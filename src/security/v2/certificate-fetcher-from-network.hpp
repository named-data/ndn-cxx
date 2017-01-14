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

#ifndef NDN_SECURITY_V2_CERTIFICATE_FETCHER_FROM_NETWORK_HPP
#define NDN_SECURITY_V2_CERTIFICATE_FETCHER_FROM_NETWORK_HPP

#include "certificate-fetcher.hpp"

namespace ndn {

namespace lp {
class Nack;
} // namespace lp

namespace security {
namespace v2 {

/**
 * @brief Fetch missing keys from the network
 */
class CertificateFetcherFromNetwork : public CertificateFetcher
{
public:
  explicit
  CertificateFetcherFromNetwork(Face& face);

protected:
  void
  doFetch(const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
          const ValidationContinuation& continueValidation) override;

private:
  /**
   * @brief Callback invoked when certificate is retrieved.
   */
  void
  dataCallback(const Data& data,
               const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
               const ValidationContinuation& continueValidation);

  /**
   * @brief Callback invoked when interest for fetching certificate gets NACKed.
   *
   * It will retry if certRequest->m_nRetriesLeft > 0
   *
   * @todo Delay retry for some amount of time
   */
  void
  nackCallback(const lp::Nack& nack,
               const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
               const ValidationContinuation& continueValidation);

  /**
   * @brief Callback invoked when interest for fetching certificate times out.
   *
   * It will retry if certRequest->m_nRetriesLeft > 0
   */
  void
  timeoutCallback(const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
                  const ValidationContinuation& continueValidation);

protected:
  Face& m_face;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_FETCHER_FROM_NETWORK_HPP
