/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#include "ndn-cxx/signature-info.hpp"

#include "tests/boost-test.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestSignatureInfo)

const uint8_t sigInfoRsa[] = {
  0x16, 0x1b, // SignatureInfo
    0x1b, 0x01, // SignatureType
      0x01, // Sha256WithRsa
    0x1c, 0x16, // KeyLocator
      0x07, 0x14, // Name
        0x08, 0x04,
          0x74, 0x65, 0x73, 0x74,
        0x08, 0x03,
          0x6b, 0x65, 0x79,
        0x08, 0x07,
          0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
};

BOOST_AUTO_TEST_CASE(Constructor)
{
  SignatureInfo info;
  BOOST_CHECK_EQUAL(info.getSignatureType(), -1);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), false);
  BOOST_CHECK_THROW(info.getKeyLocator(), SignatureInfo::Error);
  BOOST_CHECK_EQUAL(info.hasValidityPeriod(), false);
  BOOST_CHECK_THROW(info.getValidityPeriod(), SignatureInfo::Error);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info), "Invalid SignatureInfo");

  SignatureInfo sha256Info(tlv::DigestSha256);
  BOOST_CHECK_EQUAL(sha256Info.getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK_EQUAL(sha256Info.hasKeyLocator(), false);
  BOOST_CHECK_THROW(sha256Info.getKeyLocator(), SignatureInfo::Error);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(sha256Info), "DigestSha256");

  KeyLocator keyLocator("/test/key/locator");
  SignatureInfo sha256RsaInfo(tlv::SignatureSha256WithRsa, keyLocator);
  BOOST_CHECK_EQUAL(sha256RsaInfo.getSignatureType(), tlv::SignatureSha256WithRsa);
  BOOST_CHECK_EQUAL(sha256RsaInfo.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(sha256RsaInfo.getKeyLocator().getName(), Name("/test/key/locator"));
  BOOST_CHECK_EQUAL(sha256RsaInfo.hasValidityPeriod(), false);
  BOOST_CHECK_THROW(sha256RsaInfo.getValidityPeriod(), SignatureInfo::Error);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(sha256RsaInfo),
                    "SignatureSha256WithRsa Name=/test/key/locator");

  auto encoded = sha256RsaInfo.wireEncode();
  Block sigInfoBlock(sigInfoRsa, sizeof(sigInfoRsa));
  BOOST_CHECK_EQUAL_COLLECTIONS(sigInfoBlock.wire(), sigInfoBlock.wire() + sigInfoBlock.size(),
                                encoded.wire(), encoded.wire() + encoded.size());

  sha256RsaInfo = SignatureInfo(sigInfoBlock);
  BOOST_CHECK_EQUAL(sha256RsaInfo.getSignatureType(), tlv::SignatureSha256WithRsa);
  BOOST_CHECK_EQUAL(sha256RsaInfo.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(sha256RsaInfo.getKeyLocator().getName(), Name("/test/key/locator"));
}

BOOST_AUTO_TEST_CASE(ConstructorError)
{
  const uint8_t error1[] = {
    0x15, 0x1b, // Wrong SignatureInfo (0x16, 0x1b)
      0x1b, 0x01, // SignatureType
        0x01, // Sha256WithRsa
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
  };
  Block errorBlock1(error1, sizeof(error1));
  BOOST_CHECK_THROW(SignatureInfo info(errorBlock1), tlv::Error);

  const uint8_t error2[] = {
    0x16, 0x01, // SignatureInfo
      0x01 // Wrong SignatureInfo value
  };
  Block errorBlock2(error2, sizeof(error2));
  BOOST_CHECK_THROW(SignatureInfo info(errorBlock2), tlv::Error);

  const uint8_t error3[] = {
    0x16, 0x01, // SignatureInfo
      0x1a, 0x01, // Wrong SignatureType (0x1b, 0x1b)
        0x01, // Sha256WithRsa
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
  };
  Block errorBlock3(error3, sizeof(error3));
  BOOST_CHECK_THROW(SignatureInfo info(errorBlock3), tlv::Error);

  const uint8_t error4[] = {
    0x16, 0x00 // Empty SignatureInfo
  };
  Block errorBlock4(error4, sizeof(error4));
  BOOST_CHECK_THROW(SignatureInfo info(errorBlock4), tlv::Error);

}

BOOST_AUTO_TEST_CASE(GetSetSignatureType)
{
  SignatureInfo info;
  BOOST_CHECK_EQUAL(info.getSignatureType(), -1);
  BOOST_CHECK_THROW(info.wireEncode(), SignatureInfo::Error);

  info.setSignatureType(tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK_EQUAL(info.getSignatureType(), tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK_EQUAL(info.hasWire(), false);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setSignatureType(tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK_EQUAL(info.hasWire(), true);

  info.setSignatureType(static_cast<tlv::SignatureTypeValue>(1234));
  BOOST_CHECK_EQUAL(info.getSignatureType(), 1234);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(GetSetKeyLocator)
{
  SignatureInfo info(tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), false);
  BOOST_CHECK_THROW(info.getKeyLocator(), SignatureInfo::Error);

  info.setKeyLocator(Name("/test/key/locator"));
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(info.getKeyLocator().getName(), "/test/key/locator");
  BOOST_CHECK_EQUAL(info.hasWire(), false);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setKeyLocator(Name("/test/key/locator"));
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setKeyLocator(Name("/another/key/locator"));
  BOOST_CHECK_EQUAL(info.hasWire(), false);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setKeyLocator(nullopt);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), false);
  BOOST_CHECK_THROW(info.getKeyLocator(), SignatureInfo::Error);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(ValidityPeriod)
{
  const uint8_t sigInfo[] = {
    0x16, 0x45, // SignatureInfo
      0x1b, 0x01, // SignatureType
        0x01, // Sha256WithRsa
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
      0xfd, 0x00, 0xfd, 0x26, // ValidityPeriod
        0xfd, 0x00, 0xfe, 0x0f, // NotBefore
          0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
          0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0xfd, 0x00, 0xff, 0x0f, // NotAfter
          0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
          0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
  };

  const auto notBefore = time::getUnixEpoch();
  const auto notAfter = notBefore + 1_day;
  security::ValidityPeriod vp1(notBefore, notAfter);

  SignatureInfo info(tlv::SignatureSha256WithRsa, KeyLocator("/test/key/locator"));
  BOOST_CHECK_EQUAL(info.hasValidityPeriod(), false);
  BOOST_CHECK_THROW(info.getValidityPeriod(), SignatureInfo::Error);

  info.wireEncode();
  info.setValidityPeriod(vp1);
  BOOST_CHECK_EQUAL(info.hasValidityPeriod(), true);
  BOOST_CHECK_EQUAL(info.getValidityPeriod(), vp1);
  BOOST_CHECK_EQUAL(info.hasWire(), false);

  // encode
  auto encoded = info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  BOOST_CHECK_EQUAL_COLLECTIONS(sigInfo, sigInfo + sizeof(sigInfo),
                                encoded.wire(), encoded.wire() + encoded.size());

  info.setValidityPeriod(vp1);
  BOOST_CHECK_EQUAL(info.hasWire(), true);

  // decode
  Block block(sigInfo, sizeof(sigInfo));
  SignatureInfo info2(block);
  BOOST_CHECK_EQUAL(info2.hasValidityPeriod(), true);
  BOOST_CHECK_EQUAL(info2.getValidityPeriod(), vp1);
  BOOST_CHECK_EQUAL(info2.hasWire(), true);

  info2.setValidityPeriod(security::ValidityPeriod(notBefore, notBefore + 42_days));
  BOOST_CHECK_EQUAL(info2.hasValidityPeriod(), true);
  BOOST_CHECK_NE(info2.getValidityPeriod(), vp1);
  BOOST_CHECK(info2.getValidityPeriod().getPeriod() == std::make_pair(notBefore, notBefore + 42_days));
  BOOST_CHECK_EQUAL(info2.hasWire(), false);

  info2.wireEncode();
  BOOST_CHECK_EQUAL(info2.hasWire(), true);
  info2.setValidityPeriod(nullopt);
  BOOST_CHECK_EQUAL(info2.hasValidityPeriod(), false);
  BOOST_CHECK_THROW(info2.getValidityPeriod(), SignatureInfo::Error);
  BOOST_CHECK_EQUAL(info2.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(OtherTlvs)
{
  SignatureInfo info(tlv::SignatureSha256WithEcdsa);
  info.appendTypeSpecificTlv("810101"_block);
  BOOST_CHECK_THROW(info.getTypeSpecificTlv(0x82), SignatureInfo::Error);
  BOOST_CHECK_EQUAL(info.getTypeSpecificTlv(0x81).type(), 0x81);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.appendTypeSpecificTlv("82020202"_block);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(OtherTlvsEncoding) // Bug #3914
{
  SignatureInfo info1(tlv::SignatureSha256WithRsa);
  info1.appendTypeSpecificTlv(makeStringBlock(101, "First"));
  info1.appendTypeSpecificTlv(makeStringBlock(102, "Second"));
  info1.appendTypeSpecificTlv(makeStringBlock(103, "Third"));

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info1), "SignatureSha256WithRsa { 101 102 103 }");

  SignatureInfo info2;
  info2.wireDecode(info1.wireEncode());
  BOOST_CHECK_EQUAL(info1, info2);

  const uint8_t infoBytes[] = {
    0x16, 0x19, // SignatureInfo
          0x1b, 0x01, 0x01, // SignatureType=1
          0x65, 0x05, 0x46, 0x69, 0x72, 0x73, 0x74, // 101 "First"
          0x66, 0x06, 0x53, 0x65, 0x63, 0x6f, 0x6e, 0x64, // 102 "Second"
          0x67, 0x05, 0x54, 0x68, 0x69, 0x72, 0x64 // 103 "Third"
  };

  SignatureInfo info3(Block(infoBytes, sizeof(infoBytes)));
  BOOST_CHECK_EQUAL(info3, info1);
  BOOST_CHECK_EQUAL_COLLECTIONS(infoBytes, infoBytes + sizeof(infoBytes),
                                info1.wireEncode().begin(), info1.wireEncode().end());
}

BOOST_AUTO_TEST_SUITE_END() // TestSignatureInfo

} // namespace tests
} // namespace ndn
