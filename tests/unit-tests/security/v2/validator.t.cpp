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

#include "security/v2/validator.hpp"
#include "security/v2/validation-policy-simple-hierarchy.hpp"

#include "boost-test.hpp"
#include "validator-fixture.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_FIXTURE_TEST_SUITE(TestValidator, HierarchicalValidatorFixture<ValidationPolicySimpleHierarchy>)

BOOST_AUTO_TEST_CASE(ConstructorSetValidator)
{
  auto middlePolicy = make_unique<ValidationPolicySimpleHierarchy>();
  auto innerPolicy = make_unique<ValidationPolicySimpleHierarchy>();

  validator.getPolicy().setInnerPolicy(std::move(middlePolicy));
  validator.getPolicy().setInnerPolicy(std::move(innerPolicy));

  BOOST_CHECK(validator.getPolicy().m_validator != nullptr);
  BOOST_CHECK(validator.getPolicy().getInnerPolicy().m_validator != nullptr);
  BOOST_CHECK(validator.getPolicy().getInnerPolicy().getInnerPolicy().m_validator != nullptr);
}

BOOST_AUTO_TEST_CASE(Timeouts)
{
  processInterest = nullptr; // no response for all interests

  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "Should fail to retrieve certificate");
  BOOST_CHECK_GT(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(NackedInterests)
{
  processInterest = [this] (const Interest& interest) {
    lp::Nack nack(interest);
    nack.setReason(lp::NackReason::NO_ROUTE);
    face.receive(nack);
  };

  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "All interests should get NACKed");
  BOOST_CHECK_GT(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(MalformedCert)
{
  Data malformedCert = subIdentity.getDefaultKey().getDefaultCertificate();
  malformedCert.setContentType(tlv::ContentType_Blob);
  m_keyChain.sign(malformedCert, signingByIdentity(identity));
  // wrong content type & missing ValidityPeriod
  BOOST_REQUIRE_THROW(Certificate(malformedCert.wireEncode()), tlv::Error);

  auto originalProcessInterest = processInterest;
  processInterest = [this, &originalProcessInterest, &malformedCert] (const Interest& interest) {
    if (interest.getName().isPrefixOf(malformedCert.getName())) {
      face.receive(malformedCert);
    }
    else {
      originalProcessInterest(interest);
    }
  };

  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "Signed by a malformed certificate");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(ExpiredCert)
{
  Data expiredCert = subIdentity.getDefaultKey().getDefaultCertificate();
  SignatureInfo info;
  info.setValidityPeriod(ValidityPeriod(time::system_clock::now() - 2_h,
                                        time::system_clock::now() - 1_h));
  m_keyChain.sign(expiredCert, signingByIdentity(identity).setSignatureInfo(info));
  BOOST_REQUIRE_NO_THROW(Certificate(expiredCert.wireEncode()));

  auto originalProcessInterest = processInterest;
  processInterest = [this, &originalProcessInterest, &expiredCert] (const Interest& interest) {
    if (interest.getName().isPrefixOf(expiredCert.getName())) {
      face.receive(expiredCert);
    }
    else {
      originalProcessInterest(interest);
    }
  };

  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "Signed by an expired certificate");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(ResetAnchors)
{
  validator.resetAnchors();

  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  VALIDATE_FAILURE(data, "Should fail, as no anchors configured");
}

BOOST_AUTO_TEST_CASE(TrustedCertCaching)
{
  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_SUCCESS(data, "Should get accepted, as signed by the policy-compliant cert");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  face.sentInterests.clear();

  processInterest = nullptr; // disable data responses from mocked network

  VALIDATE_SUCCESS(data, "Should get accepted, based on the cached trusted cert");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 0);
  face.sentInterests.clear();

  advanceClocks(1_h, 2); // expire trusted cache

  VALIDATE_FAILURE(data, "Should try and fail to retrieve certs");
  BOOST_CHECK_GT(face.sentInterests.size(), 1);
  face.sentInterests.clear();
}

BOOST_AUTO_TEST_CASE(ResetVerifiedCertificates)
{
  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  VALIDATE_SUCCESS(data, "Should get accepted, as signed by the policy-compliant cert");

  // reset anchors
  validator.resetAnchors();
  VALIDATE_SUCCESS(data, "Should get accepted, as signed by the cert in trusted cache");

  // reset trusted cache
  validator.resetVerifiedCertificates();
  VALIDATE_FAILURE(data, "Should fail, as no trusted cache or anchors");
}

BOOST_AUTO_TEST_CASE(UntrustedCertCaching)
{
  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subSelfSignedIdentity));

  VALIDATE_FAILURE(data, "Should fail, as signed by the policy-violating cert");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  face.sentInterests.clear();

  processInterest = nullptr; // disable data responses from mocked network

  VALIDATE_FAILURE(data, "Should fail again, but no network operations expected");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 0);
  face.sentInterests.clear();

  advanceClocks(10_min, 2); // expire untrusted cache

  VALIDATE_FAILURE(data, "Should try and fail to retrieve certs");
  BOOST_CHECK_GT(face.sentInterests.size(), 1);
  face.sentInterests.clear();
}

class ValidationPolicySimpleHierarchyForInterestOnly : public ValidationPolicySimpleHierarchy
{
public:
  void
  checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) override
  {
    continueValidation(nullptr, state);
  }
};

BOOST_FIXTURE_TEST_CASE(ValidateInterestsButBypassForData,
                        HierarchicalValidatorFixture<ValidationPolicySimpleHierarchyForInterestOnly>)
{
  Interest interest("/Security/V2/ValidatorFixture/Sub1/Sub2/Interest");
  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Interest");

  VALIDATE_FAILURE(interest, "Unsigned");
  VALIDATE_SUCCESS(data, "Policy requests validation bypassing for all data");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 0);
  face.sentInterests.clear();

  interest = Interest("/Security/V2/ValidatorFixture/Sub1/Sub2/Interest");
  m_keyChain.sign(interest, signingWithSha256());
  m_keyChain.sign(data, signingWithSha256());
  VALIDATE_FAILURE(interest, "Required KeyLocator/Name missing (not passed to policy)");
  VALIDATE_SUCCESS(data, "Policy requests validation bypassing for all data");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 0);
  face.sentInterests.clear();

  m_keyChain.sign(interest, signingByIdentity(identity));
  m_keyChain.sign(data, signingByIdentity(identity));
  VALIDATE_SUCCESS(interest, "Should get accepted, as signed by the anchor");
  VALIDATE_SUCCESS(data, "Policy requests validation bypassing for all data");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 0);
  face.sentInterests.clear();

  m_keyChain.sign(interest, signingByIdentity(subIdentity));
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  VALIDATE_FAILURE(interest, "Should fail, as policy is not allowed to create new trust anchors");
  VALIDATE_SUCCESS(data, "Policy requests validation bypassing for all data");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  face.sentInterests.clear();

  m_keyChain.sign(interest, signingByIdentity(otherIdentity));
  m_keyChain.sign(data, signingByIdentity(otherIdentity));
  VALIDATE_FAILURE(interest, "Should fail, as signed by the policy-violating cert");
  VALIDATE_SUCCESS(data, "Policy requests validation bypassing for all data");
  // no network operations expected, as certificate is not validated by the policy
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 0);
  face.sentInterests.clear();

  advanceClocks(1_h, 2); // expire trusted cache

  m_keyChain.sign(interest, signingByIdentity(subSelfSignedIdentity));
  m_keyChain.sign(data, signingByIdentity(subSelfSignedIdentity));
  VALIDATE_FAILURE(interest, "Should fail, as policy is not allowed to create new trust anchors");
  VALIDATE_SUCCESS(data, "Policy requests validation bypassing for all data");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  face.sentInterests.clear();
}

BOOST_AUTO_TEST_CASE(InfiniteCertChain)
{
  processInterest = [this] (const Interest& interest) {
    // create another key for the same identity and sign it properly
    Key parentKey = m_keyChain.createKey(subIdentity);
    Key requestedKey = subIdentity.getKey(interest.getName());

    Name certificateName = requestedKey.getName();
    certificateName
    .append("looper")
    .appendVersion();
    v2::Certificate certificate;
    certificate.setName(certificateName);

    // set metainfo
    certificate.setContentType(tlv::ContentType_Key);
    certificate.setFreshnessPeriod(1_h);

    // set content
    certificate.setContent(requestedKey.getPublicKey().data(), requestedKey.getPublicKey().size());

    // set signature-info
    SignatureInfo info;
    info.setValidityPeriod(security::ValidityPeriod(time::system_clock::now() - 10_days,
                                                    time::system_clock::now() + 10_days));

    m_keyChain.sign(certificate, signingByKey(parentKey).setSignatureInfo(info));
    face.receive(certificate);
  };

  Data data("/Security/V2/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  validator.setMaxDepth(40);
  BOOST_CHECK_EQUAL(validator.getMaxDepth(), 40);
  VALIDATE_FAILURE(data, "Should fail, as certificate should be looped");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 40);
  face.sentInterests.clear();

  advanceClocks(1_h, 5); // expire caches

  validator.setMaxDepth(30);
  BOOST_CHECK_EQUAL(validator.getMaxDepth(), 30);
  VALIDATE_FAILURE(data, "Should fail, as certificate chain is infinite");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 30);
}

BOOST_AUTO_TEST_CASE(LoopedCertChain)
{
  auto s1 = addIdentity("/loop");
  auto k1 = m_keyChain.createKey(s1, RsaKeyParams(name::Component("key1")));
  auto k2 = m_keyChain.createKey(s1, RsaKeyParams(name::Component("key2")));
  auto k3 = m_keyChain.createKey(s1, RsaKeyParams(name::Component("key3")));

  auto makeCert = [this] (Key& key, const Key& signer) {
    v2::Certificate request = key.getDefaultCertificate();
    request.setName(Name(key.getName()).append("looper").appendVersion());

    SignatureInfo info;
    info.setValidityPeriod({time::system_clock::now() - 100_days,
                            time::system_clock::now() + 100_days});
    m_keyChain.sign(request, signingByKey(signer).setSignatureInfo(info));
    m_keyChain.addCertificate(key, request);

    cache.insert(request);
  };

  makeCert(k1, k2);
  makeCert(k2, k3);
  makeCert(k3, k1);

  Data data("/loop/Data");
  m_keyChain.sign(data, signingByKey(k1));
  VALIDATE_FAILURE(data, "Should fail, as certificate chain loops");
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END() // TestValidator
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
