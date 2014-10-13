/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "security/key-chain.hpp"
#include "security/validator.hpp"
#include "identity-management-fixture.hpp"
#include "boost-test.hpp"

namespace ndn {

BOOST_FIXTURE_TEST_SUITE(SecurityTestSignedInterest, security::IdentityManagementFixture)

BOOST_AUTO_TEST_CASE(SignVerifyInterest)
{
  Name identityName("/TestSignedInterest/SignVerify");
  identityName.appendVersion();
  BOOST_REQUIRE(addIdentity(identityName, RsaKeyParams()));
  Name certificateName = m_keyChain.getDefaultCertificateNameForIdentity(identityName);

  Interest interest("/TestSignedInterest/SignVerify/Interest1");
  BOOST_CHECK_NO_THROW(m_keyChain.signByIdentity(interest, identityName));

  usleep(100000);

  Interest interest11("/TestSignedInterest/SignVerify/Interest1");
  BOOST_CHECK_NO_THROW(m_keyChain.signByIdentity(interest11, identityName));

  time::system_clock::TimePoint timestamp1 =
    time::fromUnixTimestamp(
      time::milliseconds(interest.getName().get(signed_interest::POS_TIMESTAMP).toNumber()));

  time::system_clock::TimePoint timestamp2 =
    time::fromUnixTimestamp(
      time::milliseconds(interest11.getName().get(signed_interest::POS_TIMESTAMP).toNumber()));

  BOOST_CHECK_LT(time::milliseconds(100), (timestamp2 - timestamp1));

  uint64_t nonce1 = interest.getName().get(signed_interest::POS_RANDOM_VAL).toNumber();
  uint64_t nonce2 = interest11.getName().get(signed_interest::POS_RANDOM_VAL).toNumber();
  BOOST_CHECK_NE(nonce1, nonce2);

  Block interestBlock(interest.wireEncode().wire(), interest.wireEncode().size());

  Interest interest2;
  interest2.wireDecode(interestBlock);

  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = m_keyChain.getPublicKeyFromTpm(
    m_keyChain.getDefaultKeyNameForIdentity(identityName)));
  bool result = Validator::verifySignature(interest2, *publicKey);

  BOOST_CHECK_EQUAL(result, true);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
