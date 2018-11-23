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

#include "security/command-interest-signer.hpp"
#include "security/signing-helpers.hpp"

#include "boost-test.hpp"
#include "../identity-management-time-fixture.hpp"

namespace ndn {
namespace security {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestCommandInterestSigner, IdentityManagementTimeFixture)

BOOST_AUTO_TEST_CASE(Basic)
{
  addIdentity("/test");

  CommandInterestSigner signer(m_keyChain);
  Interest i1 = signer.makeCommandInterest("/hello/world");
  BOOST_CHECK_EQUAL(i1.getName().size(), 6);
  BOOST_CHECK_EQUAL(i1.getName().at(command_interest::POS_SIG_VALUE).blockFromValue().type(), tlv::SignatureValue);
  BOOST_CHECK_EQUAL(i1.getName().at(command_interest::POS_SIG_INFO).blockFromValue().type(), tlv::SignatureInfo);

  time::milliseconds timestamp = toUnixTimestamp(time::system_clock::now());
  BOOST_CHECK_EQUAL(i1.getName().at(command_interest::POS_TIMESTAMP).toNumber(), timestamp.count());

  Interest i2 = signer.makeCommandInterest("/hello/world/!", signingByIdentity("/test"));
  BOOST_CHECK_EQUAL(i2.getName().size(), 7);
  BOOST_CHECK_EQUAL(i2.getName().at(command_interest::POS_SIG_VALUE).blockFromValue().type(), tlv::SignatureValue);
  BOOST_CHECK_EQUAL(i2.getName().at(command_interest::POS_SIG_INFO).blockFromValue().type(), tlv::SignatureInfo);
  BOOST_CHECK_GT(i2.getName().at(command_interest::POS_TIMESTAMP), i1.getName().at(command_interest::POS_TIMESTAMP));
  BOOST_CHECK_NE(i2.getName().at(command_interest::POS_RANDOM_VAL),
                 i1.getName().at(command_interest::POS_RANDOM_VAL)); // this sometimes can fail

  advanceClocks(100_s);

  i2 = signer.makeCommandInterest("/hello/world/!");
  BOOST_CHECK_GT(i2.getName().at(command_interest::POS_TIMESTAMP), i1.getName().at(command_interest::POS_TIMESTAMP));
}

BOOST_AUTO_TEST_SUITE_END() // TestCommandInterestSigner
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
