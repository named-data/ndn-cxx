/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/security/validator.hpp"
#include "ndn-cxx/util/logger.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn::security {

NDN_LOG_INIT(ndn.security.Validator);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(state->getDepth() + 1, '>') << " " << x)
#define NDN_LOG_TRACE_DEPTH(x) NDN_LOG_TRACE(std::string(state->getDepth() + 1, '>') << " " << x)

Validator::Validator(unique_ptr<ValidationPolicy> policy, unique_ptr<CertificateFetcher> certFetcher)
  : m_policy(std::move(policy))
  , m_certFetcher(std::move(certFetcher))
{
  BOOST_ASSERT(m_policy != nullptr);
  BOOST_ASSERT(m_certFetcher != nullptr);
  m_policy->setValidator(*this);
  m_certFetcher->setCertificateStorage(*this);
}

Validator::~Validator() noexcept = default;

void
Validator::validate(const Data& data,
                    const DataValidationSuccessCallback& successCb,
                    const DataValidationFailureCallback& failureCb)
{
  auto state = make_shared<DataValidationState>(data, successCb, failureCb);
  NDN_LOG_DEBUG_DEPTH("Start validating data " << data.getName());

  m_policy->checkPolicy(data, state, [this] (auto&&... args) {
    continueValidation(std::forward<decltype(args)>(args)...);
  });
}

void
Validator::validate(const Interest& interest,
                    const InterestValidationSuccessCallback& successCb,
                    const InterestValidationFailureCallback& failureCb)
{
  auto state = make_shared<InterestValidationState>(interest, successCb, failureCb);
  NDN_LOG_DEBUG_DEPTH("Start validating interest " << interest.getName());

  try {
    auto fmt = interest.getSignatureInfo() ? SignedInterestFormat::V03 : SignedInterestFormat::V02;
    state->setTag(make_shared<SignedInterestFormatTag>(fmt));
  }
  catch (const tlv::Error& e) {
    return state->fail({ValidationError::MALFORMED_SIGNATURE, "Malformed InterestSignatureInfo in `" +
                        interest.getName().toUri() + "`: " + e.what()});
  }

  m_policy->checkPolicy(interest, state, [this] (auto&&... args) {
    continueValidation(std::forward<decltype(args)>(args)...);
  });
}

void
Validator::validate(const Certificate& cert, const shared_ptr<ValidationState>& state)
{
  NDN_LOG_DEBUG_DEPTH("Start validating certificate " << cert.getName());

  if (!cert.isValid()) {
    return state->fail({ValidationError::EXPIRED_CERT, "`" + cert.getName().toUri() + "` is valid "
                        "between " + boost::lexical_cast<std::string>(cert.getValidityPeriod())});
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
Validator::continueValidation(const shared_ptr<CertificateRequest>& certRequest,
                              const shared_ptr<ValidationState>& state)
{
  BOOST_ASSERT(state);

  if (certRequest == nullptr) {
    state->bypassValidation();
  }
  else {
    // need to fetch key and validate it
    requestCertificate(certRequest, state);
  }
}

void
Validator::requestCertificate(const shared_ptr<CertificateRequest>& certRequest,
                              const shared_ptr<ValidationState>& state)
{
  if (state->getDepth() >= m_maxDepth) {
    state->fail({ValidationError::EXCEEDED_DEPTH_LIMIT, to_string(m_maxDepth)});
    return;
  }

  if (certRequest->interest.getName() == SigningInfo::getDigestSha256Identity()) {
    state->verifyOriginalPacket(std::nullopt);
    return;
  }

  if (state->hasSeenCertificateName(certRequest->interest.getName())) {
    state->fail({ValidationError::LOOP_DETECTED, certRequest->interest.getName().toUri()});
    return;
  }

  NDN_LOG_DEBUG_DEPTH("Retrieving " << certRequest->interest.getName());

  auto cert = findTrustedCert(certRequest->interest);
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

  m_certFetcher->fetch(certRequest, state, [this] (auto&&... args) {
    validate(std::forward<decltype(args)>(args)...);
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

} // namespace ndn::security
