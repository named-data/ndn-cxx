/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/validation-policy-signed-interest.hpp"

#include "ndn-cxx/security/interest-signer.hpp"
#include "ndn-cxx/security/validation-policy-accept-all.hpp"
#include "ndn-cxx/security/validation-policy-simple-hierarchy.hpp"

#include "tests/test-common.hpp"
#include "tests/unit/security/validator-fixture.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)

class SignedInterestDefaultOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    return {};
  }
};

template<class T, class InnerPolicy>
class SignedInterestPolicyWrapper : public ValidationPolicySignedInterest
{
public:
  SignedInterestPolicyWrapper()
    : ValidationPolicySignedInterest(make_unique<InnerPolicy>(), T::getOptions())
  {
  }
};

template<class T, class InnerPolicy = ValidationPolicySimpleHierarchy>
class ValidationPolicySignedInterestFixture
  : public HierarchicalValidatorFixture<SignedInterestPolicyWrapper<T, InnerPolicy>>
{
public:
  ValidationPolicySignedInterestFixture()
    : m_signer(this->m_keyChain)
  {
  }

  Interest
  makeSignedInterest(const Identity& identity,
                     uint32_t signingFlags = InterestSigner::WantNonce | InterestSigner::WantTime)
  {
    Interest i(Name(identity.getName()).append("CMD"));
    m_signer.makeSignedInterest(i, signingByIdentity(identity), signingFlags);
    return i;
  }

public:
  InterestSigner m_signer;

  static constexpr uint32_t WantAll = InterestSigner::WantNonce |
                                        InterestSigner::WantTime |
                                        InterestSigner::WantSeqNum;
};

BOOST_FIXTURE_TEST_SUITE(TestValidationPolicySignedInterest,
                         ValidationPolicySignedInterestFixture<SignedInterestDefaultOptions>)

BOOST_AUTO_TEST_CASE(BasicV3)
{
  auto i1 = makeSignedInterest(identity, WantAll);
  VALIDATE_SUCCESS(i1, "Should succeed (within grace period)");
  VALIDATE_FAILURE(i1, "Should fail (replay attack)");

  advanceClocks(5_ms);
  auto i2 = makeSignedInterest(identity, WantAll);
  VALIDATE_SUCCESS(i2, "Should succeed (timestamp and sequence number larger than previous)");

  Interest i3(Name(identity.getName()).append("CMD"));
  m_signer.makeSignedInterest(i3, signingWithSha256());
  VALIDATE_FAILURE(i3, "Should fail (Sha256 signature violates policy)");
}

BOOST_AUTO_TEST_CASE(DataPassthrough)
{
  Data d1("/Security/ValidatorFixture/Sub1");
  m_keyChain.sign(d1);
  VALIDATE_SUCCESS(d1, "Should succeed (fallback on inner validation policy for data)");
}

BOOST_AUTO_TEST_CASE(InnerPolicyReject)
{
  auto i1 = makeSignedInterest(otherIdentity);
  VALIDATE_FAILURE(i1, "Should fail (inner policy should reject)");
}

class LimitedRecordsOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.timestampGracePeriod = 15_s;
    options.maxRecordCount = 3;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(LimitedRecords, ValidationPolicySignedInterestFixture<LimitedRecordsOptions>)
{
  Identity id1 = addSubCertificate("/Security/ValidatorFixture/Sub1", identity);
  cache.insert(id1.getDefaultKey().getDefaultCertificate());
  Identity id2 = addSubCertificate("/Security/ValidatorFixture/Sub2", identity);
  cache.insert(id2.getDefaultKey().getDefaultCertificate());
  Identity id3 = addSubCertificate("/Security/ValidatorFixture/Sub3", identity);
  cache.insert(id3.getDefaultKey().getDefaultCertificate());
  Identity id4 = addSubCertificate("/Security/ValidatorFixture/Sub4", identity);
  cache.insert(id4.getDefaultKey().getDefaultCertificate());

  auto i1 = makeSignedInterest(id2);
  auto i2 = makeSignedInterest(id3);
  auto i3 = makeSignedInterest(id4);
  auto i00 = makeSignedInterest(id1); // signed at 0s
  advanceClocks(1_s);
  auto i01 = makeSignedInterest(id1); // signed at 1s
  advanceClocks(1_s);
  auto i02 = makeSignedInterest(id1); // signed at 2s

  VALIDATE_SUCCESS(i00, "Should succeed");
  rewindClockAfterValidation();

  VALIDATE_SUCCESS(i02, "Should succeed");
  rewindClockAfterValidation();

  VALIDATE_SUCCESS(i1, "Should succeed");
  rewindClockAfterValidation();

  VALIDATE_SUCCESS(i2, "Should succeed");
  rewindClockAfterValidation();

  VALIDATE_SUCCESS(i3, "Should succeed, forgets identity id1");
  rewindClockAfterValidation();

  VALIDATE_SUCCESS(i01, "Should succeed despite timestamp is reordered, because record has been evicted");
}

class UnlimitedRecordsOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.timestampGracePeriod = 15_s;
    options.maxRecordCount = -1;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(UnlimitedRecords, ValidationPolicySignedInterestFixture<UnlimitedRecordsOptions>)
{
  std::vector<Identity> identities;
  for (size_t i = 0; i < 20; ++i) {
    Identity id = addSubCertificate("/Security/ValidatorFixture/Sub" + to_string(i), identity);
    cache.insert(id.getDefaultKey().getDefaultCertificate());
    identities.push_back(id);
  }

  auto i1 = makeSignedInterest(identities.at(0)); // signed at 0s
  advanceClocks(1_s);
  for (size_t i = 0; i < 20; ++i) {
    auto i2 = makeSignedInterest(identities.at(i)); // signed at +1s

    VALIDATE_SUCCESS(i2, "Should succeed");
    rewindClockAfterValidation();
  }
  VALIDATE_FAILURE(i1, "Should fail (timestamp reorder)");
}

class ZeroRecordsOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.timestampGracePeriod = 15_s;
    options.maxRecordCount = 0;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(ZeroRecords, ValidationPolicySignedInterestFixture<ZeroRecordsOptions>)
{
  auto i1 = makeSignedInterest(identity); // signed at 0s
  advanceClocks(1_s);
  auto i2 = makeSignedInterest(identity); // signed at +1s
  VALIDATE_SUCCESS(i2, "Should succeed");
  rewindClockAfterValidation();

  VALIDATE_SUCCESS(i1, "Should succeed despite timestamp reordering, as records aren't kept");
}

BOOST_AUTO_TEST_SUITE(TimestampValidation)

BOOST_AUTO_TEST_CASE(MissingTimestamp)
{
  auto i1 = makeSignedInterest(identity, InterestSigner::WantSeqNum);
  VALIDATE_FAILURE(i1, "Should fail (timestamp missing)");
}

class DisabledTimestampValidationOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.shouldValidateTimestamps = false;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(Disabled,
                        ValidationPolicySignedInterestFixture<DisabledTimestampValidationOptions>)
{
  auto i1 = makeSignedInterest(identity); // signed at 0ms
  advanceClocks(100_ms);
  VALIDATE_SUCCESS(i1, "Should succeed");

  auto i2 = makeSignedInterest(identity); // signed at +100ms
  // Set i2 to have same timestamp as i1
  auto si2 = i2.getSignatureInfo();
  si2->setTime(i2.getSignatureInfo()->getTime());
  i2.setSignatureInfo(*si2);
  VALIDATE_SUCCESS(i2, "Should succeed");
}

class GracePeriod15Sec
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.timestampGracePeriod = 15_s;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(TimestampTooOld, ValidationPolicySignedInterestFixture<GracePeriod15Sec>)
{
  auto i1 = makeSignedInterest(identity); // signed at 0s
  advanceClocks(16_s); // verifying at +16s
  VALIDATE_FAILURE(i1, "Should fail (timestamp outside the grace period)");
  rewindClockAfterValidation();

  auto i2 = makeSignedInterest(identity); // signed at +16s
  VALIDATE_SUCCESS(i2, "Should succeed");
}

BOOST_FIXTURE_TEST_CASE(TimestampTooNew, ValidationPolicySignedInterestFixture<GracePeriod15Sec>)
{
  auto i1 = makeSignedInterest(identity); // signed at 0s
  advanceClocks(1_s);
  auto i2 = makeSignedInterest(identity); // signed at +1s
  advanceClocks(1_s);
  auto i3 = makeSignedInterest(identity); // signed at +2s

  m_systemClock->advance(-18_s); // verifying at -16s
  VALIDATE_FAILURE(i1, "Should fail (timestamp outside the grace period)");
  rewindClockAfterValidation();

  // SignedInterestValidator should not remember i1's timestamp
  VALIDATE_FAILURE(i2, "Should fail (timestamp outside the grace period)");
  rewindClockAfterValidation();

  // SignedInterestValidator should not remember i2's timestamp, and should treat i3 as initial
  advanceClocks(18_s); // verifying at +2s
  VALIDATE_SUCCESS(i3, "Should succeed");
}

BOOST_AUTO_TEST_CASE(TimestampReorderEqual)
{
  auto i1 = makeSignedInterest(identity); // signed at 0s
  VALIDATE_SUCCESS(i1, "Should succeed");

  auto i2 = makeSignedInterest(identity); // signed at 0s
  auto si2 = i2.getSignatureInfo();
  si2->setTime(i1.getSignatureInfo()->getTime());
  i2.setSignatureInfo(*si2);
  VALIDATE_FAILURE(i2, "Should fail (timestamp reordered)");

  advanceClocks(2_s);
  auto i3 = makeSignedInterest(identity); // signed at +2s
  VALIDATE_SUCCESS(i3, "Should succeed");
}

BOOST_AUTO_TEST_CASE(TimestampReorderNegative)
{
  auto i2 = makeSignedInterest(identity); // signed at 0ms
  advanceClocks(200_ms);
  auto i3 = makeSignedInterest(identity); // signed at +200ms
  advanceClocks(900_ms);
  auto i1 = makeSignedInterest(identity); // signed at +1100ms
  advanceClocks(300_ms);
  auto i4 = makeSignedInterest(identity); // signed at +1400ms

  m_systemClock->advance(-300_ms); // verifying at +1100ms
  VALIDATE_SUCCESS(i1, "Should succeed");
  rewindClockAfterValidation();

  m_systemClock->advance(-1100_ms); // verifying at 0ms
  VALIDATE_FAILURE(i2, "Should fail (timestamp reordered)");
  rewindClockAfterValidation();

  // SignedInterestValidator should not remember i2's timestamp
  advanceClocks(200_ms); // verifying at +200ms
  VALIDATE_FAILURE(i3, "Should fail (timestamp reordered)");
  rewindClockAfterValidation();

  advanceClocks(1200_ms); // verifying at 1400ms
  VALIDATE_SUCCESS(i4, "Should succeed");
}

template<class T>
class GracePeriod
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.timestampGracePeriod = time::seconds(T::value);
    return options;
  }
};

typedef boost::mpl::vector<
  GracePeriod<boost::mpl::int_<0>>,
  GracePeriod<boost::mpl::int_<-1>>
> GraceNonPositiveValues;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(GraceNonPositive, GracePeriod, GraceNonPositiveValues,
                                 ValidationPolicySignedInterestFixture<GracePeriod>)
{
  auto i1 = this->makeSignedInterest(this->identity); // signed at 0ms
  auto i2 = this->makeSignedInterest(this->subIdentity); // signed at 0ms
  // ensure timestamps are exactly 0ms
  for (auto interest : {&i1, &i2}) {
    auto si = interest->getSignatureInfo();
    si->setTime(time::system_clock::now());
    interest->setSignatureInfo(*si);
  }

  VALIDATE_SUCCESS(i1, "Should succeed when validating at 0ms");
  this->rewindClockAfterValidation();

  this->advanceClocks(1_ms);
  VALIDATE_FAILURE(i2, "Should fail when validating at 1ms");
}

BOOST_AUTO_TEST_SUITE_END() // TimestampValidation

BOOST_AUTO_TEST_SUITE(SeqNumValidation)

// By default, sequence number validation is disabled
BOOST_AUTO_TEST_CASE(Disabled)
{
  auto i1 = makeSignedInterest(identity, WantAll); // signed at 0ms
  VALIDATE_SUCCESS(i1, "Should succeed");

  auto i2 = makeSignedInterest(identity, WantAll); // signed at +100ms
  // Set i2 to have same seq num as i1
  auto si2 = i2.getSignatureInfo();
  si2->setSeqNum(i2.getSignatureInfo()->getSeqNum());
  i2.setSignatureInfo(*si2);
  VALIDATE_SUCCESS(i2, "Should succeed");
}

class SeqNumValidationOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.shouldValidateSeqNums = true;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(MissingSeqNum,
                        ValidationPolicySignedInterestFixture<SeqNumValidationOptions>)
{
  auto i1 = makeSignedInterest(identity, InterestSigner::WantTime);
  VALIDATE_FAILURE(i1, "Should fail (sequence number missing");
}

BOOST_FIXTURE_TEST_CASE(SeqNumReorder,
                        ValidationPolicySignedInterestFixture<SeqNumValidationOptions>)
{
  auto i1 = makeSignedInterest(identity, WantAll); // seq num is i
  VALIDATE_SUCCESS(i1, "Should succeed");

  auto i2 = makeSignedInterest(identity, WantAll); // seq num is i+1
  auto si2 = i2.getSignatureInfo();
  si2->setSeqNum(i1.getSignatureInfo()->getSeqNum());
  i2.setSignatureInfo(*si2);
  VALIDATE_FAILURE(i2, "Should fail (sequence number reordered)");

  auto i3 = makeSignedInterest(identity, WantAll); // seq num is i+2
  VALIDATE_SUCCESS(i3, "Should succeed");
}

BOOST_AUTO_TEST_SUITE_END() // SeqNumValidation

BOOST_AUTO_TEST_SUITE(NonceValidation)

BOOST_AUTO_TEST_CASE(MissingNonce)
{
  auto i1 = makeSignedInterest(identity, InterestSigner::WantTime); // Specifically exclude nonce
  VALIDATE_FAILURE(i1, "Should fail (nonce missing)");
}

BOOST_AUTO_TEST_CASE(DuplicateNonce)
{
  auto i1 = makeSignedInterest(identity, WantAll);
  VALIDATE_SUCCESS(i1, "Should succeed");

  auto i2 = makeSignedInterest(identity, WantAll);
  auto si2 = i2.getSignatureInfo();
  si2->setNonce(i1.getSignatureInfo()->getNonce());
  i2.setSignatureInfo(*si2);
  VALIDATE_FAILURE(i2, "Should fail (duplicate nonce)");

  auto i3 = makeSignedInterest(identity, WantAll);
  // On the off chance that the generated nonce is identical to i1
  while (i3.getSignatureInfo()->getNonce() == i1.getSignatureInfo()->getNonce()) {
    i3 = makeSignedInterest(identity, WantAll);
  }
  VALIDATE_SUCCESS(i3, "Should succeed");
}

class DisabledNonceValidationOptions
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.shouldValidateNonces = false;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(Disabled,
                        ValidationPolicySignedInterestFixture<DisabledNonceValidationOptions>)
{
  auto i1 = makeSignedInterest(identity, WantAll ^ InterestSigner::WantNonce);
  VALIDATE_SUCCESS(i1, "Should succeed");

  // Ensure still works when a nonce is present
  auto i2 = makeSignedInterest(identity, WantAll);
  VALIDATE_SUCCESS(i2, "Should succeed");

  // Ensure a duplicate still succeeds
  auto i3 = makeSignedInterest(identity, WantAll);
  auto si3 = i3.getSignatureInfo();
  si3->setNonce(i2.getSignatureInfo()->getNonce());
  i3.setSignatureInfo(*si3);
  m_keyChain.sign(i3, signingByIdentity(identity).setSignedInterestFormat(SignedInterestFormat::V03)
                                                 .setSignatureInfo(*si3));
  VALIDATE_SUCCESS(i3, "Should succeed");
}

class NonceLimit2Options
{
public:
  static ValidationPolicySignedInterest::Options
  getOptions()
  {
    ValidationPolicySignedInterest::Options options;
    options.shouldValidateTimestamps = false;
    options.shouldValidateSeqNums = false;
    options.maxNonceRecordCount = 2;
    return options;
  }
};

BOOST_FIXTURE_TEST_CASE(NonceRecordLimit,
                        ValidationPolicySignedInterestFixture<NonceLimit2Options>)
{
  auto i1 = makeSignedInterest(identity, WantAll);
  VALIDATE_SUCCESS(i1, "Should succeed");

  auto i2 = makeSignedInterest(identity, WantAll);
  // On the off chance that the generated nonce is identical to i1
  while (i2.getSignatureInfo()->getNonce() == i1.getSignatureInfo()->getNonce()) {
    i2 = makeSignedInterest(identity, WantAll);
  }
  VALIDATE_SUCCESS(i2, "Should succeed");

  auto i3 = makeSignedInterest(identity, WantAll);
  auto si3 = i3.getSignatureInfo();
  si3->setNonce(i1.getSignatureInfo()->getNonce());
  i3.setSignatureInfo(*si3);
  m_keyChain.sign(i3, signingByIdentity(identity).setSignedInterestFormat(SignedInterestFormat::V03)
                                                 .setSignatureInfo(*si3));
  VALIDATE_FAILURE(i3, "Should fail (duplicate nonce)");

  // Pop i1's nonce off the list
  auto i4 = makeSignedInterest(identity, WantAll);
  // On the off chance that the generated nonce is identical to i1 or i2
  while (i4.getSignatureInfo()->getNonce() == i1.getSignatureInfo()->getNonce() ||
         i4.getSignatureInfo()->getNonce() == i2.getSignatureInfo()->getNonce()) {
    i4 = makeSignedInterest(identity, WantAll);
  }
  VALIDATE_SUCCESS(i4, "Should succeed");

  // Now i3 should succeed because i1's nonce has been popped off the list
  auto i5 = makeSignedInterest(identity, WantAll);
  auto si5 = i5.getSignatureInfo();
  si5->setNonce(i1.getSignatureInfo()->getNonce());
  i5.setSignatureInfo(*si5);
  m_keyChain.sign(i5, signingByIdentity(identity).setSignedInterestFormat(SignedInterestFormat::V03)
                                                 .setSignatureInfo(*si5));
  VALIDATE_SUCCESS(i5, "Should succeed");
}

BOOST_AUTO_TEST_SUITE_END() // NonceValidation

BOOST_AUTO_TEST_SUITE_END() // TestValidationPolicySignedInterest
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn
