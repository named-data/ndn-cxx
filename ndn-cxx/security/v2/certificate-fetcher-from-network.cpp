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

#include "ndn-cxx/security/v2/certificate-fetcher-from-network.hpp"
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.CertificateFetcher);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(state->getDepth() + 1, '>') << " " << x)
#define NDN_LOG_TRACE_DEPTH(x) NDN_LOG_TRACE(std::string(state->getDepth() + 1, '>') << " " << x)

CertificateFetcherFromNetwork::CertificateFetcherFromNetwork(Face& face)
  : m_face(face)
  , m_scheduler(face.getIoService())
{
}

void
CertificateFetcherFromNetwork::doFetch(const shared_ptr<CertificateRequest>& certRequest,
                                       const shared_ptr<ValidationState>& state,
                                       const ValidationContinuation& continueValidation)
{
  m_face.expressInterest(certRequest->interest,
                         [=] (const Interest& interest, const Data& data) {
                           dataCallback(data, certRequest, state, continueValidation);
                         },
                         [=] (const Interest& interest, const lp::Nack& nack) {
                           nackCallback(nack, certRequest, state, continueValidation);
                         },
                         [=] (const Interest& interest) {
                           timeoutCallback(certRequest, state, continueValidation);
                         });
}

void
CertificateFetcherFromNetwork::dataCallback(const Data& data,
                                            const shared_ptr<CertificateRequest>& certRequest,
                                            const shared_ptr<ValidationState>& state,
                                            const ValidationContinuation& continueValidation)
{
  NDN_LOG_DEBUG_DEPTH("Fetched certificate from network " << data.getName());

  Certificate cert;
  try {
    cert = Certificate(data);
  }
  catch (const tlv::Error& e) {
    return state->fail({ValidationError::Code::MALFORMED_CERT, "Fetched a malformed certificate "
                        "`" + data.getName().toUri() + "` (" + e.what() + ")"});
  }
  continueValidation(cert, state);
}

void
CertificateFetcherFromNetwork::nackCallback(const lp::Nack& nack,
                                            const shared_ptr<CertificateRequest>& certRequest,
                                            const shared_ptr<ValidationState>& state,
                                            const ValidationContinuation& continueValidation)
{
  NDN_LOG_DEBUG_DEPTH("NACK (" << nack.getReason() <<  ") while fetching certificate "
                      << certRequest->interest.getName());

  --certRequest->nRetriesLeft;
  if (certRequest->nRetriesLeft >= 0) {
    m_scheduler.schedule(certRequest->waitAfterNack,
                         [=] { fetch(certRequest, state, continueValidation); });
    certRequest->waitAfterNack *= 2;
  }
  else {
    state->fail({ValidationError::Code::CANNOT_RETRIEVE_CERT, "Cannot fetch certificate after all "
                 "retries `" + certRequest->interest.getName().toUri() + "`"});
  }
}

void
CertificateFetcherFromNetwork::timeoutCallback(const shared_ptr<CertificateRequest>& certRequest,
                                               const shared_ptr<ValidationState>& state,
                                               const ValidationContinuation& continueValidation)
{
  NDN_LOG_DEBUG_DEPTH("Timeout while fetching certificate " << certRequest->interest.getName()
                      << ", retrying");

  --certRequest->nRetriesLeft;
  if (certRequest->nRetriesLeft >= 0) {
    fetch(certRequest, state, continueValidation);
  }
  else {
    state->fail({ValidationError::Code::CANNOT_RETRIEVE_CERT, "Cannot fetch certificate after all "
                 "retries `" + certRequest->interest.getName().toUri() + "`"});
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
