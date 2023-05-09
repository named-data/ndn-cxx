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
#include "ndn-cxx/security/validation-policy-simple-hierarchy.hpp"

#include "tests/test-common.hpp"
#include "tests/unit/security/validator-fixture.hpp"

namespace ndn::tests {

using namespace ndn::security;

BOOST_AUTO_TEST_SUITE(Security)
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

  BOOST_CHECK_THROW(validator.getPolicy().setInnerPolicy(nullptr), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(BadSignatureInfo)
{
  Interest interest("/Security/ValidatorFixture/Sub1/Sub2/Interest");
  m_keyChain.sign(interest, signingByIdentity(subIdentity)
                            .setSignedInterestFormat(SignedInterestFormat::V03));

  // add an unrecognized critical element inside InterestSignatureInfo
  auto si = interest.getSignatureInfo().value();
  si.addCustomTlv("7F00"_block);
  interest.setSignatureInfo(si);
  BOOST_REQUIRE_THROW(interest.getSignatureInfo(), tlv::Error);
  BOOST_REQUIRE_NO_THROW(interest.getSignatureValue());

  VALIDATE_FAILURE(interest, "InterestSignatureInfo decoding should fail");
  BOOST_TEST(lastError.getCode() == ValidationError::MALFORMED_SIGNATURE);
  BOOST_TEST(face.sentInterests.size() == 0);
}

BOOST_AUTO_TEST_CASE(BadSignatureValue)
{
  const uint8_t sv[] = {0x12, 0x34, 0x56, 0x78};

  Interest interest("/Security/ValidatorFixture/Sub1/Sub2/Interest");
  m_keyChain.sign(interest, signingByIdentity(subIdentity)
                            .setSignedInterestFormat(SignedInterestFormat::V03));
  interest.setSignatureValue(sv);

  VALIDATE_FAILURE(interest, "Signature check should fail");
  BOOST_TEST(lastError.getCode() == ValidationError::INVALID_SIGNATURE);
  BOOST_TEST(face.sentInterests.size() == 1);

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  data.setSignatureValue(sv);

  VALIDATE_FAILURE(data, "Signature check should fail");
  BOOST_TEST(lastError.getCode() == ValidationError::INVALID_SIGNATURE);
  BOOST_TEST(face.sentInterests.size() == 1);
}

BOOST_AUTO_TEST_CASE(Timeout)
{
  processInterest = nullptr; // no response for any interest

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "Should fail to retrieve certificate");
  BOOST_TEST(lastError.getCode() == ValidationError::CANNOT_RETRIEVE_CERT);
  BOOST_TEST(face.sentInterests.size() == 4);
}

BOOST_AUTO_TEST_CASE(Nack)
{
  processInterest = [this] (const Interest& interest) {
    face.receive(makeNack(interest, lp::NackReason::NO_ROUTE));
  };

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "All interests should get NACKed");
  BOOST_TEST(lastError.getCode() == ValidationError::CANNOT_RETRIEVE_CERT);
  // 1 for the first interest, 3 for the retries on nack
  BOOST_TEST(face.sentInterests.size() == 4);
}

BOOST_AUTO_TEST_CASE(MalformedCert)
{
  Data malformedCert = subIdentity.getDefaultKey().getDefaultCertificate();
  malformedCert.setContentType(tlv::ContentType_Blob);
  m_keyChain.sign(malformedCert, signingByIdentity(identity));
  // wrong content type & missing ValidityPeriod
  BOOST_REQUIRE_THROW(Certificate(malformedCert.wireEncode()), tlv::Error);

  auto originalProcessInterest = processInterest;
  processInterest = [&] (const Interest& interest) {
    if (interest.getName().isPrefixOf(malformedCert.getName())) {
      face.receive(malformedCert);
    }
    else {
      originalProcessInterest(interest);
    }
  };

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "Signed by a malformed certificate");
  BOOST_TEST(lastError.getCode() == ValidationError::MALFORMED_CERT);
  BOOST_TEST(face.sentInterests.size() == 1);
}

BOOST_AUTO_TEST_CASE(ExpiredCert)
{
  Data expiredCert = subIdentity.getDefaultKey().getDefaultCertificate();
  SignatureInfo info;
  info.setValidityPeriod(ValidityPeriod::makeRelative(-2_h, -1_h));
  m_keyChain.sign(expiredCert, signingByIdentity(identity).setSignatureInfo(info));
  BOOST_REQUIRE_NO_THROW(Certificate(expiredCert.wireEncode()));

  auto originalProcessInterest = processInterest;
  processInterest = [&] (const Interest& interest) {
    if (interest.getName().isPrefixOf(expiredCert.getName())) {
      face.receive(expiredCert);
    }
    else {
      originalProcessInterest(interest);
    }
  };

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_FAILURE(data, "Signed by an expired certificate");
  BOOST_TEST(lastError.getCode() == ValidationError::EXPIRED_CERT);
  BOOST_TEST(face.sentInterests.size() == 1);
}

BOOST_AUTO_TEST_CASE(ResetAnchors)
{
  validator.resetAnchors();

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  VALIDATE_FAILURE(data, "Should fail, as no anchors configured");
  BOOST_TEST(lastError.getCode() == ValidationError::LOOP_DETECTED);
}

BOOST_AUTO_TEST_CASE(TrustedCertCaching)
{
  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));

  VALIDATE_SUCCESS(data, "Should get accepted, as signed by the policy-compliant cert");
  BOOST_TEST(face.sentInterests.size() == 1);
  face.sentInterests.clear();

  processInterest = nullptr; // disable data responses from mocked network

  VALIDATE_SUCCESS(data, "Should get accepted, based on the cached trusted cert");
  BOOST_TEST(face.sentInterests.size() == 0);
  face.sentInterests.clear();

  advanceClocks(1_h, 2); // expire trusted cache

  VALIDATE_FAILURE(data, "Should try and fail to retrieve certs");
  BOOST_TEST(lastError.getCode() == ValidationError::CANNOT_RETRIEVE_CERT);
  BOOST_TEST(face.sentInterests.size() > 1);
}

BOOST_AUTO_TEST_CASE(ResetVerifiedCerts)
{
  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  VALIDATE_SUCCESS(data, "Should get accepted, as signed by the policy-compliant cert");

  // reset anchors
  validator.resetAnchors();
  VALIDATE_SUCCESS(data, "Should get accepted, as signed by the cert in trusted cache");

  // reset trusted cache
  validator.resetVerifiedCertificates();
  VALIDATE_FAILURE(data, "Should fail, as no trusted cache or anchors");
  BOOST_TEST(lastError.getCode() == ValidationError::LOOP_DETECTED);
}

BOOST_AUTO_TEST_CASE(UntrustedCertCaching)
{
  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subSelfSignedIdentity));

  VALIDATE_FAILURE(data, "Should fail, as signed by the policy-violating cert");
  BOOST_TEST(lastError.getCode() == ValidationError::LOOP_DETECTED);
  BOOST_TEST(face.sentInterests.size() == 1);
  face.sentInterests.clear();

  processInterest = nullptr; // disable data responses from mocked network

  VALIDATE_FAILURE(data, "Should fail again, but no network operations expected");
  BOOST_TEST(lastError.getCode() == ValidationError::LOOP_DETECTED);
  BOOST_TEST(face.sentInterests.size() == 0);
  face.sentInterests.clear();

  advanceClocks(10_min, 2); // expire untrusted cache

  VALIDATE_FAILURE(data, "Should try and fail to retrieve certs");
  BOOST_TEST(lastError.getCode() == ValidationError::CANNOT_RETRIEVE_CERT);
  BOOST_TEST(face.sentInterests.size() > 1);
}

class ValidationPolicySimpleHierarchyForInterestOnly : public ValidationPolicySimpleHierarchy
{
public:
  void
  checkPolicy(const Data&, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) override
  {
    continueValidation(nullptr, state);
  }
};

BOOST_FIXTURE_TEST_CASE(ValidateInterestsButBypassForData,
                        HierarchicalValidatorFixture<ValidationPolicySimpleHierarchyForInterestOnly>)
{
  Interest interest("/Security/ValidatorFixture/Sub1/Sub2/Interest");
  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");

  VALIDATE_FAILURE(interest, "Unsigned");
  BOOST_TEST(lastError.getCode() == ValidationError::MALFORMED_SIGNATURE);
  VALIDATE_SUCCESS(data, "Policy bypasses validation for all data");
  BOOST_TEST(face.sentInterests.size() == 0);
  face.sentInterests.clear();

  interest = Interest("/Security/ValidatorFixture/Sub1/Sub2/Interest");
  m_keyChain.sign(interest, signingWithSha256());
  m_keyChain.sign(data, signingWithSha256());
  VALIDATE_FAILURE(interest, "Required KeyLocator/Name missing (not passed to policy)");
  BOOST_TEST(lastError.getCode() == ValidationError::POLICY_ERROR);
  VALIDATE_SUCCESS(data, "Policy bypasses validation for all data");
  BOOST_TEST(face.sentInterests.size() == 0);
  face.sentInterests.clear();

  m_keyChain.sign(interest, signingByIdentity(identity));
  m_keyChain.sign(data, signingByIdentity(identity));
  VALIDATE_SUCCESS(interest, "Should get accepted, as signed by the anchor");
  VALIDATE_SUCCESS(data, "Policy bypasses validation for all data");
  BOOST_TEST(face.sentInterests.size() == 0);
  face.sentInterests.clear();

  m_keyChain.sign(interest, signingByIdentity(subIdentity));
  m_keyChain.sign(data, signingByIdentity(subIdentity));
  VALIDATE_FAILURE(interest, "Should fail, as policy is not allowed to create new trust anchors");
  BOOST_TEST(lastError.getCode() == ValidationError::POLICY_ERROR);
  VALIDATE_SUCCESS(data, "Policy bypasses validation for all data");
  BOOST_TEST(face.sentInterests.size() == 1);
  face.sentInterests.clear();

  m_keyChain.sign(interest, signingByIdentity(otherIdentity));
  m_keyChain.sign(data, signingByIdentity(otherIdentity));
  VALIDATE_FAILURE(interest, "Should fail, as signed by the policy-violating cert");
  BOOST_TEST(lastError.getCode() == ValidationError::POLICY_ERROR);
  VALIDATE_SUCCESS(data, "Policy bypasses validation for all data");
  // no network operations expected, as certificate is not validated by the policy
  BOOST_TEST(face.sentInterests.size() == 0);
  face.sentInterests.clear();

  advanceClocks(1_h, 2); // expire trusted cache

  m_keyChain.sign(interest, signingByIdentity(subSelfSignedIdentity));
  m_keyChain.sign(data, signingByIdentity(subSelfSignedIdentity));
  VALIDATE_FAILURE(interest, "Should fail, as policy is not allowed to create new trust anchors");
  BOOST_TEST(lastError.getCode() == ValidationError::POLICY_ERROR);
  VALIDATE_SUCCESS(data, "Policy bypasses validation for all data");
  BOOST_TEST(face.sentInterests.size() == 1);
  face.sentInterests.clear();
}

BOOST_AUTO_TEST_CASE(InfiniteCertChain)
{
  processInterest = [this] (const Interest& interest) {
    // create another key for the same identity and sign it properly
    Key parentKey = m_keyChain.createKey(subIdentity);
    Key requestedKey = subIdentity.getKey(interest.getName());

    SignatureInfo sigInfo;
    sigInfo.setKeyLocator(parentKey.getName());
    auto si = signingByKey(parentKey).setSignatureInfo(sigInfo);

    auto cert = m_keyChain.makeCertificate(requestedKey, si);
    face.receive(cert);
  };

  Data data("/Security/ValidatorFixture/Sub1/Sub2/Data");
  m_keyChain.sign(data, signingByIdentity(subIdentity)
                        .setSignatureInfo(SignatureInfo()
                                          .setKeyLocator(subIdentity.getDefaultKey().getName())));

  validator.setMaxDepth(40);
  BOOST_CHECK_EQUAL(validator.getMaxDepth(), 40);
  VALIDATE_FAILURE(data, "Should fail, as certificate should be looped");
  BOOST_TEST(lastError.getCode() == ValidationError::EXCEEDED_DEPTH_LIMIT);
  BOOST_TEST(face.sentInterests.size() == 40);
  face.sentInterests.clear();

  advanceClocks(1_h, 5); // expire caches

  validator.setMaxDepth(30);
  BOOST_CHECK_EQUAL(validator.getMaxDepth(), 30);
  VALIDATE_FAILURE(data, "Should fail, as certificate chain is infinite");
  BOOST_TEST(lastError.getCode() == ValidationError::EXCEEDED_DEPTH_LIMIT);
  BOOST_TEST(face.sentInterests.size() == 30);
}

BOOST_AUTO_TEST_CASE(LoopedCertChain)
{
  auto s1 = m_keyChain.createIdentity("/loop");
  auto k1 = m_keyChain.createKey(s1, RsaKeyParams(name::Component("key1")));
  auto k2 = m_keyChain.createKey(s1, RsaKeyParams(name::Component("key2")));
  auto k3 = m_keyChain.createKey(s1, RsaKeyParams(name::Component("key3")));

  auto makeLoopCert = [this] (Key& key, const Key& signer) {
    SignatureInfo sigInfo;
    sigInfo.setKeyLocator(signer.getName());
    auto si = signingByKey(signer).setSignatureInfo(sigInfo);

    auto cert = m_keyChain.makeCertificate(key, si);
    m_keyChain.setDefaultCertificate(key, cert);
    cache.insert(cert);
  };

  makeLoopCert(k1, k2);
  makeLoopCert(k2, k3);
  makeLoopCert(k3, k1);

  Data data("/loop/Data");
  m_keyChain.sign(data, signingByKey(k1));
  VALIDATE_FAILURE(data, "Should fail, as certificate chain loops");
  BOOST_TEST(lastError.getCode() == ValidationError::LOOP_DETECTED);
  BOOST_TEST_REQUIRE(face.sentInterests.size() == 3);
  BOOST_CHECK_EQUAL(face.sentInterests[0].getName(), k1.getDefaultCertificate().getName());
  BOOST_CHECK_EQUAL(face.sentInterests[1].getName(), k2.getName());
  BOOST_CHECK_EQUAL(face.sentInterests[2].getName(), k3.getName());
}

BOOST_AUTO_TEST_SUITE_END() // TestValidator
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
