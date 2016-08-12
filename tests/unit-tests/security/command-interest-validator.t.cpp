/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "security/command-interest-validator.hpp"
#include "security/signing-helpers.hpp"
#include <boost/lexical_cast.hpp>

#include "boost-test.hpp"
#include "../../dummy-validator.hpp"
#include "../identity-management-time-fixture.hpp"
#include "../make-interest-data.hpp"

namespace ndn {
namespace security {
namespace tests {

using namespace ndn::tests;

class CommandInterestValidatorFixture : public IdentityManagementTimeFixture
{
protected:
  CommandInterestValidatorFixture()
  {
    this->initialize(CommandInterestValidator::Options{});
  }

  void
  initialize(const CommandInterestValidator::Options& options)
  {
    auto inner = make_unique<DummyValidator>();
    this->inner = inner.get();
    this->validator = make_unique<CommandInterestValidator>(std::move(inner), options);
  }

  Name
  makeIdentity(int identity)
  {
    Name name("/localhost/CommandInterestValidatorIdentity");
    name.appendSequenceNumber(identity);
    BOOST_REQUIRE(m_keyChain.doesIdentityExist(name) || this->addIdentity(name));
    return name;
  }

  shared_ptr<Interest>
  makeCommandInterest(int identity = 0)
  {
    auto interest = makeInterest("/CommandInterestPrefix");
    m_keyChain.sign(*interest, signingByIdentity(makeIdentity(identity)));
    BOOST_TEST_MESSAGE("makeCommandInterest " << interest->getName());
    return interest;
  }

  /** \brief check that validator accepts interest
   *  \param interest to be validated
   */
  void
  assertAccept(const Interest& interest)
  {
    BOOST_TEST_MESSAGE("assertAccept " << interest.getName());
    int nAccepts = 0;
    validator->validate(interest,
      [&nAccepts] (const shared_ptr<const Interest>&) { ++nAccepts; },
      [] (const shared_ptr<const Interest>&, const std::string& msg) {
        BOOST_ERROR("validation request should succeed but fails with: " << msg);
      });
    BOOST_CHECK_EQUAL(nAccepts, 1);
  }

  /** \brief check that validator rejects interest
   *  \param interest to be validated
   *  \param error if not NONE, further check the error code matches \p error
   *               if NONE, error code is not checked
   */
  void
  assertReject(const Interest& interest, CommandInterestValidator::ErrorCode error)
  {
    BOOST_TEST_MESSAGE("assertReject " << interest.getName());
    int nRejects = 0;
    validator->validate(interest,
      [] (const shared_ptr<const Interest>&) {
        BOOST_ERROR("validation request should fail but succeeds");
      },
      [&nRejects, error] (const shared_ptr<const Interest>&, const std::string& msg) {
        ++nRejects;
        if (error != CommandInterestValidator::ErrorCode::NONE) {
          BOOST_CHECK_EQUAL(msg, boost::lexical_cast<std::string>(error));
        }
      });
    BOOST_CHECK_EQUAL(nRejects, 1);
  }

protected:
  DummyValidator* inner;
  unique_ptr<CommandInterestValidator> validator;
};

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestCommandInterestValidator, CommandInterestValidatorFixture)

BOOST_AUTO_TEST_CASE(Normal)
{
  auto i1 = makeCommandInterest();
  assertAccept(*i1);

  advanceClocks(time::milliseconds(5));
  auto i2 = makeCommandInterest();
  assertAccept(*i2);

  advanceClocks(time::seconds(2));
  auto i3 = makeCommandInterest();
  assertAccept(*i3);
}

BOOST_AUTO_TEST_CASE(DataPassthru)
{
  auto d1 = makeData("/data");
  int nAccepts = 0;
  validator->validate(*d1,
    [&nAccepts] (const shared_ptr<const Data>&) { ++nAccepts; },
    [] (const shared_ptr<const Data>&, const std::string& msg) {
      BOOST_ERROR("validation request should succeed but fails with " << msg);
    });
  BOOST_CHECK_EQUAL(nAccepts, 1);
}

BOOST_AUTO_TEST_SUITE(Rejects)

BOOST_AUTO_TEST_CASE(NameTooShort)
{
  auto i1 = makeInterest("/name/too/short");
  assertReject(*i1, CommandInterestValidator::ErrorCode::NAME_TOO_SHORT);
}

BOOST_AUTO_TEST_CASE(BadTimestamp)
{
  auto i1 = makeCommandInterest();
  setNameComponent(*i1, signed_interest::POS_TIMESTAMP, "not-timestamp");
  assertReject(*i1, CommandInterestValidator::ErrorCode::BAD_TIMESTAMP);
}

BOOST_AUTO_TEST_CASE(BadSigInfo)
{
  auto i1 = makeCommandInterest();
  setNameComponent(*i1, signed_interest::POS_SIG_INFO, "not-SignatureInfo");
  assertReject(*i1, CommandInterestValidator::ErrorCode::BAD_SIG_INFO);
}

BOOST_AUTO_TEST_CASE(MissingKeyLocator)
{
  auto i1 = makeCommandInterest();
  SignatureInfo sigInfo;
  setNameComponent(*i1, signed_interest::POS_SIG_INFO,
                   sigInfo.wireEncode().begin(), sigInfo.wireEncode().end());
  assertReject(*i1, CommandInterestValidator::ErrorCode::MISSING_KEY_LOCATOR);
}

BOOST_AUTO_TEST_CASE(BadKeyLocatorType)
{
  auto i1 = makeCommandInterest();
  KeyLocator kl;
  kl.setKeyDigest(makeBinaryBlock(tlv::KeyDigest, "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD", 8));
  SignatureInfo sigInfo;
  sigInfo.setKeyLocator(kl);
  setNameComponent(*i1, signed_interest::POS_SIG_INFO,
                   sigInfo.wireEncode().begin(), sigInfo.wireEncode().end());
  assertReject(*i1, CommandInterestValidator::ErrorCode::BAD_KEY_LOCATOR_TYPE);
}

BOOST_AUTO_TEST_CASE(BadCertName)
{
  auto i1 = makeCommandInterest();
  KeyLocator kl;
  kl.setName("/bad/cert/name");
  SignatureInfo sigInfo;
  sigInfo.setKeyLocator(kl);
  setNameComponent(*i1, signed_interest::POS_SIG_INFO,
                   sigInfo.wireEncode().begin(), sigInfo.wireEncode().end());
  assertReject(*i1, CommandInterestValidator::ErrorCode::BAD_CERT_NAME);
}

BOOST_AUTO_TEST_CASE(InnerReject)
{
  inner->setResult(false);
  auto i1 = makeCommandInterest();
  assertReject(*i1, CommandInterestValidator::ErrorCode::NONE);
}

BOOST_AUTO_TEST_CASE(TimestampOutOfGracePositive)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(15);
  initialize(options);

  auto i1 = makeCommandInterest(); // signed at 0s
  advanceClocks(time::seconds(16)); // verifying at +16s
  assertReject(*i1, CommandInterestValidator::ErrorCode::TIMESTAMP_OUT_OF_GRACE);

  auto i2 = makeCommandInterest(); // signed at +16s
  assertAccept(*i2); // verifying at +16s
}

BOOST_AUTO_TEST_CASE(TimestampOutOfGraceNegative)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(15);
  initialize(options);

  auto i1 = makeCommandInterest(); // signed at 0s
  advanceClocks(time::seconds(1));
  auto i2 = makeCommandInterest(); // signed at +1s
  advanceClocks(time::seconds(1));
  auto i3 = makeCommandInterest(); // signed at +2s

  systemClock->advance(time::seconds(-18)); // verifying at -16s
  assertReject(*i1, CommandInterestValidator::ErrorCode::TIMESTAMP_OUT_OF_GRACE);

  // CommandInterestValidator should not remember i1's timestamp
  assertReject(*i2, CommandInterestValidator::ErrorCode::TIMESTAMP_OUT_OF_GRACE);

  // CommandInterestValidator should not remember i2's timestamp, and should treat i3 as initial
  advanceClocks(time::seconds(18)); // verifying at +2s
  assertAccept(*i3);
}

BOOST_AUTO_TEST_CASE(TimestampReorderEqual)
{
  auto i1 = makeCommandInterest(); // signed at 0s
  assertAccept(*i1);

  auto i2 = makeCommandInterest(); // signed at 0s
  setNameComponent(*i2, signed_interest::POS_TIMESTAMP,
                   i1->getName()[signed_interest::POS_TIMESTAMP]);
  assertReject(*i2, CommandInterestValidator::ErrorCode::TIMESTAMP_REORDER);

  advanceClocks(time::seconds(2));
  auto i3 = makeCommandInterest(); // signed at +2s
  assertAccept(*i3);
}

BOOST_AUTO_TEST_CASE(TimestampReorderNegative)
{
  auto i2 = makeCommandInterest(); // signed at 0ms
  advanceClocks(time::milliseconds(200));
  auto i3 = makeCommandInterest(); // signed at +200ms
  advanceClocks(time::milliseconds(900));
  auto i1 = makeCommandInterest(); // signed at +1100ms
  advanceClocks(time::milliseconds(300));
  auto i4 = makeCommandInterest(); // signed at +1400ms

  systemClock->advance(time::milliseconds(-300)); // verifying at +1100ms
  assertAccept(*i1);

  systemClock->advance(time::milliseconds(-1100)); // verifying at 0ms
  assertReject(*i2, CommandInterestValidator::ErrorCode::TIMESTAMP_REORDER);

  // CommandInterestValidator should not remember i2's timestamp
  advanceClocks(time::milliseconds(200)); // verifying at +200ms
  assertReject(*i3, CommandInterestValidator::ErrorCode::TIMESTAMP_REORDER);

  advanceClocks(time::milliseconds(1200)); // verifying at 1400ms
  assertAccept(*i4);
}

BOOST_AUTO_TEST_SUITE_END() // Rejects

BOOST_AUTO_TEST_SUITE(Options)

typedef boost::mpl::vector<
  boost::mpl::int_<0>,
  boost::mpl::int_<-1>
> GraceNonPositiveValues;

BOOST_AUTO_TEST_CASE_TEMPLATE(GraceNonPositive, VALUE, GraceNonPositiveValues)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(VALUE::value);
  initialize(options);

  auto i1 = makeCommandInterest(1); // signed at 0ms
  auto i2 = makeCommandInterest(2); // signed at 0ms
  for (auto interest : {i1, i2}) {
    setNameComponent(*interest, signed_interest::POS_TIMESTAMP,
                     name::Component::fromNumber(time::toUnixTimestamp(time::system_clock::now()).count()));
  } // ensure timestamps are exactly 0ms

  assertAccept(*i1); // verifying at 0ms

  advanceClocks(time::milliseconds(1));
  assertReject(*i2, CommandInterestValidator::ErrorCode::TIMESTAMP_OUT_OF_GRACE); // verifying at 1ms
}

BOOST_AUTO_TEST_CASE(TimestampsLimited)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(15);
  options.maxTimestamps = 3;
  initialize(options);

  auto i1 = makeCommandInterest(1);
  auto i2 = makeCommandInterest(2);
  auto i3 = makeCommandInterest(3);
  auto i00 = makeCommandInterest(0); // signed at 0s
  advanceClocks(time::seconds(1));
  auto i01 = makeCommandInterest(0); // signed at 1s
  advanceClocks(time::seconds(1));
  auto i02 = makeCommandInterest(0); // signed at 2s

  assertAccept(*i00);
  assertAccept(*i02);
  assertAccept(*i1);
  assertAccept(*i2);
  assertAccept(*i3); // forgets identity 0
  assertAccept(*i01); // accepted despite timestamp is reordered, because record has been evicted
}

BOOST_AUTO_TEST_CASE(TimestampsUnlimited)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(15);
  options.maxTimestamps = -1;
  initialize(options);

  auto i1 = makeCommandInterest(0); // signed at 0s
  advanceClocks(time::seconds(1));
  for (int identity = 0; identity < 20; ++identity) {
    auto i2 = makeCommandInterest(identity); // signed at +1s
    assertAccept(*i2);
  }
  assertReject(*i1, CommandInterestValidator::ErrorCode::TIMESTAMP_REORDER);
}

BOOST_AUTO_TEST_CASE(TimestampsDisabled)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(15);
  options.maxTimestamps = 0;
  initialize(options);

  auto i1 = makeCommandInterest(); // signed at 0s
  advanceClocks(time::seconds(1));
  auto i2 = makeCommandInterest(); // signed at +1s
  assertAccept(*i2);

  assertAccept(*i1); // accepted despite timestamp is reordered, because record isn't kept
}

BOOST_AUTO_TEST_CASE(TtlLimited)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(120);
  options.timestampTtl = time::seconds(300);
  initialize(options);

  auto i1 = makeCommandInterest(); // signed at 0s
  advanceClocks(time::seconds(240));
  auto i2 = makeCommandInterest(); // signed at +240s
  advanceClocks(time::seconds(120));
  auto i3 = makeCommandInterest(); // signed at +360s

  systemClock->advance(time::seconds(-360)); // rewind system clock to 0s
  assertAccept(*i1);
  assertAccept(*i3);

  advanceClocks(time::seconds(30), time::seconds(301)); // advance steady clock by 301s, and system clock to +301s
  assertAccept(*i2); // accepted despite timestamp is reordered, because record has been expired
}

BOOST_AUTO_TEST_CASE(TtlZero)
{
  CommandInterestValidator::Options options;
  options.gracePeriod = time::seconds(15);
  options.timestampTtl = time::seconds::zero();
  initialize(options);

  auto i1 = makeCommandInterest(); // signed at 0s
  advanceClocks(time::seconds(1));
  auto i2 = makeCommandInterest(); // signed at +1s
  assertAccept(*i2);

  assertAccept(*i1); // accepted despite timestamp is reordered, because record has been expired
}

BOOST_AUTO_TEST_SUITE_END() // Options

BOOST_AUTO_TEST_SUITE_END() // TestCommandInterestValidator
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
