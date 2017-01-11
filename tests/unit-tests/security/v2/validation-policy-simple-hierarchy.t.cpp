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

#include "security/v2/validation-policy-simple-hierarchy.hpp"

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
BOOST_FIXTURE_TEST_SUITE(TestValidationPolicySimpleHierarchy,
                         HierarchicalValidatorFixture<ValidationPolicySimpleHierarchy>)

typedef boost::mpl::vector<Interest, Data> Packets;

BOOST_AUTO_TEST_CASE_TEMPLATE(Validate, Packet, Packets)
{
  Packet unsignedPacket("/Security/V2/ValidatorFixture/Sub1/Sub2/Packet");

  Packet packet = unsignedPacket;
  VALIDATE_FAILURE(packet, "Unsigned");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_FAILURE(packet, "Policy doesn't accept Sha256Digest signature");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingByIdentity(identity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the anchor");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingByIdentity(subIdentity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the policy-compliant cert");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingByIdentity(otherIdentity));
  VALIDATE_FAILURE(packet, "Should fail, as signed by the policy-violating cert");

  packet = unsignedPacket;
  m_keyChain.sign(packet, signingByIdentity(subSelfSignedIdentity));
  VALIDATE_FAILURE(packet, "Should fail, because subSelfSignedIdentity is not a trust anchor");

  // TODO add checks with malformed packets
}

BOOST_AUTO_TEST_SUITE_END() // TestValidator
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
