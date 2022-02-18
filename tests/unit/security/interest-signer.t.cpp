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

#include "ndn-cxx/security/interest-signer.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"
#include "tests/unit/clock-fixture.hpp"

namespace ndn {
namespace security {
namespace tests {

using namespace ndn::tests;

class InterestSignerFixture : public ClockFixture, public KeyChainFixture
{
};

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestInterestSigner, InterestSignerFixture)

BOOST_AUTO_TEST_CASE(V02)
{
  m_keyChain.createIdentity("/test");

  InterestSigner signer(m_keyChain);
  Interest i1 = signer.makeCommandInterest("/hello/world");
  BOOST_REQUIRE_EQUAL(i1.getName().size(), 6);
  BOOST_TEST(i1.getName().at(command_interest::POS_SIG_VALUE).blockFromValue().type() == tlv::SignatureValue);
  BOOST_TEST(i1.getName().at(command_interest::POS_SIG_INFO).blockFromValue().type() == tlv::SignatureInfo);

  time::milliseconds timestamp = toUnixTimestamp(time::system_clock::now());
  BOOST_TEST(i1.getName().at(command_interest::POS_TIMESTAMP).toNumber() == timestamp.count());

  Interest i2 = signer.makeCommandInterest("/hello/world/!", signingByIdentity("/test"));
  BOOST_REQUIRE_EQUAL(i2.getName().size(), 7);
  BOOST_TEST(i2.getName().at(command_interest::POS_SIG_VALUE).blockFromValue().type() == tlv::SignatureValue);
  BOOST_TEST(i2.getName().at(command_interest::POS_SIG_INFO).blockFromValue().type() == tlv::SignatureInfo);
  // These doesn't play well with BOOST_TEST for some reason
  BOOST_CHECK_GT(i2.getName().at(command_interest::POS_TIMESTAMP),
                 i1.getName().at(command_interest::POS_TIMESTAMP));
  BOOST_CHECK_NE(i2.getName().at(command_interest::POS_RANDOM_VAL),
                 i1.getName().at(command_interest::POS_RANDOM_VAL)); // this sometimes can fail

  advanceClocks(100_s);

  i2 = signer.makeCommandInterest("/hello/world/!");
  // This doesn't play well with BOOST_TEST for some reason
  BOOST_CHECK_GT(i2.getName().at(command_interest::POS_TIMESTAMP),
                 i1.getName().at(command_interest::POS_TIMESTAMP));
}

BOOST_AUTO_TEST_CASE(V03)
{
  m_keyChain.createIdentity("/test");

  InterestSigner signer(m_keyChain);
  Interest i1("/hello/world");
  signer.makeSignedInterest(i1, SigningInfo(),
                            InterestSigner::SigningFlags::WantNonce |
                            InterestSigner::SigningFlags::WantTime);
  BOOST_TEST(i1.isSigned() == true);
  BOOST_TEST_REQUIRE(i1.getName().size() == 3);
  BOOST_TEST_REQUIRE(i1.getSignatureInfo().has_value());

  BOOST_TEST(i1.getSignatureInfo()->getNonce().has_value() == true);
  BOOST_TEST(*i1.getSignatureInfo()->getTime() == time::system_clock::now());
  BOOST_TEST(i1.getSignatureInfo()->getSeqNum().has_value() == false);

  Interest i2("/hello/world/!");
  signer.makeSignedInterest(i2, signingByIdentity("/test"),
                            InterestSigner::SigningFlags::WantNonce |
                            InterestSigner::SigningFlags::WantTime |
                            InterestSigner::SigningFlags::WantSeqNum);
  BOOST_TEST(i2.isSigned() == true);
  BOOST_REQUIRE_EQUAL(i2.getName().size(), 4);
  BOOST_REQUIRE(i2.getSignatureInfo());

  BOOST_TEST(*i2.getSignatureInfo()->getNonce() != *i1.getSignatureInfo()->getNonce());
  BOOST_TEST(*i2.getSignatureInfo()->getTime() > *i1.getSignatureInfo()->getTime());
  BOOST_TEST_REQUIRE(i2.getSignatureInfo()->getSeqNum().has_value() == true);

  advanceClocks(100_s);

  Interest i3("/hello/world/2");
  signer.makeSignedInterest(i3, SigningInfo(), InterestSigner::SigningFlags::WantSeqNum);
  BOOST_TEST(i3.isSigned() == true);
  BOOST_REQUIRE_EQUAL(i3.getName().size(), 4);
  BOOST_REQUIRE(i3.getSignatureInfo());

  BOOST_TEST(i3.getSignatureInfo()->getNonce().has_value() == false);
  BOOST_TEST(i3.getSignatureInfo()->getTime().has_value() == false);
  BOOST_TEST_REQUIRE(i3.getSignatureInfo()->getSeqNum().has_value() == true);
  BOOST_TEST(*i3.getSignatureInfo()->getSeqNum() > *i2.getSignatureInfo()->getSeqNum());

  signer.makeSignedInterest(i3);
  BOOST_TEST(i3.isSigned() == true);

  BOOST_TEST(*i3.getSignatureInfo()->getTime() == time::system_clock::now());

  BOOST_CHECK_THROW(signer.makeSignedInterest(i3, SigningInfo(), 0), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestInterestSigner
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
