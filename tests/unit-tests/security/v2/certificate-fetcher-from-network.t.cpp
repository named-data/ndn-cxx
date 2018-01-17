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

#include "security/v2/certificate-fetcher-from-network.hpp"
#include "security/v2/validation-policy-simple-hierarchy.hpp"
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
BOOST_AUTO_TEST_SUITE(TestCertificateFetcherFromNetwork)

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
class CertificateFetcherFromNetworkFixture : public HierarchicalValidatorFixture<ValidationPolicySimpleHierarchy,
                                                                                 CertificateFetcherFromNetwork>
{
public:
  CertificateFetcherFromNetworkFixture()
    : data("/Security/V2/ValidatorFixture/Sub1/Sub3/Data")
    , interest("/Security/V2/ValidatorFixture/Sub1/Sub3/Interest")
  {
    Identity subSubIdentity = addSubCertificate("/Security/V2/ValidatorFixture/Sub1/Sub3", subIdentity);
    cache.insert(subSubIdentity.getDefaultKey().getDefaultCertificate());

    m_keyChain.sign(data, signingByIdentity(subSubIdentity));
    m_keyChain.sign(interest, signingByIdentity(subSubIdentity));

    processInterest = bind(&CertificateFetcherFromNetworkFixture<Response>::makeResponse, this, _1);
  }

  void
  makeResponse(const Interest& interest);

public:
  Data data;
  Interest interest;
};

template<>
void
CertificateFetcherFromNetworkFixture<Cert>::makeResponse(const Interest& interest)
{
  auto cert = cache.find(interest);
  if (cert == nullptr) {
    return;
  }
  face.receive(*cert);
}

template<>
void
CertificateFetcherFromNetworkFixture<Timeout>::makeResponse(const Interest& interest)
{
  // do nothing
}

template<>
void
CertificateFetcherFromNetworkFixture<Nack>::makeResponse(const Interest& interest)
{
  lp::Nack nack(interest);
  nack.setHeader(lp::NackHeader().setReason(lp::NackReason::NO_ROUTE));
  face.receive(nack);
}

using Failures = boost::mpl::vector<Timeout, Nack>;

BOOST_FIXTURE_TEST_CASE(ValidateSuccess, CertificateFetcherFromNetworkFixture<Cert>)
{
  VALIDATE_SUCCESS(this->data, "Should get accepted, as normal interests bring cert");
  BOOST_CHECK_EQUAL(this->face.sentInterests.size(), 2);
  this->face.sentInterests.clear();

  this->advanceClocks(1_h, 2); // expire validator caches

  VALIDATE_SUCCESS(this->interest, "Should get accepted, as interests bring certs");
  BOOST_CHECK_EQUAL(this->face.sentInterests.size(), 2);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateFailure, T, Failures, CertificateFetcherFromNetworkFixture<T>)
{
  VALIDATE_FAILURE(this->data, "Should fail, as interests don't bring data");
  BOOST_CHECK_GT(this->face.sentInterests.size(), 2);
  this->face.sentInterests.clear();

  this->advanceClocks(1_h, 2); // expire validator caches

  VALIDATE_FAILURE(this->interest, "Should fail, as interests don't bring data");
  BOOST_CHECK_GT(this->face.sentInterests.size(), 2);
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateFetcherFromNetwork
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
