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

#include "security/v2/certificate-bundle-fetcher.hpp"
#include "security/v2/validation-policy-simple-hierarchy.hpp"
#include "util/regex/regex-pattern-list-matcher.hpp"
#include "lp/nack.hpp"

#include "boost-test.hpp"
#include "validator-fixture.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(TestCertificateBundleFetcher)

class CertificateBundleFetcherWrapper : public CertificateBundleFetcher
{
public:
  CertificateBundleFetcherWrapper(Face& face)
    : CertificateBundleFetcher(make_unique<CertificateFetcherFromNetwork>(face), face)
  {
  }
};

class Bundle
{
};

class Cert
{
};

class Timeout
{
};

class Nack
{
};

template<class Response>
class CertificateBundleFetcherFixture : public HierarchicalValidatorFixture<ValidationPolicySimpleHierarchy,
                                                                            CertificateBundleFetcherWrapper>
{
public:
  CertificateBundleFetcherFixture()
    : data("/Security/V2/ValidatorFixture/Sub1/Sub3/Data")
  {
    subSubIdentity = addSubCertificate("/Security/V2/ValidatorFixture/Sub1/Sub3", subIdentity);
    cache.insert(subSubIdentity.getDefaultKey().getDefaultCertificate());

    m_keyChain.sign(data, signingByIdentity(subSubIdentity));
    bundleRegexMatcher = make_shared<RegexPatternListMatcher>("<>*<_BUNDLE><>*", nullptr);
    processInterest = [this] (const Interest& interest) {
      // check if the interest is for Bundle or individual certificates
      if (bundleRegexMatcher->match(interest.getName(), 0, interest.getName().size())) {
        makeResponse(interest);
      }
      else {
        auto cert = cache.find(interest);
        if (cert == nullptr) {
          return;
        }
        face.receive(*cert);
      }
    };
  }

  void
  makeResponse(const Interest& interest);

public:
  Data data;
  Identity subSubIdentity;
  shared_ptr<RegexPatternListMatcher> bundleRegexMatcher;
};

template<>
void
CertificateBundleFetcherFixture<Bundle>::makeResponse(const Interest& interest)
{
  Block certList = Block(tlv::Content);
  Name bundleName(interest.getName());

  if (!bundleName.get(-1).isSegment() || bundleName.get(-1).toSegment() == 0) {
    Block subSubCert = subSubIdentity.getDefaultKey().getDefaultCertificate().wireEncode();
    certList.push_back(subSubCert);

    if (!bundleName.get(-1).isSegment()) {
      bundleName
        .appendVersion()
        .appendSegment(0);
    }
  }
  else {
    Block subCert = subIdentity.getDefaultKey().getDefaultCertificate().wireEncode();
    Block anchor = identity.getDefaultKey().getDefaultCertificate().wireEncode();
    certList.push_back(subCert);
    certList.push_back(anchor);
  }

  shared_ptr<Data> certBundle = make_shared<Data>();
  certBundle->setName(bundleName);
  certBundle->setFreshnessPeriod(100_s);
  certBundle->setContent(certList);
  certBundle->setFinalBlock(name::Component::fromSegment(1));

  m_keyChain.sign(*certBundle, signingWithSha256());

  face.receive(*certBundle);
}

template<>
void
CertificateBundleFetcherFixture<Timeout>::makeResponse(const Interest& interest)
{
  this->advanceClocks(200_s);
}

template<>
void
CertificateBundleFetcherFixture<Nack>::makeResponse(const Interest& interest)
{
  lp::Nack nack(interest);
  nack.setHeader(lp::NackHeader().setReason(lp::NackReason::NO_ROUTE));
  face.receive(nack);
}

BOOST_FIXTURE_TEST_CASE(ValidateSuccessWithBundle, CertificateBundleFetcherFixture<Bundle>)
{
  VALIDATE_SUCCESS(this->data, "Should get accepted, as interest brings the bundle segments");
  BOOST_CHECK_EQUAL(this->face.sentInterests.size(), 2); // produced bundle has 2 segments

  for (const auto& sentInterest : this->face.sentInterests) {
    BOOST_CHECK(this->bundleRegexMatcher->match(sentInterest.getName(), 0, sentInterest.getName().size()));
  }
}

using SuccessWithoutBundle = boost::mpl::vector<Nack, Timeout>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateSuccessWithoutBundle, T, SuccessWithoutBundle, CertificateBundleFetcherFixture<T>)
{
  VALIDATE_SUCCESS(this->data, "Should get accepted, as interest brings the certs");
  BOOST_CHECK_EQUAL(this->face.sentInterests.size(), 4); // since interest for Bundle fails, each cert is retrieved

  bool toggle = true;
  for (const auto& sentInterest : this->face.sentInterests) {
    if (toggle) {
      // every alternate interest is going to be that of a bundle
      BOOST_CHECK(this->bundleRegexMatcher->match(sentInterest.getName(), 0, sentInterest.getName().size()));
    }
    else {
      BOOST_CHECK(!this->bundleRegexMatcher->match(sentInterest.getName(), 0, sentInterest.getName().size()));
    }
    toggle = !toggle;
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateBundleFetcher
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
