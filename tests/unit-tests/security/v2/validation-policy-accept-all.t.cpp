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

#include "security/v2/validation-policy-accept-all.hpp"

#include "boost-test.hpp"
#include "validator-fixture.hpp"

#include <boost/mpl/vector.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)

class ValidationPolicyAcceptAllFixture : public ValidatorFixture<ValidationPolicyAcceptAll>
{
public:
  ValidationPolicyAcceptAllFixture()
  {
    identity = addIdentity("/Security/V2/TestValidationPolicyAcceptAll");
    // don't add trust anchors
  }

public:
  Identity identity;
};

BOOST_FIXTURE_TEST_SUITE(TestValidationPolicyAcceptAll, ValidationPolicyAcceptAllFixture)

typedef boost::mpl::vector<Interest, Data> Packets;

BOOST_AUTO_TEST_CASE_TEMPLATE(Validate, Packet, Packets)
{
  Packet unsignedPacket("/Security/V2/TestValidationPolicyAcceptAll/Sub/Packet");

  Packet packet = unsignedPacket;
  VALIDATE_SUCCESS(packet, "Should accept unsigned");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_SUCCESS(packet, "Should accept Sha256Digest signature");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingByIdentity(identity));
  VALIDATE_SUCCESS(packet, "Should accept signature while no trust anchors configured");
}

BOOST_AUTO_TEST_SUITE_END() // TestValidationPolicyAcceptAll
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
