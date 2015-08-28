/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "security/sec-rule-relative.hpp"
#include "identity-management-fixture.hpp"

namespace ndn {
namespace security {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestSecRuleRelative, IdentityManagementV1Fixture)

BOOST_AUTO_TEST_CASE(Basic)
{
  Name rsaIdentity("/SecurityTestSecRule/Basic/Rsa");
  addIdentity(rsaIdentity, RsaKeyParams());
  Name ecIdentity("/SecurityTestSecRule/Basic/Ec");
  addIdentity(ecIdentity, EcKeyParams());

  Name dataName("SecurityTestSecRule/Basic");
  Data rsaData(dataName);
  m_keyChain.sign(rsaData,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        rsaIdentity));
  Data ecData(dataName);
  m_keyChain.sign(ecData,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        ecIdentity));
  Data sha256Data(dataName);
  m_keyChain.sign(sha256Data, security::SigningInfo(security::SigningInfo::SIGNER_TYPE_SHA256));

  SecRuleRelative rule("^(<SecurityTestSecRule><Basic>)$",
                       "^(<SecurityTestSecRule><Basic>)<><KEY><><>$",
                       "==", "\\1", "\\1", true);
  BOOST_CHECK(rule.satisfy(rsaData));
  BOOST_CHECK(rule.satisfy(ecData));
  BOOST_CHECK_EQUAL(rule.satisfy(sha256Data), false);

  BOOST_CHECK(rule.matchSignerName(rsaData));
  BOOST_CHECK(rule.matchSignerName(ecData));
  BOOST_CHECK_EQUAL(rule.matchSignerName(sha256Data), false);
}

BOOST_AUTO_TEST_SUITE_END() // TestSecRuleRelative
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
