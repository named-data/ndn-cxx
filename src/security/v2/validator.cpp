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

Validator::Validator(unique_ptr<ValidationPolicy> policy, Face* face)
  : m_policy(std::move(policy))
  , m_face(face)
  , m_verifiedCertificateCache(time::hours(1))
  , m_unverifiedCertificateCache(time::minutes(5))
  , m_maxDepth(25)
{
}

Validator::~Validator() = default;

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

const Certificate*
Validator::findTrustedCert(const Interest& interestForCertificate, const shared_ptr<ValidationState>& state)
{
  auto anchor = m_trustAnchors.find(interestForCertificate);
  if (anchor != nullptr) {
    NDN_LOG_TRACE_DEPTH("Found certificate in anchor cache " << anchor->getName());
    return anchor;
  }

  auto key = m_verifiedCertificateCache.find(interestForCertificate);
  if (key != nullptr) {
    NDN_LOG_TRACE_DEPTH("Found certificate in verified key cache " << key->getName());
    return key;
  }
  return nullptr;
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

  NDN_LOG_DEBUG_DEPTH("Retrieving " << certRequest->m_interest.getName());

  // Check the trusted cache
  auto cert = findTrustedCert(certRequest->m_interest, state);
  if (cert != nullptr) {
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

  if (state->hasSeenCertificateName(certRequest->m_interest.getName())) {
    state->fail({ValidationError::Code::LOOP_DETECTED,
                 "Loop detected at " + certRequest->m_interest.getName().toUri()});
    return;
  }

  // Check untrusted cache
  cert = m_unverifiedCertificateCache.find(certRequest->m_interest);
  if (cert != nullptr) {
    NDN_LOG_DEBUG_DEPTH("Found certificate in **un**verified key cache " << cert->getName());
    return dataCallback(*cert, certRequest, state, false); // to avoid caching the cached key
  }

  // Attempt to retrieve certificate from the network
  fetchCertificateFromNetwork(certRequest, state);
}

void
Validator::fetchCertificateFromNetwork(const shared_ptr<CertificateRequest>& certRequest,
                                       const shared_ptr<ValidationState>& state)
{
  if (m_face == nullptr) {
    state->fail({ValidationError::Code::CANNOT_RETRIEVE_CERT, "Cannot fetch certificate in offline mode "
                 "`" + certRequest->m_interest.getName().toUri() + "`"});
    return;
  }

  m_face->expressInterest(certRequest->m_interest,
                          [=] (const Interest& interest, const Data& data) {
                            dataCallback(data, certRequest, state);
                          },
                          [=] (const Interest& interest, const lp::Nack& nack) {
                            nackCallback(nack, certRequest, state);
                          },
                          [=] (const Interest& interest) {
                            timeoutCallback(certRequest, state);
                          });
}

void
Validator::dataCallback(const Data& data,
                        const shared_ptr<CertificateRequest>& certRequest,
                        const shared_ptr<ValidationState>& state,
                        bool isFromNetwork)
{
  NDN_LOG_DEBUG_DEPTH("Retrieved certificate " << (isFromNetwork ? "from network " : "from cache ") << data.getName());

  Certificate cert;
  try {
    cert = Certificate(data);
  }
  catch (const tlv::Error& e) {
    return state->fail({ValidationError::Code::MALFORMED_CERT, "Retrieved a malformed certificate "
                        "`" + data.getName().toUri() + "` (" + e.what() + ")"});
  }

  if (!cert.isValid()) {
    return state->fail({ValidationError::Code::EXPIRED_CERT, "Retrieved certificate is not yet "
                        "valid or has expired `" + cert.getName().toUri() + "`"});
  }
  if (isFromNetwork) {
    cacheUnverifiedCertificate(Certificate(cert));
  }
  return validate(cert, state); // recursion step
}

void
Validator::nackCallback(const lp::Nack& nack, const shared_ptr<CertificateRequest>& certRequest,
                        const shared_ptr<ValidationState>& state)
{
  NDN_LOG_DEBUG_DEPTH("NACK (" << nack.getReason() <<  ") while retrieving certificate "
                      << certRequest->m_interest.getName());

  --certRequest->m_nRetriesLeft;
  if (certRequest->m_nRetriesLeft > 0) {
    // TODO implement delay for the the next fetch
    fetchCertificateFromNetwork(certRequest, state);
  }
  else {
    state->fail({ValidationError::Code::CANNOT_RETRIEVE_CERT, "Cannot fetch certificate after all "
                 "retries `" + certRequest->m_interest.getName().toUri() + "`"});
  }
}

void
Validator::timeoutCallback(const shared_ptr<CertificateRequest>& certRequest,
                           const shared_ptr<ValidationState>& state)
{
  NDN_LOG_DEBUG_DEPTH("Timeout while retrieving certificate "
                      << certRequest->m_interest.getName() << ", retrying");

  --certRequest->m_nRetriesLeft;
  if (certRequest->m_nRetriesLeft > 0) {
    fetchCertificateFromNetwork(certRequest, state);
  }
  else {
    state->fail({ValidationError::Code::CANNOT_RETRIEVE_CERT, "Cannot fetch certificate after all "
                 "retries `" + certRequest->m_interest.getName().toUri() + "`"});
  }
}

////////////////////////////////////////////////////////////////////////
// Trust anchor management
////////////////////////////////////////////////////////////////////////

void
Validator::loadAnchor(const std::string& groupId, Certificate&& cert)
{
  m_trustAnchors.insert(groupId, std::move(cert));
}

void
Validator::loadAnchor(const std::string& groupId, const std::string& certfilePath,
                      time::nanoseconds refreshPeriod, bool isDir)
{
  m_trustAnchors.insert(groupId, certfilePath, refreshPeriod, isDir);
}

void
Validator::cacheVerifiedCertificate(Certificate&& cert)
{
  m_verifiedCertificateCache.insert(std::move(cert));
}

void
Validator::cacheUnverifiedCertificate(Certificate&& cert)
{
  m_unverifiedCertificateCache.insert(std::move(cert));
}

const TrustAnchorContainer&
Validator::getTrustAnchors() const
{
  return m_trustAnchors;
}

const CertificateCache&
Validator::getVerifiedCertificateCache() const
{
  return m_verifiedCertificateCache;
}

const CertificateCache&
Validator::getUnverifiedCertificateCache() const
{
  return m_unverifiedCertificateCache;
}

bool
Validator::isCertificateCached(const Name& certName) const
{
  return (getVerifiedCertificateCache().find(certName) != nullptr ||
          getVerifiedCertificateCache().find(certName) != nullptr);
}

} // namespace v2
} // namespace security
} // namespace ndn
