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

#include "ndn-cxx/security/v2/certificate-fetcher-direct-fetch.hpp"
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/lp/tags.hpp"

namespace ndn {
namespace security {
namespace v2 {

CertificateFetcherDirectFetch::CertificateFetcherDirectFetch(Face& face)
  : CertificateFetcherFromNetwork(face)
{
}

void
CertificateFetcherDirectFetch::setSendDirectInterestOnly(bool wantDirectInterestOnly)
{
  m_wantDirectInterestOnly = wantDirectInterestOnly;
}

void
CertificateFetcherDirectFetch::doFetch(const shared_ptr<CertificateRequest>& keyRequest,
                                       const shared_ptr<ValidationState>& state,
                                       const ValidationContinuation& continueValidation)
{
  auto interestState = dynamic_pointer_cast<InterestValidationState>(state);
  uint64_t incomingFaceId = 0;
  if (interestState != nullptr) {
    auto incomingFaceIdTag = interestState->getOriginalInterest().getTag<lp::IncomingFaceIdTag>();
    if (incomingFaceIdTag != nullptr) {
      incomingFaceId = incomingFaceIdTag->get();
    }
  }
  else {
    auto dataState = dynamic_pointer_cast<DataValidationState>(state);
    auto incomingFaceIdTag = dataState->getOriginalData().getTag<lp::IncomingFaceIdTag>();
    if (incomingFaceIdTag != nullptr) {
      incomingFaceId = incomingFaceIdTag->get();
    }
  }
  if (incomingFaceId != 0) {
    Interest directInterest(keyRequest->interest);
    directInterest.refreshNonce();
    directInterest.setTag(make_shared<lp::NextHopFaceIdTag>(incomingFaceId));

    if (!m_wantDirectInterestOnly) {
      // disable callbacks
      m_face.expressInterest(directInterest, nullptr, nullptr, nullptr);
    }
    else {
      m_face.expressInterest(directInterest,
                             [=] (const Interest& interest, const Data& data) {
                               dataCallback(data, keyRequest, state, continueValidation);
                             },
                             [=] (const Interest& interest, const lp::Nack& nack) {
                               nackCallback(nack, keyRequest, state, continueValidation);
                             },
                             [=] (const Interest& interest) {
                               timeoutCallback(keyRequest, state, continueValidation);
                             });
    }
  }

  if (!m_wantDirectInterestOnly) {
    // send infrastructure Interest
    CertificateFetcherFromNetwork::doFetch(keyRequest, state, continueValidation);
  }
  else if (incomingFaceId == 0) {
    state->fail({ValidationError::Code::CANNOT_RETRIEVE_CERT,
          "Cannot direct fetch certificate as IncomingFaceId tag is not set"});
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
