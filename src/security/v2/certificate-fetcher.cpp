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

#include "certificate-fetcher.hpp"
#include "util/logger.hpp"

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.CertificateFetcher);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(state->getDepth() + 1, '>') << " " << x)

CertificateFetcher::CertificateFetcher()
  : m_certStorage(nullptr)
{
}

CertificateFetcher::~CertificateFetcher() = default;

void
CertificateFetcher::setCertificateStorage(CertificateStorage& certStorage)
{
  m_certStorage = &certStorage;
}

void
CertificateFetcher::fetch(const shared_ptr<CertificateRequest>& certRequest,
                          const shared_ptr<ValidationState>& state,
                          const ValidationContinuation& continueValidation)
{
  BOOST_ASSERT(m_certStorage != nullptr);
  auto cert = m_certStorage->getUnverifiedCertCache().find(certRequest->m_interest);
  if (cert != nullptr) {
    NDN_LOG_DEBUG_DEPTH("Found certificate in **un**verified key cache " << cert->getName());
    continueValidation(*cert, state);
    return;
  }
  doFetch(certRequest, state,
          [continueValidation, this] (const Certificate& cert, const shared_ptr<ValidationState>& state) {
            m_certStorage->cacheUnverifiedCert(Certificate(cert));
            continueValidation(cert, state);
          });
}

} // namespace v2
} // namespace security
} // namespace ndn
