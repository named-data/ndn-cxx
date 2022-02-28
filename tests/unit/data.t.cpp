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

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/transform/signer-filter.hpp"
#include "ndn-cxx/security/transform/step-source.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"
#include "ndn-cxx/util/sha256.hpp"
#include "ndn-cxx/util/string-helper.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestData)

const uint8_t CONTENT1[] = {0x53, 0x55, 0x43, 0x43, 0x45, 0x53, 0x53, 0x21};

const uint8_t DATA1[] = {
  0x06, 0xc5, // Data
    0x07, 0x14, // Name
      0x08, 0x05,
        0x6c, 0x6f, 0x63, 0x61, 0x6c,
      0x08, 0x03,
        0x6e, 0x64, 0x6e,
      0x08, 0x06,
        0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
    0x14, 0x04, // MetaInfo
      0x19, 0x02, // FreshnessPeriod
        0x27, 0x10,
    0x15, 0x08, // Content
      0x53, 0x55, 0x43, 0x43, 0x45, 0x53, 0x53, 0x21,
    0x16, 0x1b, // SignatureInfo
      0x1b, 0x01, // SignatureType
        0x01,
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
    0x17, 0x80, // SignatureValue
      0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31,
      0xec, 0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9,
      0xb3, 0xc6, 0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8,
      0xb3, 0x6a, 0x38, 0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6,
      0x4d, 0x10, 0x1d, 0xdc, 0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96,
      0x5e, 0xc0, 0x62, 0x0b, 0xcf, 0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41,
      0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9, 0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d,
      0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8, 0x55, 0xf6, 0x1c, 0x19, 0x0b,
      0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7, 0xaa, 0x0d, 0x14, 0x58,
      0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3, 0xfc, 0x90, 0x7a,
      0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1,
};

BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
  Data d;
  BOOST_CHECK_EQUAL(d.hasWire(), false);
  BOOST_CHECK_EQUAL(d.getName(), "/");
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), DEFAULT_FRESHNESS_PERIOD);
  BOOST_CHECK(!d.getFinalBlock());
  BOOST_CHECK_EQUAL(d.hasContent(), false);
  BOOST_CHECK_EQUAL(d.getContent().isValid(), false);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);
  BOOST_CHECK(!d.getSignatureInfo());
  BOOST_CHECK_EQUAL(d.getSignatureType(), -1);
  BOOST_CHECK(!d.getKeyLocator());
  BOOST_CHECK_EQUAL(d.getSignatureValue().isValid(), false);
}

class DataSigningKeyFixture
{
protected:
  DataSigningKeyFixture()
  {
    m_privKey.loadPkcs1(PRIVATE_KEY_DER);
    auto buf = m_privKey.derivePublicKey();
    m_pubKey.loadPkcs8(*buf);
  }

protected:
  security::transform::PrivateKey m_privKey;
  security::transform::PublicKey m_pubKey;

private:
  static const uint8_t PRIVATE_KEY_DER[632];
};

const uint8_t DataSigningKeyFixture::PRIVATE_KEY_DER[] = {
  0x30, 0x82, 0x02, 0x74, 0x02, 0x01, 0x00, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
  0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x04, 0x82, 0x02, 0x5e, 0x30, 0x82, 0x02, 0x5a, 0x02, 0x01,
  0x00, 0x02, 0x81, 0x81, 0x00, 0x9e, 0x06, 0x3e, 0x47, 0x85, 0xb2, 0x34, 0x37, 0xaa, 0x85, 0x47,
  0xac, 0x03, 0x24, 0x83, 0xb5, 0x9c, 0xa8, 0x05, 0x3a, 0x24, 0x1e, 0xeb, 0x89, 0x01, 0xbb, 0xe9,
  0x9b, 0xb2, 0xc3, 0x22, 0xac, 0x68, 0xe3, 0xf0, 0x6c, 0x02, 0xce, 0x68, 0xa6, 0xc4, 0xd0, 0xa7,
  0x06, 0x90, 0x9c, 0xaa, 0x1b, 0x08, 0x1d, 0x8b, 0x43, 0x9a, 0x33, 0x67, 0x44, 0x6d, 0x21, 0xa3,
  0x1b, 0x88, 0x9a, 0x97, 0x5e, 0x59, 0xc4, 0x15, 0x0b, 0xd9, 0x2c, 0xbd, 0x51, 0x07, 0x61, 0x82,
  0xad, 0xc1, 0xb8, 0xd7, 0xbf, 0x9b, 0xcf, 0x7d, 0x24, 0xc2, 0x63, 0xf3, 0x97, 0x17, 0xeb, 0xfe,
  0x62, 0x25, 0xba, 0x5b, 0x4d, 0x8a, 0xc2, 0x7a, 0xbd, 0x43, 0x8a, 0x8f, 0xb8, 0xf2, 0xf1, 0xc5,
  0x6a, 0x30, 0xd3, 0x50, 0x8c, 0xc8, 0x9a, 0xdf, 0xef, 0xed, 0x35, 0xe7, 0x7a, 0x62, 0xea, 0x76,
  0x7c, 0xbb, 0x08, 0x26, 0xc7, 0x02, 0x01, 0x11, 0x02, 0x81, 0x80, 0x04, 0xa5, 0xd4, 0xa7, 0xc0,
  0x2a, 0xe3, 0x6b, 0x0c, 0x8b, 0x73, 0x0c, 0x96, 0xae, 0x40, 0x1b, 0xee, 0x04, 0xf1, 0x18, 0x4c,
  0x5b, 0x43, 0x29, 0xad, 0x3a, 0x3b, 0x93, 0xa3, 0x60, 0x17, 0x9b, 0xa8, 0xbb, 0x68, 0xf4, 0x1e,
  0x33, 0x3f, 0x50, 0x32, 0xf7, 0x13, 0xf8, 0xa9, 0xe6, 0x7d, 0x79, 0x44, 0x00, 0xde, 0x72, 0xed,
  0xf2, 0x73, 0xfa, 0x7b, 0xae, 0x2a, 0x71, 0xc0, 0x40, 0xc8, 0x37, 0x6f, 0x38, 0xb2, 0x69, 0x1f,
  0xa8, 0x83, 0x7b, 0x42, 0x00, 0x73, 0x46, 0xe6, 0x4c, 0x91, 0x7f, 0x13, 0x06, 0x69, 0x06, 0xd8,
  0x3f, 0x22, 0x15, 0x75, 0xf6, 0xde, 0xcd, 0xb0, 0xbc, 0x66, 0x61, 0x91, 0x08, 0x9b, 0x2b, 0xb2,
  0x00, 0xa9, 0x67, 0x05, 0x39, 0x40, 0xb9, 0x37, 0x85, 0x88, 0x4f, 0x76, 0x79, 0x63, 0xc0, 0x88,
  0x3c, 0x86, 0xa8, 0x12, 0x94, 0x5f, 0xe4, 0x36, 0x3d, 0xea, 0xb9, 0x02, 0x41, 0x00, 0xb6, 0x2e,
  0xbb, 0xcd, 0x2f, 0x3a, 0x99, 0xe0, 0xa1, 0xa5, 0x44, 0x77, 0xea, 0x0b, 0xbe, 0x16, 0x95, 0x0e,
  0x64, 0xa7, 0x68, 0xd7, 0x4b, 0x15, 0x15, 0x23, 0xe2, 0x1e, 0x4e, 0x00, 0x2c, 0x22, 0x97, 0xae,
  0xb0, 0x74, 0xa6, 0x99, 0xd0, 0x5d, 0xb7, 0x1b, 0x10, 0x34, 0x13, 0xd2, 0x5f, 0x6e, 0x56, 0xad,
  0x85, 0x4a, 0xdb, 0xf0, 0x78, 0xbd, 0xf4, 0x8c, 0xb7, 0x9a, 0x3e, 0x99, 0xef, 0xb9, 0x02, 0x41,
  0x00, 0xde, 0x0d, 0xa7, 0x48, 0x75, 0x90, 0xad, 0x11, 0xa1, 0xac, 0xee, 0xcb, 0x41, 0x81, 0xc6,
  0xc8, 0x7f, 0xe7, 0x25, 0x94, 0xa1, 0x2a, 0x21, 0xa8, 0x57, 0xfe, 0x84, 0xf2, 0x5e, 0xb4, 0x96,
  0x35, 0xaf, 0xef, 0x2e, 0x7a, 0xf8, 0xda, 0x3f, 0xac, 0x8a, 0x3c, 0x1c, 0x9c, 0xbd, 0x44, 0xd6,
  0x90, 0xb5, 0xce, 0x1b, 0x12, 0xf9, 0x3b, 0x8c, 0x69, 0xf6, 0xa9, 0x02, 0x93, 0x48, 0x35, 0x0a,
  0x7f, 0x02, 0x40, 0x6b, 0x2a, 0x8c, 0x96, 0xd0, 0x7c, 0xd2, 0xfc, 0x9b, 0x52, 0x28, 0x46, 0x89,
  0xac, 0x8d, 0xef, 0x2a, 0x80, 0xef, 0xea, 0x01, 0x6f, 0x95, 0x93, 0xee, 0x51, 0x57, 0xd5, 0x97,
  0x4b, 0x65, 0x41, 0x86, 0x66, 0xc2, 0x26, 0x80, 0x1e, 0x3e, 0x55, 0x3e, 0x88, 0x63, 0xe2, 0x66,
  0x03, 0x47, 0x31, 0xd8, 0xa2, 0x4e, 0x68, 0x45, 0x24, 0x0a, 0xca, 0x17, 0x61, 0xd5, 0x69, 0xca,
  0x78, 0xab, 0x21, 0x02, 0x41, 0x00, 0x8f, 0xae, 0x7b, 0x4d, 0x00, 0xc7, 0x06, 0x92, 0xf0, 0x24,
  0x9a, 0x83, 0x84, 0xbd, 0x62, 0x81, 0xbc, 0x2c, 0x27, 0x60, 0x2c, 0x0c, 0x33, 0xe5, 0x66, 0x1d,
  0x28, 0xd9, 0x10, 0x1a, 0x7f, 0x4f, 0xea, 0x4f, 0x78, 0x6d, 0xb0, 0x14, 0xbf, 0xc9, 0xff, 0x17,
  0xd6, 0x47, 0x4d, 0x4a, 0xa8, 0xf4, 0x39, 0x67, 0x3e, 0xb1, 0xec, 0x8f, 0xf1, 0x71, 0xbd, 0xb8,
  0xa7, 0x50, 0x3d, 0xc7, 0xf7, 0xbb, 0x02, 0x40, 0x0d, 0x85, 0x32, 0x73, 0x9f, 0x0a, 0x33, 0x2f,
  0x4b, 0xa2, 0xbd, 0xd1, 0xb1, 0x42, 0xf0, 0x72, 0xa8, 0x7a, 0xc8, 0x15, 0x37, 0x1b, 0xde, 0x76,
  0x70, 0xce, 0xfd, 0x69, 0x20, 0x00, 0x4d, 0xc9, 0x4f, 0x35, 0x6f, 0xd1, 0x35, 0xa1, 0x04, 0x95,
  0x30, 0xe8, 0x3b, 0xd5, 0x03, 0x5a, 0x50, 0x21, 0x6d, 0xa0, 0x84, 0x39, 0xe9, 0x2e, 0x1e, 0xfc,
  0xe4, 0x82, 0x43, 0x20, 0x46, 0x7d, 0x0a, 0xb6
};

BOOST_AUTO_TEST_SUITE(Encode)

BOOST_AUTO_TEST_CASE(NotSigned)
{
  Data d;
  BOOST_CHECK_EXCEPTION(d.wireEncode(), tlv::Error, [] (const auto& e) {
    return e.what() == "Requested wire format, but Data has not been signed"s;
  });
}

BOOST_AUTO_TEST_CASE(Minimal)
{
  Data d;
  d.setSignatureInfo(SignatureInfo(tlv::DigestSha256));
  d.setSignatureValue(std::make_shared<Buffer>());
  BOOST_CHECK_EQUAL(d.wireEncode(), "060B 0700 1400 16031B0100 1700"_block);
}

BOOST_FIXTURE_TEST_CASE(Full, DataSigningKeyFixture)
{
  Data d("/local/ndn/prefix");
  d.setContentType(tlv::ContentType_Blob);
  d.setFreshnessPeriod(10_s);
  d.setContent(CONTENT1);

  SignatureInfo signatureInfo;
  signatureInfo.setSignatureType(tlv::SignatureSha256WithRsa);
  signatureInfo.setKeyLocator(Name("/test/key/locator"));
  d.setSignatureInfo(signatureInfo);

  OBufferStream sig;
  {
    namespace tr = security::transform;

    tr::StepSource input;
    input >> tr::signerFilter(DigestAlgorithm::SHA256, m_privKey) >> tr::streamSink(sig);

    input.write(d.getName().wireEncode());
    input.write(d.getMetaInfo().wireEncode());
    input.write(d.getContent());
    input.write(signatureInfo.wireEncode());
    input.end();
  }
  d.setSignatureValue(sig.buf());

  BOOST_TEST(d.wireEncode() == DATA1, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END() // Encode

class DecodeFixture
{
protected:
  DecodeFixture()
  {
    // initialize all elements to non-empty, to verify wireDecode clears them
    d.setName("/A");
    d.setContentType(tlv::ContentType_Key);
    d.setFreshnessPeriod(123_s);
    d.setFinalBlock(name::Component::fromNumber(42));
    d.setContent("1504C0C1C2C3"_block);
    d.setSignatureInfo(SignatureInfo("160A 1B0101 1C050703080142"_block));
    d.setSignatureValue(fromHex("B48F1707A3BCA3CFC5F32DE51D9B46C32D7D262A21544EBDA88C3B415D637503"
                                "FC9BEF20F88202A56AF9831E0D30205FD4154B08502BCDEE860267A5C3E03D8E"
                                "A6CB74BE391C01E0A57B991B4404FC11B7D777F1B700A4B65F201118CF1840A8"
                                "30A2A7C17DB4B7A8777E58515121AF9E2498627F8475414CDFD9801B8152AD5B"));
  }

protected:
  Data d;
};

BOOST_FIXTURE_TEST_SUITE(Decode, DecodeFixture)

BOOST_AUTO_TEST_CASE(NotData)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode("4202CAFE"_block), tlv::Error, [] (const auto& e) {
    return e.what() == "Expecting Data element, but TLV has type 66"s;
  });
}

BOOST_AUTO_TEST_CASE(Minimal)
{
  d.wireDecode("062C 0703(080144) 1603(1B0100) "
               "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block);
  BOOST_CHECK_EQUAL(d.getName(), "/D");
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 0_ms);
  BOOST_CHECK_EQUAL(d.getFinalBlock().has_value(), false);
  BOOST_CHECK_EQUAL(d.hasContent(), false);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);
  BOOST_CHECK_EQUAL(d.getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK_EQUAL(d.getKeyLocator().has_value(), false);
  BOOST_CHECK_EQUAL(d.getSignatureValue().value_size(), 32);

  // encode without modification: retain original wire encoding
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  BOOST_CHECK_EQUAL(d.wireEncode().value_size(), 44);

  // modify then re-encode
  d.setName("/E");
  BOOST_CHECK_EQUAL(d.hasWire(), false);
  BOOST_CHECK_EQUAL(d.wireEncode(),
                    "062E 0703(080145) 1400 1603(1B0100) "
                    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block);
}

BOOST_AUTO_TEST_CASE(MinimalEmptyName)
{
  d.wireDecode("0609 0700 1603(1B0100) 1700"_block);
  BOOST_CHECK_EQUAL(d.getName(), "/"); // empty Name is allowed in Data
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 0_ms);
  BOOST_CHECK_EQUAL(d.getFinalBlock().has_value(), false);
  BOOST_CHECK_EQUAL(d.hasContent(), false);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);
  BOOST_CHECK_EQUAL(d.getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK_EQUAL(d.getKeyLocator().has_value(), false);
  BOOST_CHECK_EQUAL(d.getSignatureValue().value_size(), 0);
}

BOOST_AUTO_TEST_CASE(Full)
{
  d.wireDecode(Block(DATA1));
  BOOST_CHECK_EQUAL(d.getName(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 10_s);
  BOOST_CHECK_EQUAL(d.getFinalBlock().has_value(), false);
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(readString(d.getContent()), "SUCCESS!");
  BOOST_CHECK_EQUAL(d.getSignatureType(), tlv::SignatureSha256WithRsa);
  BOOST_REQUIRE(d.getKeyLocator().has_value());
  BOOST_CHECK_EQUAL(d.getKeyLocator()->getName(), "/test/key/locator");
  BOOST_CHECK_EQUAL(d.getSignatureValue().value_size(), 128);
}

BOOST_AUTO_TEST_CASE(UnrecognizedNonCriticalElements)
{
  d.wireDecode("063A 0703(080144) FC00 1400 FC00 1500 FC00 1603(1B0100) FC00 "
               "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76 FC00"_block);
  BOOST_CHECK_EQUAL(d.getName(), "/D");
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 0_ms);
  BOOST_CHECK_EQUAL(d.getFinalBlock().has_value(), false);
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);
  BOOST_CHECK_EQUAL(d.getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK_EQUAL(d.getKeyLocator().has_value(), false);
  BOOST_CHECK_EQUAL(d.getSignatureValue().value_size(), 32);

  // encode without modification: retain original wire encoding
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  BOOST_CHECK_EQUAL(d.wireEncode().value_size(), 58);

  // modify then re-encode
  d.setName("/E");
  BOOST_CHECK_EQUAL(d.hasWire(), false);
  BOOST_CHECK_EQUAL(d.wireEncode(),
                    "0630 0703080145 1400 1500 16031B0100 "
                    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block);
}

BOOST_AUTO_TEST_CASE(CriticalElementOutOfOrder)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode(
    "0630 1400 0703080145 1500 16031B0100 "
    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
  BOOST_CHECK_EXCEPTION(d.wireDecode(
    "0630 0703080145 1500 1400 16031B0100 "
    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "MetaInfo element is out of order"s; });
  BOOST_CHECK_EXCEPTION(d.wireDecode(
    "0630 0703080145 1400 16031B0100 1500 "
    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "Content element is out of order"s; });
  BOOST_CHECK_EXCEPTION(d.wireDecode(
    "0630 0703080145 1400 1500 "
    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76 16031B0100"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "SignatureInfo element is out of order"s; });
  BOOST_CHECK_EXCEPTION(d.wireDecode(
    "0652 0703080145 1400 1500 16031B0100 "
    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"
    "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "SignatureValue element is out of order"s; });
}

BOOST_AUTO_TEST_CASE(MissingName)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode("0607 16031B0100 1700"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
}

BOOST_AUTO_TEST_CASE(MissingSignatureInfo)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode("0607 0703080144 1700"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "SignatureInfo element is missing"s; });
}

BOOST_AUTO_TEST_CASE(MissingSignatureValue)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode("0607 0700 16031B0100"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "SignatureValue element is missing"s; });
}

BOOST_AUTO_TEST_CASE(UnrecognizedNonCriticalElementBeforeName)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode(
                          "062E FC00 0703080144 16031B0100 "
                          "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block),
                        tlv::Error,
                        [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
}

BOOST_AUTO_TEST_CASE(UnrecognizedCriticalElement)
{
  BOOST_CHECK_EXCEPTION(d.wireDecode(
                          "0632 0703080145 FB00 1400 1500 16031B0100 "
                          "1720612A79399E60304A9F701C1ECAC7956BF2F1B046E6C6F0D6C29B3FE3A29BAD76"_block),
                        tlv::Error,
                        [] (const auto& e) { return e.what() == "Unrecognized element of critical type 251"s; });
}

BOOST_AUTO_TEST_SUITE_END() // Decode

BOOST_FIXTURE_TEST_CASE(FullName, KeyChainFixture)
{
  Data d(Name("/local/ndn/prefix"));
  d.setContentType(tlv::ContentType_Blob);
  d.setFreshnessPeriod(10_s);
  d.setContent(CONTENT1);
  BOOST_CHECK_THROW(d.getFullName(), Data::Error); // FullName is unavailable without signing

  m_keyChain.sign(d);
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  Name fullName = d.getFullName(); // FullName is available after signing

  BOOST_CHECK_EQUAL(d.getName().size() + 1, fullName.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(d.getName().begin(), d.getName().end(),
                                fullName.begin(), fullName.end() - 1);
  BOOST_CHECK_EQUAL(fullName.get(-1).value_size(), util::Sha256::DIGEST_SIZE);

  // FullName should be cached, so value() pointer points to same memory location
  BOOST_CHECK_EQUAL(fullName.get(-1).value(), d.getFullName().get(-1).value());

  d.setFreshnessPeriod(100_s); // invalidates FullName
  BOOST_CHECK_THROW(d.getFullName(), Data::Error);

  Data d1(Block{DATA1});
  BOOST_CHECK_EQUAL(d1.getFullName(),
    "/local/ndn/prefix/"
    "sha256digest=28bad4b5275bd392dbb670c75cf0b66f13f7942b21e80f55c0e86b374753a548");
}

BOOST_AUTO_TEST_CASE(SetName)
{
  Data d;
  d.setName("/first");
  BOOST_CHECK_EQUAL(d.getName(), "/first");
  BOOST_CHECK_EQUAL(d.hasWire(), false);

  d.setSignatureInfo(SignatureInfo(tlv::DigestSha256));
  d.setSignatureValue(std::make_shared<Buffer>());
  d.wireEncode();
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  d.setName("/first");
  BOOST_CHECK_EQUAL(d.getName(), "/first");
  BOOST_CHECK_EQUAL(d.hasWire(), true);

  d.setName("/second");
  BOOST_CHECK_EQUAL(d.getName(), "/second");
  BOOST_CHECK_EQUAL(d.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(SetContentType)
{
  Data d;
  d.setContentType(tlv::ContentType_Key);
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Key);
  BOOST_CHECK_EQUAL(d.hasWire(), false);

  d.setSignatureInfo(SignatureInfo(tlv::DigestSha256));
  d.setSignatureValue(std::make_shared<Buffer>());
  d.wireEncode();
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  d.setContentType(tlv::ContentType_Key);
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_Key);
  BOOST_CHECK_EQUAL(d.hasWire(), true);

  d.setContentType(tlv::ContentType_PrefixAnn);
  BOOST_CHECK_EQUAL(d.getContentType(), tlv::ContentType_PrefixAnn);
  BOOST_CHECK_EQUAL(d.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(SetFreshnessPeriod)
{
  Data d;
  d.setFreshnessPeriod(15_min);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 15_min);
  BOOST_CHECK_EQUAL(d.hasWire(), false);

  d.setSignatureInfo(SignatureInfo(tlv::DigestSha256));
  d.setSignatureValue(std::make_shared<Buffer>());
  d.wireEncode();
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  d.setFreshnessPeriod(15_min);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 15_min);
  BOOST_CHECK_EQUAL(d.hasWire(), true);

  d.setFreshnessPeriod(1_h);
  BOOST_CHECK_EQUAL(d.getFreshnessPeriod(), 1_h);
  BOOST_CHECK_EQUAL(d.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(SetFinalBlock)
{
  Data d;
  d.setFinalBlock(name::Component("foo"));
  BOOST_CHECK(d.getFinalBlock() == name::Component("foo"));
  BOOST_CHECK_EQUAL(d.hasWire(), false);

  d.setSignatureInfo(SignatureInfo(tlv::DigestSha256));
  d.setSignatureValue(std::make_shared<Buffer>());
  d.wireEncode();
  BOOST_CHECK_EQUAL(d.hasWire(), true);
  d.setFinalBlock(name::Component("foo"));
  BOOST_CHECK(d.getFinalBlock() == name::Component("foo"));
  BOOST_CHECK_EQUAL(d.hasWire(), true);

  d.setFinalBlock(name::Component("bar"));
  BOOST_CHECK(d.getFinalBlock() == name::Component("bar"));
  BOOST_CHECK_EQUAL(d.hasWire(), false);
}

BOOST_AUTO_TEST_CASE(SetContent)
{
  Data d;
  BOOST_CHECK_EQUAL(d.hasContent(), false);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Invalid);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);

  // Block overload, used directly as Content
  const uint8_t direct[] = {0xca, 0xfe};
  d.setContent("1502CAFE"_block);
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Content);
  BOOST_CHECK_EQUAL_COLLECTIONS(d.getContent().value_begin(), d.getContent().value_end(),
                                direct, direct + sizeof(direct));

  // Block overload, nested inside Content element
  const uint8_t nested[] = {0x99, 0x02, 0xca, 0xfe};
  d.setContent(Block(nested));
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Content);
  BOOST_CHECK_EQUAL_COLLECTIONS(d.getContent().value_begin(), d.getContent().value_end(),
                                nested, nested + sizeof(nested));

  // Block overload, default constructed (invalid)
  BOOST_CHECK_THROW(d.setContent(Block{}), std::invalid_argument);

  // span overload
  d.setContent(nested);
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Content);
  BOOST_CHECK_EQUAL_COLLECTIONS(d.getContent().value_begin(), d.getContent().value_end(),
                                nested, nested + sizeof(nested));
  d.setContent(span<uint8_t>{});
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Content);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);

  // raw buffer overload (deprecated)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  BOOST_CHECK_THROW(d.setContent(nullptr, 1), std::invalid_argument);
#pragma GCC diagnostic pop

  // ConstBufferPtr overload
  d.setContent(std::make_shared<Buffer>(direct, sizeof(direct)));
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Content);
  BOOST_CHECK_EQUAL_COLLECTIONS(d.getContent().value_begin(), d.getContent().value_end(),
                                direct, direct + sizeof(direct));
  d.setContent(std::make_shared<Buffer>());
  BOOST_CHECK_EQUAL(d.hasContent(), true);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Content);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);
  BOOST_CHECK_THROW(d.setContent(nullptr), std::invalid_argument);

  // unset
  d.unsetContent();
  BOOST_CHECK_EQUAL(d.hasContent(), false);
  BOOST_CHECK_EQUAL(d.getContent().type(), tlv::Invalid);
  BOOST_CHECK_EQUAL(d.getContent().value_size(), 0);
}

BOOST_AUTO_TEST_CASE(SetSignatureValue)
{
  Data d;
  BOOST_CHECK_EQUAL(d.getSignatureValue().type(), tlv::Invalid);

  d.setSignatureValue(fromHex("FACADE"));
  BOOST_CHECK_EQUAL(d.getSignatureValue().type(), tlv::SignatureValue);
  BOOST_CHECK_EQUAL(d.getSignatureValue().value_size(), 3);

  d.setSignatureValue(std::make_shared<Buffer>()); // empty buffer
  BOOST_CHECK_EQUAL(d.getSignatureValue().type(), tlv::SignatureValue);
  BOOST_CHECK_EQUAL(d.getSignatureValue().value_size(), 0);

  BOOST_CHECK_THROW(d.setSignatureValue(nullptr), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(ExtractSignedRanges, KeyChainFixture)
{
  Data d1("/test/prefix");
  m_keyChain.sign(d1);
  auto ranges1 = d1.extractSignedRanges();
  BOOST_REQUIRE_EQUAL(ranges1.size(), 1);
  const Block& wire1 = d1.wireEncode();
  const auto& sigInfoWire1 = wire1.find(tlv::SignatureInfo);
  BOOST_REQUIRE(sigInfoWire1 != wire1.elements_end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges1.front().begin(), ranges1.front().end(),
                                wire1.value_begin(), sigInfoWire1->value_end());

  // Test with decoded Data and ensure excludes elements after SignatureValue
  const uint8_t WIRE[] = {
    0x06, 0x0B, // Data
          0x07, 0x00, // Name
          0x16, 0x03, // SignatureInfo
                0x1B, 0x01, // SignatureType
                      0x00,
          0x17, 0x00, // SignatureValue
          0xAA, 0x00 // Unrecognized non-critical element
  };
  Data d2(Block{WIRE});
  auto ranges2 = d2.extractSignedRanges();
  BOOST_REQUIRE_EQUAL(ranges2.size(), 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges2.front().begin(), ranges2.front().end(), &WIRE[2], &WIRE[9]);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  Data a;
  Data b;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setName("/A");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("/B");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("/A");
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setFreshnessPeriod(10_s);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setFreshnessPeriod(10_s);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  static const uint8_t someData[] = "someData";
  a.setContent(someData);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setContent(someData);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setSignatureInfo(SignatureInfo(tlv::SignatureSha256WithRsa));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setSignatureInfo(SignatureInfo(tlv::SignatureSha256WithRsa));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_CASE(Print)
{
  Data d1(Block{DATA1});
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(d1),
                    "Name: /local/ndn/prefix\n"
                    "MetaInfo: [ContentType: 0, FreshnessPeriod: 10000 milliseconds]\n"
                    "Content: [8 bytes]\n"
                    "Signature: [type: SignatureSha256WithRsa, length: 128]\n");

  Data d2("/foo");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(d2),
                    "Name: /foo\n"
                    "MetaInfo: [ContentType: 0]\n"
                    "Signature: [type: Unknown(65535), length: 0]\n");
}

BOOST_AUTO_TEST_SUITE_END() // TestData

} // namespace tests
} // namespace ndn
