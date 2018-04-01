/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "certificate-bundle-fetcher.hpp"
#include "face.hpp"
#include "util/logger.hpp"

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.CertificateBundleFetcher);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(state->getDepth() + 1, '>') << " " << x)
#define NDN_LOG_TRACE_DEPTH(x) NDN_LOG_TRACE(std::string(state->getDepth() + 1, '>') << " " << x)

CertificateBundleFetcher::CertificateBundleFetcher(unique_ptr<CertificateFetcher> inner,
                                                   Face& face)
  : m_inner(std::move(inner))
  , m_face(face)
  , m_bundleInterestLifetime(1000)
{
  BOOST_ASSERT(m_inner != nullptr);
}

void
CertificateBundleFetcher::setBundleInterestLifetime(time::milliseconds time)
{
  m_bundleInterestLifetime = time;
}

time::milliseconds
CertificateBundleFetcher::getBundleInterestLifetime() const
{
  return m_bundleInterestLifetime;
}

void
CertificateBundleFetcher::setCertificateStorage(CertificateStorage& certStorage)
{
  m_certStorage = &certStorage;
  m_inner->setCertificateStorage(certStorage);
}

void
CertificateBundleFetcher::doFetch(const shared_ptr<CertificateRequest>& certRequest,
                                  const shared_ptr<ValidationState>& state,
                                  const ValidationContinuation& continueValidation)
{
  auto dataValidationState = dynamic_pointer_cast<DataValidationState>(state);
  if (dataValidationState == nullptr) {
    return m_inner->fetch(certRequest, state, continueValidation);
  }

  // check if a bundle segment was fetched before
  shared_ptr<BundleNameTag> bundleNameTag = state->getTag<BundleNameTag>();
  if (bundleNameTag == nullptr) {
    const Name& originalDataName = dataValidationState->getOriginalData().getName();
    if (originalDataName.empty()) {
      return m_inner->fetch(certRequest, state, continueValidation);
    }
    // derive certificate bundle name from original data name
    Name bundleNamePrefix = deriveBundleName(originalDataName);
    fetchFirstBundleSegment(bundleNamePrefix, certRequest, state, continueValidation);
  }
  else {
    Name fullBundleName = bundleNameTag->get();
    fetchNextBundleSegment(fullBundleName, fullBundleName.get(-1).getSuccessor(),
                           certRequest, state, continueValidation);
  }
}

void
CertificateBundleFetcher::fetchFirstBundleSegment(const Name& bundleNamePrefix,
                                                  const shared_ptr<CertificateRequest>& certRequest,
                                                  const shared_ptr<ValidationState>& state,
                                                  const ValidationContinuation& continueValidation)
{
  Interest bundleInterest = Interest(bundleNamePrefix);
  bundleInterest.setInterestLifetime(m_bundleInterestLifetime);
  bundleInterest.setMustBeFresh(true);
  bundleInterest.setChildSelector(1);

  m_face.expressInterest(bundleInterest,
                         [=] (const Interest& interest, const Data& data) {
                           dataCallback(data, true, certRequest, state, continueValidation);
                         },
                         [=] (const Interest& interest, const lp::Nack& nack) {
                           nackCallback(nack, certRequest, state, continueValidation, bundleNamePrefix);
                         },
                         [=] (const Interest& interest) {
                           timeoutCallback(certRequest, state, continueValidation, bundleNamePrefix);
                         });
}

void
CertificateBundleFetcher::fetchNextBundleSegment(const Name& fullBundleName, const name::Component& segmentNo,
                                                 const shared_ptr<CertificateRequest>& certRequest,
                                                 const shared_ptr<ValidationState>& state,
                                                 const ValidationContinuation& continueValidation)
{
  shared_ptr<FinalBlockIdTag> finalBlockId = state->getTag<FinalBlockIdTag>();
  if (finalBlockId != nullptr && segmentNo > finalBlockId->get()) {
    return m_inner->fetch(certRequest, state, continueValidation);
  }

  Interest bundleInterest(fullBundleName.getPrefix(-1).append(segmentNo));
  bundleInterest.setInterestLifetime(m_bundleInterestLifetime);
  bundleInterest.setMustBeFresh(false);

  m_face.expressInterest(bundleInterest,
                         [=] (const Interest& interest, const Data& data) {
                           dataCallback(data, false, certRequest, state, continueValidation);
                         },
                         [=] (const Interest& interest, const lp::Nack& nack) {
                           nackCallback(nack, certRequest, state, continueValidation, fullBundleName);
                         },
                         [=] (const Interest& interest) {
                           timeoutCallback(certRequest, state, continueValidation, fullBundleName);
                         });
}

void
CertificateBundleFetcher::dataCallback(const Data& bundleData,
                                       bool isSegmentZeroExpected,
                                       const shared_ptr<CertificateRequest>& certRequest,
                                       const shared_ptr<ValidationState>& state,
                                       const ValidationContinuation& continueValidation)
{
  NDN_LOG_DEBUG_DEPTH("Fetched certificate bundle from network " << bundleData.getName());

  name::Component currentSegment = bundleData.getName().get(-1);
  if (!currentSegment.isSegment()) {
    return m_inner->fetch(certRequest, state, continueValidation);
  }

  if (isSegmentZeroExpected && currentSegment.toSegment() != 0) {
    // fetch segment zero
    fetchNextBundleSegment(bundleData.getName(), name::Component::fromSegment(0),
                           certRequest, state, continueValidation);
  }
  else {
    state->setTag(make_shared<BundleNameTag>(bundleData.getName()));

    const auto& finalBlockId = bundleData.getFinalBlock();
    if (!finalBlockId) {
      state->setTag(make_shared<FinalBlockIdTag>(*finalBlockId));
    }

    Block bundleContent = bundleData.getContent();
    bundleContent.parse();

    // store all the certificates in unverified cache
    for (const auto& block : bundleContent.elements()) {
      m_certStorage->cacheUnverifiedCert(Certificate(block));
    }

    auto cert = m_certStorage->getUnverifiedCertCache().find(certRequest->m_interest);
    continueValidation(*cert, state);
  }
}

void
CertificateBundleFetcher::nackCallback(const lp::Nack& nack,
                                       const shared_ptr<CertificateRequest>& certRequest,
                                       const shared_ptr<ValidationState>& state,
                                       const ValidationContinuation& continueValidation,
                                       const Name& bundleName)
{
  NDN_LOG_DEBUG_DEPTH("NACK (" << nack.getReason() <<  ") while fetching certificate bundle"
                      << bundleName);

  m_inner->fetch(certRequest, state, continueValidation);
}

void
CertificateBundleFetcher::timeoutCallback(const shared_ptr<CertificateRequest>& certRequest,
                                          const shared_ptr<ValidationState>& state,
                                          const ValidationContinuation& continueValidation,
                                          const Name& bundleName)
{
  NDN_LOG_DEBUG_DEPTH("Timeout while fetching certificate bundle" << bundleName);

  m_inner->fetch(certRequest, state, continueValidation);
}

Name
CertificateBundleFetcher::deriveBundleName(const Name& name)
{
  name::Component lastComponent = name.at(-1);

  Name bundleName = name;
  if (lastComponent.isImplicitSha256Digest()) {
    if (name.size() >= 2 && name.get(-2).isSegment()) {
      bundleName = name.getPrefix(-2);
    }
    else {
      bundleName = name.getPrefix(-1);
    }
  }
  else if (lastComponent.isSegment()) {
    bundleName = name.getPrefix(-1);
  }
  bundleName.append("_BUNDLE");
  bundleName.appendNumber(00);

  return bundleName;
}

} // namespace v2
} // namespace security
} // namespace ndn
