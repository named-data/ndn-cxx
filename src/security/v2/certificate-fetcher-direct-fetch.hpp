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

#ifndef NDN_SECURITY_V2_CERTIFICATE_FETCHER_DIRECT_FETCH_HPP
#define NDN_SECURITY_V2_CERTIFICATE_FETCHER_DIRECT_FETCH_HPP

#include "certificate-fetcher-from-network.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief Extends CertificateFetcherFromNetwork to fetch certificates from the incoming face of
 *   the packet
 *
 * During Interest and Data validation, if IncomingFaceId tag is present on the original Interest,
 * this fetcher will send a "direct Interest" to fetch certificates from the face where the original
 * packet was received, in addition to fetching from the infrastructure. The application must
 * enable NextHopFaceId privilege on the face used by this fetcher prior to the validation.
 */
class CertificateFetcherDirectFetch : public CertificateFetcherFromNetwork
{
public:
  explicit
  CertificateFetcherDirectFetch(Face& face);

protected:
  void
  doFetch(const shared_ptr<CertificateRequest>& keyRequest, const shared_ptr<ValidationState>& state,
          const ValidationContinuation& continueValidation) override;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_FETCHER_DIRECT_FETCH_HPP
