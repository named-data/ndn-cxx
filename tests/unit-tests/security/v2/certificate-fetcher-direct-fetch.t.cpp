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

#include "security/v2/certificate-fetcher-direct-fetch.hpp"
#include "security/v2/validation-policy-simple-hierarchy.hpp"
#include "lp/nack.hpp"
#include "lp/tags.hpp"

#include "boost-test.hpp"
#include "validator-fixture.hpp"

#include <boost/range/adaptor/strided.hpp>
#include <boost/range/adaptor/sliced.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(TestCertificateFetcherDirectFetch)

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
class CertificateFetcherDirectFetchFixture : public HierarchicalValidatorFixture<ValidationPolicySimpleHierarchy,
                                                                                 CertificateFetcherDirectFetch>
{
public:
  CertificateFetcherDirectFetchFixture()
    : data("/Security/V2/ValidatorFixture/Sub1/Sub3/Data")
    , interest("/Security/V2/ValidatorFixture/Sub1/Sub3/Interest")
    , interestNoTag("/Security/V2/ValidatorFixture/Sub1/Sub3/Interest2")
  {
    Identity subSubIdentity = addSubCertificate("/Security/V2/ValidatorFixture/Sub1/Sub3", subIdentity);
    cache.insert(subSubIdentity.getDefaultKey().getDefaultCertificate());

    m_keyChain.sign(data, signingByIdentity(subSubIdentity));
    m_keyChain.sign(interest, signingByIdentity(subSubIdentity));
    m_keyChain.sign(interestNoTag, signingByIdentity(subSubIdentity));

    data.setTag(make_shared<lp::IncomingFaceIdTag>(123));
    interest.setTag(make_shared<lp::IncomingFaceIdTag>(123));

    processInterest = [this] (const Interest& interest) {
      auto nextHopFaceIdTag = interest.template getTag<lp::NextHopFaceIdTag>();
      if (nextHopFaceIdTag == nullptr) {
        makeResponse(interest); // respond only to the "infrastructure" interest
      }
    };
  }

  void
  makeResponse(const Interest& interest);

public:
  Data data;
  Interest interest;
  Interest interestNoTag;
};

template<>
void
CertificateFetcherDirectFetchFixture<Cert>::makeResponse(const Interest& interest)
{
  auto cert = cache.find(interest);
  if (cert == nullptr) {
    return;
  }
  face.receive(*cert);
}

template<>
void
CertificateFetcherDirectFetchFixture<Timeout>::makeResponse(const Interest& interest)
{
  // do nothing
}

template<>
void
CertificateFetcherDirectFetchFixture<Nack>::makeResponse(const Interest& interest)
{
  lp::Nack nack(interest);
  nack.setHeader(lp::NackHeader().setReason(lp::NackReason::NO_ROUTE));
  face.receive(nack);
}

using Failures = boost::mpl::vector<Timeout, Nack>;

BOOST_FIXTURE_TEST_CASE(ValidateSuccessData, CertificateFetcherDirectFetchFixture<Cert>)
{
  VALIDATE_SUCCESS(this->data, "Should get accepted, normal and/or direct interests bring certs");
  BOOST_CHECK_EQUAL(this->face.sentInterests.size(), 4);

  // odd interests
  for (const auto& sentInterest : this->face.sentInterests | boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() != nullptr);
  }

  // even interests
  for (const auto& sentInterest : this->face.sentInterests |
                                    boost::adaptors::sliced(1, this->face.sentInterests.size()) |
                                    boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() == nullptr);
  }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateFailureData, T, Failures, CertificateFetcherDirectFetchFixture<T>)
{
  VALIDATE_FAILURE(this->data, "Should fail, as all interests either NACKed or timeout");
  BOOST_CHECK_GT(this->face.sentInterests.size(), 4);

  // odd interests
  for (const auto& sentInterest : this->face.sentInterests | boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() != nullptr);
  }

  // even interests
  for (const auto& sentInterest : this->face.sentInterests |
                                    boost::adaptors::sliced(1, this->face.sentInterests.size()) |
                                    boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() == nullptr);
  }
}

BOOST_FIXTURE_TEST_CASE(ValidateSuccessInterest, CertificateFetcherDirectFetchFixture<Cert>)
{
  VALIDATE_SUCCESS(this->interest, "Should get accepted, normal and/or direct interests bring certs");
  BOOST_CHECK_EQUAL(this->face.sentInterests.size(), 4);

  // odd interests
  for (const auto& sentInterest : this->face.sentInterests | boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() != nullptr);
  }

  // even interests
  for (const auto& sentInterest : this->face.sentInterests |
                                    boost::adaptors::sliced(1, this->face.sentInterests.size()) |
                                    boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() == nullptr);
  }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateFailureInterest, T, Failures, CertificateFetcherDirectFetchFixture<T>)
{
  VALIDATE_FAILURE(this->interest, "Should fail, as all interests either NACKed or timeout");
  BOOST_CHECK_GT(this->face.sentInterests.size(), 4);

  // odd interests
  for (const auto& sentInterest : this->face.sentInterests | boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() != nullptr);
  }

  // even interests
  for (const auto& sentInterest : this->face.sentInterests |
                                    boost::adaptors::sliced(1, this->face.sentInterests.size()) |
                                    boost::adaptors::strided(2)) {
    BOOST_CHECK(sentInterest.template getTag<lp::NextHopFaceIdTag>() == nullptr);
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateFetcherDirectFetch
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
