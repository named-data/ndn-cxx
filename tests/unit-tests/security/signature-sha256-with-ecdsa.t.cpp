/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "security/signature-sha256-with-ecdsa.hpp"
#include "security/key-chain.hpp"
#include "security/validator.hpp"
#include "util/scheduler.hpp"
#include "identity-management-fixture.hpp"
#include "../unit-test-time-fixture.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace tests {

class SignatureSha256EcdsaTimeFixture : public UnitTestTimeFixture
                                      , public security::IdentityManagementFixture
{
public:
  SignatureSha256EcdsaTimeFixture()
    : scheduler(io)
  {
  }

public:
  Scheduler scheduler;
};

BOOST_FIXTURE_TEST_SUITE(SecuritySignatureSha256WithEcdsa, SignatureSha256EcdsaTimeFixture)

const uint8_t sigInfo[] = {
0x16, 0x1b, // SignatureInfo
  0x1b, 0x01, // SignatureType
    0x03,
  0x1c, 0x16, // KeyLocator
    0x07, 0x14, // Name
      0x08, 0x04,
        0x74, 0x65, 0x73, 0x74,
      0x08, 0x03,
        0x6b, 0x65, 0x79,
      0x08, 0x07,
        0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
};

const uint8_t sigValue[] = {
0x17, 0x40, // SignatureValue
  0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
  0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
  0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
  0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
  0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b
};


BOOST_AUTO_TEST_CASE(Decoding)
{
  Block sigInfoBlock(sigInfo, sizeof(sigInfo));
  Block sigValueBlock(sigValue, sizeof(sigValue));

  Signature sig(sigInfoBlock, sigValueBlock);
  BOOST_CHECK_NO_THROW(SignatureSha256WithEcdsa(sig));
  BOOST_CHECK_NO_THROW(sig.getKeyLocator());
}

BOOST_AUTO_TEST_CASE(Encoding)
{
  Name name("/test/key/locator");
  KeyLocator keyLocator(name);

  SignatureSha256WithEcdsa sig(keyLocator);

  BOOST_CHECK_NO_THROW(sig.getKeyLocator());

  const Block& encodeSigInfoBlock = sig.getInfo();

  Block sigInfoBlock(sigInfo, sizeof(sigInfo));

  BOOST_CHECK_EQUAL_COLLECTIONS(sigInfoBlock.wire(),
                                sigInfoBlock.wire() + sigInfoBlock.size(),
                                encodeSigInfoBlock.wire(),
                                encodeSigInfoBlock.wire() + encodeSigInfoBlock.size());

  sig.setKeyLocator(Name("/test/another/key/locator"));

  const Block& encodeSigInfoBlock2 = sig.getInfo();
  BOOST_CHECK(sigInfoBlock != encodeSigInfoBlock2);
}

BOOST_AUTO_TEST_CASE(DataSignature)
{
  Name identityName("/SecurityTestSignatureSha256WithEcdsa/DataSignature");
  BOOST_REQUIRE(addIdentity(identityName, EcdsaKeyParams()));
  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = m_keyChain.getPublicKeyFromTpm(
    m_keyChain.getDefaultKeyNameForIdentity(identityName)));

  Data testData("/SecurityTestSignatureSha256WithEcdsa/DataSignature/Data1");
  char content[5] = "1234";
  testData.setContent(reinterpret_cast<uint8_t*>(content), 5);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(testData,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identityName)));
  Block dataBlock(testData.wireEncode().wire(), testData.wireEncode().size());

  Data testData2;
  testData2.wireDecode(dataBlock);
  BOOST_CHECK(Validator::verifySignature(testData2, *publicKey));
}


BOOST_AUTO_TEST_CASE(InterestSignature)
{
  Name identityName("/SecurityTestSignatureSha256WithEcdsa/InterestSignature");
  BOOST_REQUIRE(addIdentity(identityName, EcdsaKeyParams()));
  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = m_keyChain.getPublicKeyFromTpm(
    m_keyChain.getDefaultKeyNameForIdentity(identityName)));


  Interest interest("/SecurityTestSignatureSha256WithEcdsa/InterestSignature/Interest1");
  Interest interest11("/SecurityTestSignatureSha256WithEcdsa/InterestSignature/Interest1");

  scheduler.scheduleEvent(time::milliseconds(100), [&] {
      BOOST_CHECK_NO_THROW(m_keyChain.sign(interest,
                                           security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                                 identityName)));
    });

  advanceClocks(time::milliseconds(100));
  scheduler.scheduleEvent(time::milliseconds(100), [&] {
      BOOST_CHECK_NO_THROW(m_keyChain.sign(interest11,
                                           security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                                 identityName)));
    });

  advanceClocks(time::milliseconds(100));

  time::system_clock::TimePoint timestamp1 =
    time::fromUnixTimestamp(
      time::milliseconds(interest.getName().get(signed_interest::POS_TIMESTAMP).toNumber()));

  time::system_clock::TimePoint timestamp2 =
    time::fromUnixTimestamp(
      time::milliseconds(interest11.getName().get(signed_interest::POS_TIMESTAMP).toNumber()));

  BOOST_CHECK_EQUAL(time::milliseconds(100), (timestamp2 - timestamp1));

  uint64_t nonce1 = interest.getName().get(signed_interest::POS_RANDOM_VAL).toNumber();
  uint64_t nonce2 = interest11.getName().get(signed_interest::POS_RANDOM_VAL).toNumber();
  BOOST_WARN_NE(nonce1, nonce2);

  Block interestBlock(interest.wireEncode().wire(), interest.wireEncode().size());

  Interest interest2;
  interest2.wireDecode(interestBlock);
  BOOST_CHECK(Validator::verifySignature(interest2, *publicKey));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
