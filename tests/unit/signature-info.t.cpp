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

#include "ndn-cxx/signature-info.hpp"

#include "tests/boost-test.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestSignatureInfo)

BOOST_AUTO_TEST_CASE(Constructor)
{
  SignatureInfo info;
  BOOST_CHECK_EQUAL(info.getSignatureType(), -1);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), false);
  BOOST_CHECK_THROW(info.getKeyLocator(), SignatureInfo::Error);
  BOOST_CHECK_THROW(info.getValidityPeriod(), SignatureInfo::Error);
  BOOST_CHECK(!info.getNonce());
  BOOST_CHECK(!info.getTime());
  BOOST_CHECK(!info.getSeqNum());

  SignatureInfo sha256Info(tlv::DigestSha256);
  BOOST_CHECK_EQUAL(sha256Info.getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK_THROW(sha256Info.getKeyLocator(), SignatureInfo::Error);
  BOOST_CHECK(!info.getNonce());
  BOOST_CHECK(!info.getTime());
  BOOST_CHECK(!info.getSeqNum());

  KeyLocator keyLocator("/test/key/locator");
  SignatureInfo sha256RsaInfo(tlv::SignatureSha256WithRsa, keyLocator);
  BOOST_CHECK_EQUAL(sha256RsaInfo.getSignatureType(), tlv::SignatureSha256WithRsa);
  BOOST_CHECK_EQUAL(sha256RsaInfo.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(sha256RsaInfo.getKeyLocator().getName(), Name("/test/key/locator"));
  BOOST_CHECK_THROW(sha256RsaInfo.getValidityPeriod(), SignatureInfo::Error);
  BOOST_CHECK(!info.getNonce());
  BOOST_CHECK(!info.getTime());
  BOOST_CHECK(!info.getSeqNum());
}

BOOST_AUTO_TEST_CASE(SignatureType)
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

// We must name this test case differently to avoid naming conflicts
BOOST_AUTO_TEST_CASE(KeyLocatorField)
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

BOOST_AUTO_TEST_CASE(SignatureNonce)
{
  SignatureInfo info(tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK(!info.getNonce());
  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  std::vector<uint8_t> nonce{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  info.setNonce(nonce);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(info.getNonce() == nonce);

  info.wireEncode();
  info.setNonce(nonce);
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  nonce[2] = 0xFF;
  info.setNonce(nonce);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(info.getNonce() == nonce);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setNonce(nullopt);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(!info.getNonce());
}

BOOST_AUTO_TEST_CASE(SignatureTime)
{
  SignatureInfo info(tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK(!info.getTime());
  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  time::system_clock::time_point timePoint(1590169108480_ms);
  info.setTime(timePoint);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(info.getTime() == timePoint);

  info.wireEncode();
  info.setTime(timePoint);
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setTime(timePoint + 2_s);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(info.getTime() == timePoint + 2_s);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setTime(nullopt);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(!info.getTime());
}

BOOST_AUTO_TEST_CASE(SignatureSeqNum)
{
  SignatureInfo info(tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK(!info.getSeqNum());
  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setSeqNum(256);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(info.getSeqNum() == 256UL);

  info.wireEncode();
  info.setSeqNum(256);
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setSeqNum(512);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(info.getSeqNum() == 512UL);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.setSeqNum(nullopt);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(!info.getSeqNum());
}

const uint8_t sigInfoDataRsa[] = {
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
          0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
};

const uint8_t sigInfoInterestRsa[] = {
  0x2c, 0x33, // InterestSignatureInfo
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
    0x26, 0x08, // SignatureNonce
      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x28, 0x08, // SignatureTime
      0x00, 0x00, 0x01, 0x72, 0x3d, 0x77, 0x00, 0x00,
    0x2a, 0x02, // SignatureSeqNum
      0x10, 0x20,
};

const uint8_t sigInfoDataEcdsa[] = {
  0x16, 0x1b, // SignatureInfo
    0x1b, 0x01, // SignatureType
      0x03, // Sha256WithEcdsa
    0x1c, 0x16, // KeyLocator
      0x07, 0x14, // Name
        0x08, 0x04,
          0x74, 0x65, 0x73, 0x74,
        0x08, 0x03,
          0x6b, 0x65, 0x79,
        0x08, 0x07,
          0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
};

BOOST_AUTO_TEST_CASE(EncodeDecode)
{
  const KeyLocator keyLocator("/test/key/locator");

  // RSA
  SignatureInfo info(tlv::SignatureSha256WithRsa, keyLocator);

  // Encode as (Data)SignatureInfo
  auto encodedData = info.wireEncode(SignatureInfo::Type::Data);

  BOOST_CHECK_EQUAL_COLLECTIONS(sigInfoDataRsa, sigInfoDataRsa + sizeof(sigInfoDataRsa),
                                encodedData.begin(), encodedData.end());

  // Decode as (Data)SignatureInfo
  info = SignatureInfo(Block(sigInfoDataRsa), SignatureInfo::Type::Data);

  BOOST_CHECK_EQUAL(info.getSignatureType(), tlv::SignatureSha256WithRsa);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(info.getKeyLocator().getName(), Name("/test/key/locator"));
  BOOST_CHECK(!info.getNonce());
  BOOST_CHECK(!info.getTime());
  BOOST_CHECK(!info.getSeqNum());

  // Encode as InterestSignatureInfo
  const std::vector<uint8_t> nonce{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  info.setNonce(nonce);
  const time::system_clock::time_point timePoint(1590169108480_ms);
  info.setTime(timePoint);
  info.setSeqNum(0x1020);
  auto encodedInterest = info.wireEncode(SignatureInfo::Type::Interest);

  BOOST_CHECK_EQUAL_COLLECTIONS(sigInfoInterestRsa, sigInfoInterestRsa + sizeof(sigInfoInterestRsa),
                                encodedInterest.begin(), encodedInterest.end());

  // Decode as InterestSignatureInfo
  info = SignatureInfo(Block(sigInfoInterestRsa), SignatureInfo::Type::Interest);

  BOOST_CHECK_EQUAL(info.getSignatureType(), tlv::SignatureSha256WithRsa);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(info.getKeyLocator().getName(), Name("/test/key/locator"));
  BOOST_CHECK(info.getNonce() == nonce);
  BOOST_CHECK(info.getTime() == timePoint);
  BOOST_CHECK(info.getSeqNum() == 0x1020UL);

  // ECDSA
  info = SignatureInfo(tlv::SignatureSha256WithEcdsa, keyLocator);

  // Encode as (Data)SignatureInfo
  auto encodedDataEcdsa = info.wireEncode(SignatureInfo::Type::Data);

  BOOST_CHECK_EQUAL_COLLECTIONS(sigInfoDataEcdsa, sigInfoDataEcdsa + sizeof(sigInfoDataEcdsa),
                                encodedDataEcdsa.begin(), encodedDataEcdsa.end());

  // Decode as (Data)SignatureInfo
  info = SignatureInfo(Block(sigInfoDataEcdsa), SignatureInfo::Type::Data);

  BOOST_CHECK_EQUAL(info.getSignatureType(), tlv::SignatureSha256WithEcdsa);
  BOOST_CHECK_EQUAL(info.hasKeyLocator(), true);
  BOOST_CHECK_EQUAL(info.getKeyLocator().getName(), Name("/test/key/locator"));
  BOOST_CHECK(!info.getNonce());
  BOOST_CHECK(!info.getTime());
  BOOST_CHECK(!info.getSeqNum());
}

BOOST_AUTO_TEST_CASE(DecodeError)
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
  Block errorBlock1(error1);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock1, SignatureInfo::Type::Data), tlv::Error);

  const uint8_t error2[] = {
    0x16, 0x05, // SignatureInfo
      0x1b, 0x01, // SignatureType
        0x01, // Sha256WithRsa
      0x83, 0x00, // Unrecognized critical TLV
  };
  Block errorBlock2(error2);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock2, SignatureInfo::Type::Data), tlv::Error);

  const uint8_t error3[] = {
    0x16, 0x00 // Empty SignatureInfo
  };
  Block errorBlock3(error3);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock3, SignatureInfo::Type::Data), tlv::Error);

  // Encoding is correct for SignatureInfo, but decoder is expecting InterestSignatureInfo
  const uint8_t error4[] = {
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
  Block errorBlock4(error4);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock4, SignatureInfo::Type::Interest), tlv::Error);

  // SignatureType and KeyLocator out-of-order
  const uint8_t error5[] = {
    0x2c, 0x1b, // SignatureInfo
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
      0x1b, 0x01, // SignatureType
        0x01, // Sha256WithRsa
  };
  Block errorBlock5(error5);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock5, SignatureInfo::Type::Interest), tlv::Error);

  // Repeated KeyLocator
  const uint8_t error6[] = {
    0x2c, 0x33, // SignatureInfo
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
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
  };
  Block errorBlock6(error6);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock6, SignatureInfo::Type::Interest), tlv::Error);

  // Zero-length SignatureNonce
  const uint8_t error7[] = {
    0x2c, 0x05, // SignatureInfo
      0x1b, 0x01, // SignatureType
        0x01, // Sha256WithRsa
      0x26, 0x00 // SignatureNonce
  };
  Block errorBlock7(error7);
  BOOST_CHECK_THROW(SignatureInfo(errorBlock7, SignatureInfo::Type::Interest), tlv::Error);
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
  BOOST_CHECK_THROW(info.getValidityPeriod(), SignatureInfo::Error);

  info.wireEncode();
  info.setValidityPeriod(vp1);
  BOOST_CHECK_EQUAL(info.getValidityPeriod(), vp1);
  BOOST_CHECK_EQUAL(info.hasWire(), false);

  // encode
  auto encoded = info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  BOOST_TEST(encoded == sigInfo, boost::test_tools::per_element());

  info.setValidityPeriod(vp1);
  BOOST_CHECK_EQUAL(info.hasWire(), true);

  // decode
  Block block(sigInfo);
  SignatureInfo info2(block, SignatureInfo::Type::Data);
  BOOST_CHECK_EQUAL(info2.getValidityPeriod(), vp1);
  BOOST_CHECK_EQUAL(info2.hasWire(), true);

  info2.setValidityPeriod(security::ValidityPeriod(notBefore, notBefore + 42_days));
  BOOST_CHECK_NE(info2.getValidityPeriod(), vp1);
  BOOST_CHECK(info2.getValidityPeriod().getPeriod() == std::make_pair(notBefore, notBefore + 42_days));
  BOOST_CHECK_EQUAL(info2.hasWire(), false);

  info2.wireEncode();
  BOOST_CHECK_EQUAL(info2.hasWire(), true);
  info2.setValidityPeriod(nullopt);
  BOOST_CHECK_THROW(info2.getValidityPeriod(), SignatureInfo::Error);
  BOOST_CHECK_EQUAL(info2.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(CustomTlvs)
{
  SignatureInfo info(tlv::SignatureSha256WithEcdsa);
  info.addCustomTlv("810101"_block);
  BOOST_CHECK(!info.getCustomTlv(0x82));
  BOOST_REQUIRE(info.getCustomTlv(0x81));
  BOOST_CHECK_EQUAL(info.getCustomTlv(0x81)->type(), 0x81);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.addCustomTlv("82020202"_block);
  BOOST_CHECK_EQUAL(info.hasWire(), false);

  info.wireEncode();
  BOOST_CHECK_EQUAL(info.hasWire(), true);
  info.removeCustomTlv(0x81);
  BOOST_CHECK_EQUAL(info.hasWire(), false);
  BOOST_CHECK(!info.getCustomTlv(0x81));
}

BOOST_AUTO_TEST_CASE(CustomTlvsEncoding) // Bug #3914
{
  SignatureInfo info1(tlv::SignatureSha256WithRsa);
  info1.addCustomTlv(makeStringBlock(102, "First"));
  info1.addCustomTlv(makeStringBlock(104, "Second"));
  info1.addCustomTlv(makeStringBlock(106, "Third"));

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info1),
                    "SignatureSha256WithRsa { 102 104 106 }");

  SignatureInfo info2;
  info2.wireDecode(info1.wireEncode(), SignatureInfo::Type::Data);
  BOOST_CHECK_EQUAL(info1, info2);

  const uint8_t infoBytes[] = {
    0x16, 0x19, // SignatureInfo
          0x1b, 0x01, 0x01, // SignatureType=1
          0x66, 0x05, 0x46, 0x69, 0x72, 0x73, 0x74, // 102 "First"
          0x68, 0x06, 0x53, 0x65, 0x63, 0x6f, 0x6e, 0x64, // 104 "Second"
          0x6a, 0x05, 0x54, 0x68, 0x69, 0x72, 0x64 // 106 "Third"
  };

  SignatureInfo info3(Block(infoBytes), SignatureInfo::Type::Data);
  BOOST_CHECK_EQUAL(info3, info1);
  BOOST_CHECK_EQUAL_COLLECTIONS(infoBytes, infoBytes + sizeof(infoBytes),
                                info1.wireEncode().begin(), info1.wireEncode().end());
}

BOOST_AUTO_TEST_CASE(OutputStream)
{
  SignatureInfo info;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info), "Invalid SignatureInfo");

  info.setSignatureType(tlv::DigestSha256);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info), "DigestSha256");

  info.setSignatureType(tlv::SignatureSha256WithRsa);
  info.setKeyLocator(KeyLocator("/test/key/locator"));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info),
                    "SignatureSha256WithRsa Name=/test/key/locator");

  info.setNonce(std::vector<uint8_t>{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
  info.setTime(time::system_clock::time_point(1590169108480_ms));
  info.setSeqNum(0x1020);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info),
                    "SignatureSha256WithRsa Name=/test/key/locator "
                    "{ Nonce=0102030405060708 Time=1590169108480 SeqNum=4128 }");

  info.setValidityPeriod(security::ValidityPeriod(time::getUnixEpoch(), time::getUnixEpoch() + 31_days));

  info.addCustomTlv("82020102"_block);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info),
                    "SignatureSha256WithRsa Name=/test/key/locator "
                    "{ Nonce=0102030405060708 Time=1590169108480 SeqNum=4128 "
                    "ValidityPeriod=(19700101T000000, 19700201T000000) 130 }");

  info.addCustomTlv("84020102"_block);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info),
                    "SignatureSha256WithRsa Name=/test/key/locator "
                    "{ Nonce=0102030405060708 Time=1590169108480 SeqNum=4128 "
                    "ValidityPeriod=(19700101T000000, 19700201T000000) 130 132 }");
}

BOOST_AUTO_TEST_SUITE_END() // TestSignatureInfo

} // namespace tests
} // namespace ndn
