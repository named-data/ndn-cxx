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

#include "validator.hpp"

#include "face.hpp"
#include "security/transform/public-key.hpp"
#include "util/logger.hpp"

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.Validator);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(state->getDepth() + 1, '>') << " " << x)
#define NDN_LOG_TRACE_DEPTH(x) NDN_LOG_TRACE(std::string(state->getDepth() + 1, '>') << " " << x)

Validator::Validator(unique_ptr<ValidationPolicy> policy, unique_ptr<CertificateFetcher> certFetcher)
  : m_policy(std::move(policy))
  , m_certFetcher(std::move(certFetcher))
  , m_maxDepth(25)
{
  BOOST_ASSERT(m_policy != nullptr);
  BOOST_ASSERT(m_certFetcher != nullptr);
  m_policy->setValidator(*this);
  m_certFetcher->setCertificateStorage(*this);
}

Validator::~Validator() = default;

ValidationPolicy&
Validator::getPolicy()
{
  return *m_policy;
}

CertificateFetcher&
Validator::getFetcher()
{
  return *m_certFetcher;
}

void
Validator::setMaxDepth(size_t depth)
{
  m_maxDepth = depth;
}

size_t
Validator::getMaxDepth() const
{
  return m_maxDepth;
}

void
Validator::validate(const Data& data,
                    const DataValidationSuccessCallback& successCb,
                    const DataValidationFailureCallback& failureCb)
{
  auto state = make_shared<DataValidationState>(data, successCb, failureCb);
  NDN_LOG_DEBUG_DEPTH("Start validating data " << data.getName());

  m_policy->checkPolicy(data, state,
      [this] (const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state) {
      if (certRequest == nullptr) {
        state->bypassValidation();
      }
      else {
        // need to fetch key and validate it
        requestCertificate(certRequest, state);
      }
    });
}

void
Validator::validate(const Interest& interest,
                    const InterestValidationSuccessCallback& successCb,
                    const InterestValidationFailureCallback& failureCb)
{
  auto state = make_shared<InterestValidationState>(interest, successCb, failureCb);
  NDN_LOG_DEBUG_DEPTH("Start validating interest " << interest.getName());

  m_policy->checkPolicy(interest, state,
      [this] (const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state) {
      if (certRequest == nullptr) {
        state->bypassValidation();
      }
      else {
        // need to fetch key and validate it
        requestCertificate(certRequest, state);
      }
    });
}

void
Validator::validate(const Certificate& cert, const shared_ptr<ValidationState>& state)
{
  NDN_LOG_DEBUG_DEPTH("Start validating certificate " << cert.getName());

  if (!cert.isValid()) {
    return state->fail({ValidationError::Code::EXPIRED_CERT, "Retrieved certificate is not yet valid or expired "
          "`" + cert.getName().toUri() + "`"});
  }

  m_policy->checkPolicy(cert, state,
      [this, cert] (const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state) {
      if (certRequest == nullptr) {
        state->fail({ValidationError::POLICY_ERROR, "Validation policy is not allowed to designate `" +
                     cert.getName().toUri() + "` as a trust anchor"});
      }
      else {
        // need to fetch key and validate it
        state->addCertificate(cert);
        requestCertificate(certRequest, state);
      }
    });
}

void
Validator::requestCertificate(const shared_ptr<CertificateRequest>& certRequest,
                              const shared_ptr<ValidationState>& state)
{
  // TODO configurable check for the maximum number of steps
  if (state->getDepth() >= m_maxDepth) {
    state->fail({ValidationError::Code::EXCEEDED_DEPTH_LIMIT,
                 "Exceeded validation depth limit (" + to_string(m_maxDepth) + ")"});
    return;
  }

  if (state->hasSeenCertificateName(certRequest->m_interest.getName())) {
    state->fail({ValidationError::Code::LOOP_DETECTED,
                 "Validation loop detected for certificate `" + certRequest->m_interest.getName().toUri() + "`"});
    return;
  }

  NDN_LOG_DEBUG_DEPTH("Retrieving " << certRequest->m_interest.getName());

  auto cert = findTrustedCert(certRequest->m_interest);
  if (cert != nullptr) {
    NDN_LOG_TRACE_DEPTH("Found trusted certificate " << cert->getName());

    cert = state->verifyCertificateChain(*cert);
    if (cert != nullptr) {
      state->verifyOriginalPacket(*cert);
    }
    for (auto trustedCert = std::make_move_iterator(state->m_certificateChain.begin());
         trustedCert != std::make_move_iterator(state->m_certificateChain.end());
         ++trustedCert) {
      cacheVerifiedCertificate(*trustedCert);
    }
    return;
  }

  m_certFetcher->fetch(certRequest, state, [this] (const Certificate& cert, const shared_ptr<ValidationState>& state) {
      validate(cert, state);
    });
}

////////////////////////////////////////////////////////////////////////
// Trust anchor management
////////////////////////////////////////////////////////////////////////

// to change visibility from protected to public

void
Validator::loadAnchor(const std::string& groupId, Certificate&& cert)
{
  CertificateStorage::loadAnchor(groupId, std::move(cert));
}

void
Validator::loadAnchor(const std::string& groupId, const std::string& certfilePath,
                      time::nanoseconds refreshPeriod, bool isDir)
{
  CertificateStorage::loadAnchor(groupId, certfilePath, refreshPeriod, isDir);
}

void
Validator::resetAnchors()
{
  CertificateStorage::resetAnchors();
}

void
Validator::cacheVerifiedCertificate(Certificate&& cert)
{
  CertificateStorage::cacheVerifiedCert(std::move(cert));
}

void
Validator::resetVerifiedCertificates()
{
  CertificateStorage::resetVerifiedCerts();
}

} // namespace v2
} // namespace security
} // namespace ndn
