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

#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/data.hpp"

#include "tests/test-common.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestInterest)

class DisableAutoCheckParametersDigest
{
public:
  DisableAutoCheckParametersDigest()
    : m_saved(Interest::getAutoCheckParametersDigest())
  {
    Interest::setAutoCheckParametersDigest(false);
  }

  ~DisableAutoCheckParametersDigest()
  {
    Interest::setAutoCheckParametersDigest(m_saved);
  }

private:
  bool m_saved;
};

BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
  Interest i;
  BOOST_CHECK_EQUAL(i.hasWire(), false);
  BOOST_CHECK_EQUAL(i.getName(), "/");
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i.hasNonce(), false);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.getApplicationParameters().isValid(), false);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK(i.getSignatureInfo() == nullopt);
  BOOST_CHECK_EQUAL(i.getSignatureValue().isValid(), false);
  BOOST_CHECK_EQUAL(i.isSigned(), false);
}

BOOST_AUTO_TEST_SUITE(Encode)

BOOST_AUTO_TEST_CASE(Basic)
{
  const uint8_t WIRE[] = {
    0x05, 0x1c, // Interest
          0x07, 0x14, // Name
                0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // GenericNameComponent
                0x08, 0x03, 0x6e, 0x64, 0x6e, // GenericNameComponent
                0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, // GenericNameComponent
          0x0a, 0x04, // Nonce
                0x01, 0x02, 0x03, 0x04,
  };

  Interest i1;
  i1.setName("/local/ndn/prefix");
  i1.setNonce(0x01020304);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  Block wire1 = i1.wireEncode();
  BOOST_TEST(wire1 == WIRE, boost::test_tools::per_element());

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i2.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i2.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i2.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i2.hasNonce(), true);
  BOOST_CHECK_EQUAL(i2.getNonce(), 0x01020304);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i2.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i2.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i2.getApplicationParameters().isValid(), false);
  BOOST_CHECK(i2.getSignatureInfo() == nullopt);
  BOOST_CHECK_EQUAL(i2.getSignatureValue().isValid(), false);
  BOOST_CHECK_EQUAL(i2.isSigned(), false);
}

BOOST_AUTO_TEST_CASE(WithParameters)
{
  const uint8_t WIRE[] = {
    0x05, 0x44, // Interest
          0x07, 0x36, // Name
                0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // GenericNameComponent
                0x08, 0x03, 0x6e, 0x64, 0x6e, // GenericNameComponent
                0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, // GenericNameComponent
                0x02, 0x20, // ParametersSha256DigestComponent
                      0xff, 0x91, 0x00, 0xe0, 0x4e, 0xaa, 0xdc, 0xf3, 0x06, 0x74, 0xd9, 0x80,
                      0x26, 0xa0, 0x51, 0xba, 0x25, 0xf5, 0x6b, 0x69, 0xbf, 0xa0, 0x26, 0xdc,
                      0xcc, 0xd7, 0x2c, 0x6e, 0xa0, 0xf7, 0x31, 0x5a,
          0x0a, 0x04, // Nonce
                0x00, 0x00, 0x00, 0x01,
          0x24, 0x04, // ApplicationParameters
                0xc0, 0xc1, 0xc2, 0xc3
  };

  Interest i1;
  i1.setName("/local/ndn/prefix");
  i1.setNonce(0x1);
  i1.setApplicationParameters("2404C0C1C2C3"_block);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  Block wire1 = i1.wireEncode();
  BOOST_TEST(wire1 == WIRE, boost::test_tools::per_element());

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(),
                    "/local/ndn/prefix/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i2.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i2.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i2.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i2.hasNonce(), true);
  BOOST_CHECK_EQUAL(i2.getNonce(), 0x1);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i2.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i2.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i2.getApplicationParameters(), "2404C0C1C2C3"_block);
  BOOST_CHECK(i2.getSignatureInfo() == nullopt);
  BOOST_CHECK_EQUAL(i2.getSignatureValue().isValid(), false);
  BOOST_CHECK_EQUAL(i2.isSigned(), false);
}

BOOST_AUTO_TEST_CASE(Full)
{
  const uint8_t WIRE[] = {
    0x05, 0x56, // Interest
          0x07, 0x36, // Name
                0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // GenericNameComponent
                0x08, 0x03, 0x6e, 0x64, 0x6e, // GenericNameComponent
                0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, // GenericNameComponent
                0x02, 0x20, // ParametersSha256DigestComponent
                      0xff, 0x91, 0x00, 0xe0, 0x4e, 0xaa, 0xdc, 0xf3, 0x06, 0x74, 0xd9, 0x80,
                      0x26, 0xa0, 0x51, 0xba, 0x25, 0xf5, 0x6b, 0x69, 0xbf, 0xa0, 0x26, 0xdc,
                      0xcc, 0xd7, 0x2c, 0x6e, 0xa0, 0xf7, 0x31, 0x5a,
          0x21, 0x00, // CanBePrefix
          0x12, 0x00, // MustBeFresh
          0x1e, 0x05, // ForwardingHint
                0x07, 0x03, 0x08, 0x01, 0x48,
          0x0a, 0x04, // Nonce
                0x4c, 0x1e, 0xcb, 0x4a,
          0x0c, 0x02, // InterestLifetime
                0x76, 0xa1,
          0x22, 0x01, // HopLimit
                0xdc,
          0x24, 0x04, // ApplicationParameters
                0xc0, 0xc1, 0xc2, 0xc3
  };

  Interest i1;
  i1.setName("/local/ndn/prefix");
  i1.setMustBeFresh(true);
  i1.setCanBePrefix(true);
  i1.setForwardingHint({"/H"});
  i1.setNonce(0x4c1ecb4a);
  i1.setInterestLifetime(30369_ms);
  i1.setHopLimit(220);
  i1.setApplicationParameters("2404C0C1C2C3"_block);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  Block wire1 = i1.wireEncode();
  BOOST_TEST(wire1 == WIRE, boost::test_tools::per_element());

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(),
                    "/local/ndn/prefix/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i2.getCanBePrefix(), true);
  BOOST_CHECK_EQUAL(i2.getMustBeFresh(), true);
  BOOST_TEST(i2.getForwardingHint() == std::vector<Name>({"/H"}), boost::test_tools::per_element());
  BOOST_CHECK_EQUAL(i2.hasNonce(), true);
  BOOST_CHECK_EQUAL(i2.getNonce(), 0x4c1ecb4a);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), 30369_ms);
  BOOST_CHECK_EQUAL(*i2.getHopLimit(), 220);
  BOOST_CHECK_EQUAL(i2.getApplicationParameters(), "2404C0C1C2C3"_block);
}

BOOST_AUTO_TEST_CASE(Signed)
{
  const uint8_t WIRE[] = {
    0x05, 0x77, // Interest
          0x07, 0x36, // Name
                0x08, 0x05, // GenericNameComponent
                      0x6c, 0x6f, 0x63, 0x61, 0x6c,
                0x08, 0x03, // GenericNameComponent
                      0x6e, 0x64, 0x6e,
                0x08, 0x06, // GenericNameComponent
                      0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
                0x02, 0x20, // ParametersSha256DigestComponent
                      0x6f, 0x29, 0x58, 0x60, 0x53, 0xee, 0x9f, 0xcc,
                      0xd8, 0xa4, 0x22, 0x12, 0x29, 0x25, 0x28, 0x7c,
                      0x0a, 0x18, 0x43, 0x5f, 0x40, 0x74, 0xc4, 0x0a,
                      0xbb, 0x0d, 0x5b, 0x30, 0xe4, 0xaa, 0x62, 0x20,
          0x12, 0x00, // MustBeFresh
          0x0a, 0x04, // Nonce
                0x4c, 0x1e, 0xcb, 0x4a,
          0x24, 0x04, // ApplicationParameters
                0xc0, 0xc1, 0xc2, 0xc3,
          0x2c, 0x0d, // InterestSignatureInfo
                0x1b, 0x01, // SignatureType
                      0x00,
                0x26, 0x08, // SignatureNonce
                      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
          0x2e, 0x20, // InterestSignatureValue
                0x12, 0x47, 0x1a, 0xe0, 0xf8, 0x72, 0x3a, 0xc1,
                0x15, 0x6c, 0x37, 0x0a, 0x38, 0x71, 0x1e, 0xbe,
                0xbf, 0x28, 0x17, 0xde, 0x9b, 0x2d, 0xd9, 0x4e,
                0x9b, 0x7e, 0x62, 0xf1, 0x17, 0xb8, 0x76, 0xc1,
  };

  SignatureInfo si(tlv::DigestSha256);
  std::vector<uint8_t> nonce{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  si.setNonce(nonce);
  Block sv("2E20 12471AE0F8723AC1156C370A38711EBEBF2817DE9B2DD94E9B7E62F117B876C1"_block);

  Interest i1(Block{WIRE});
  BOOST_CHECK_EQUAL(i1.getName(),
                    "/local/ndn/prefix/params-sha256=6f29586053ee9fccd8a422122925287c0a18435f4074c40abb0d5b30e4aa6220");
  BOOST_CHECK_EQUAL(i1.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i1.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(i1.hasNonce(), true);
  BOOST_CHECK_EQUAL(i1.getNonce(), 0x4c1ecb4a);
  BOOST_CHECK_EQUAL(i1.getSignatureInfo()->getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK(i1.getSignatureInfo()->getNonce() == nonce);
  BOOST_TEST(i1.getSignatureValue() == sv, boost::test_tools::per_element());
  BOOST_CHECK_EQUAL(i1.getApplicationParameters(), "2404C0C1C2C3"_block);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  // Reset wire
  BOOST_CHECK_EQUAL(i1.hasWire(), true);
  i1.setCanBePrefix(true);
  i1.setCanBePrefix(false);
  BOOST_CHECK_EQUAL(i1.hasWire(), false);

  BOOST_TEST(i1.wireEncode() == WIRE, boost::test_tools::per_element());

  Interest i2("/local/ndn/prefix");
  i2.setMustBeFresh(true);
  i2.setNonce(0x4c1ecb4a);
  i2.setApplicationParameters("2404C0C1C2C3"_block);
  i2.setSignatureInfo(si);
  i2.setSignatureValue(make_shared<Buffer>(sv.value_begin(), sv.value_end()));
  BOOST_CHECK_EQUAL(i2.isParametersDigestValid(), true);

  BOOST_TEST(i2.wireEncode() == WIRE, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(SignedApplicationElements)
{
  const uint8_t WIRE[] = {
    0x05, 0x8f, // Interest
          0x07, 0x36, // Name
                0x08, 0x05, // GenericNameComponent
                      0x6c, 0x6f, 0x63, 0x61, 0x6c,
                0x08, 0x03, // GenericNameComponent
                      0x6e, 0x64, 0x6e,
                0x08, 0x06, // GenericNameComponent
                      0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
                0x02, 0x20, // ParametersSha256DigestComponent
                      0xbc, 0x36, 0x30, 0xa4, 0xd6, 0x5e, 0x0d, 0xb5,
                      0x48, 0x3d, 0xfa, 0x0d, 0x28, 0xb3, 0x31, 0x2f,
                      0xca, 0xc1, 0xd4, 0x41, 0xec, 0x89, 0x61, 0xd4,
                      0x17, 0x5e, 0x61, 0x75, 0x17, 0x78, 0x10, 0x8e,
          0x12, 0x00, // MustBeFresh
          0x0a, 0x04, // Nonce
                0x4c, 0x1e, 0xcb, 0x4a,
          0x24, 0x04, // ApplicationParameters
                0xc0, 0xc1, 0xc2, 0xc3,
          0xfd, 0x01, 0xfe, 0x08, // Application-specific element (Type 510)
                0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
          0x2c, 0x0d, // InterestSignatureInfo
                0x1b, 0x01, // SignatureType
                      0x00,
                0x26, 0x08, // SignatureNonce
                      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
          0x2e, 0x20, // InterestSignatureValue
                0x12, 0x47, 0x1a, 0xe0, 0xf8, 0x72, 0x3a, 0xc1,
                0x15, 0x6c, 0x37, 0x0a, 0x38, 0x71, 0x1e, 0xbe,
                0xbf, 0x28, 0x17, 0xde, 0x9b, 0x2d, 0xd9, 0x4e,
                0x9b, 0x7e, 0x62, 0xf1, 0x17, 0xb8, 0x76, 0xc1,
          0xfd, 0x02, 0x00, 0x08, // Application-specific element (Type 512)
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
  };

  SignatureInfo si(tlv::DigestSha256);
  std::vector<uint8_t> nonce{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  si.setNonce(nonce);
  Block sv("2E20 12471AE0F8723AC1156C370A38711EBEBF2817DE9B2DD94E9B7E62F117B876C1"_block);

  Interest i1(Block{WIRE});
  BOOST_CHECK_EQUAL(i1.getName(),
                    "/local/ndn/prefix/params-sha256=bc3630a4d65e0db5483dfa0d28b3312fcac1d441ec8961d4175e61751778108e");
  BOOST_CHECK_EQUAL(i1.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i1.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(i1.hasNonce(), true);
  BOOST_CHECK_EQUAL(i1.getNonce(), 0x4c1ecb4a);
  BOOST_CHECK_EQUAL(i1.getSignatureInfo()->getSignatureType(), tlv::DigestSha256);
  BOOST_CHECK(i1.getSignatureInfo()->getNonce() == nonce);
  BOOST_TEST(i1.getSignatureValue() == sv, boost::test_tools::per_element());
  BOOST_CHECK_EQUAL(i1.getApplicationParameters(), "2404C0C1C2C3"_block);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  // Reset wire
  BOOST_CHECK_EQUAL(i1.hasWire(), true);
  i1.setCanBePrefix(true);
  i1.setCanBePrefix(false);
  BOOST_CHECK_EQUAL(i1.hasWire(), false);

  BOOST_TEST(i1.wireEncode() == WIRE, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(MissingApplicationParameters)
{
  Interest i;
  i.setName(Name("/A").appendParametersSha256DigestPlaceholder());
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
  BOOST_CHECK_EXCEPTION(i.wireEncode(), tlv::Error, [] (const auto& e) {
    return e.what() == "Interest without parameters must not have a ParametersSha256DigestComponent"s;
  });
}

BOOST_AUTO_TEST_CASE(MissingParametersSha256DigestComponent)
{
  // there's no way to create an Interest that fails this check via programmatic construction,
  // so we have to decode an invalid Interest and force reencoding

  DisableAutoCheckParametersDigest disabler;
  Interest i("050F 0703(080149) 0A04F000F000 2402CAFE"_block);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
  BOOST_CHECK_NO_THROW(i.wireEncode()); // this succeeds because it uses the cached wire encoding

  // trigger reencoding
  i.setNonce(42);
  // now the check fails while attempting to reencode
  BOOST_CHECK_EXCEPTION(i.wireEncode(), tlv::Error, [] (const auto& e) {
    return e.what() == "Interest with parameters must have a ParametersSha256DigestComponent"s;
  });
}

BOOST_AUTO_TEST_SUITE_END() // Encode

class DecodeFixture
{
protected:
  DecodeFixture()
  {
    // initialize all elements to non-empty, to verify wireDecode clears them
    i.setName("/A");
    i.setForwardingHint({"/F"});
    i.setNonce(0x03d645a8);
    i.setInterestLifetime(18554_ms);
    i.setHopLimit(64);
    i.setApplicationParameters("2404A0A1A2A3"_block);
  }

protected:
  Interest i;
};

BOOST_FIXTURE_TEST_SUITE(Decode, DecodeFixture)

BOOST_AUTO_TEST_CASE(NotAnInterest)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("4202CAFE"_block), tlv::Error, [] (const auto& e) {
    return e.what() == "Expecting Interest element, but TLV has type 66"s;
  });
}

BOOST_AUTO_TEST_CASE(NameOnly)
{
  i.wireDecode("0505 0703(080149)"_block);
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK_EQUAL(i.getName(), "/I");
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i.hasNonce(), false);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.getApplicationParameters().isValid(), false);

  // modify then re-encode
  i.setNonce(0x957c6554);
  BOOST_CHECK_EQUAL(i.hasWire(), false);
  BOOST_CHECK_EQUAL(i.wireEncode(), "050B 0703(080149) 0A04957C6554"_block);
}

BOOST_AUTO_TEST_CASE(NameCanBePrefix)
{
  i.wireDecode("0507 0703(080149) 2100"_block);
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK_EQUAL(i.getName(), "/I");
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i.hasNonce(), false);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.getApplicationParameters().isValid(), false);
}

BOOST_AUTO_TEST_CASE(FullWithoutParameters)
{
  i.wireDecode("0531 0703(080149) "
               "FC00 2100 FC00 1200 FC00 1E0B(1F09 1E023E15 0703080148) "
               "FC00 0A044ACB1E4C FC00 0C0276A1 FC00 2201D6 FC00"_block);
  BOOST_CHECK_EQUAL(i.getName(), "/I");
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), true);
  BOOST_TEST(i.getForwardingHint() == std::vector<Name>({"/H"}), boost::test_tools::per_element());
  BOOST_CHECK_EQUAL(i.hasNonce(), true);
  BOOST_CHECK_EQUAL(i.getNonce(), 0x4acb1e4c);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 30369_ms);
  BOOST_CHECK_EQUAL(*i.getHopLimit(), 214);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.getApplicationParameters().isValid(), false);

  // encode without modification: retain original wire encoding
  BOOST_CHECK_EQUAL(i.wireEncode().value_size(), 49);

  // modify then re-encode:
  // * unrecognized elements are discarded;
  // * ForwardingHint is re-encoded as a sequence of Names
  i.setName("/J");
  BOOST_CHECK_EQUAL(i.wireEncode(),
                    "051D 0703(08014A) "
                    "2100 1200 1E05(0703080148) "
                    "0A044ACB1E4C 0C0276A1 2201D6"_block);
}

BOOST_AUTO_TEST_CASE(FullWithParameters)
{
  i.wireDecode("055B 0725(080149 0220F16DB273F40436A852063F864D5072B01EAD53151F5A688EA1560492BEBEDD05) "
               "FC00 2100 FC00 1200 FC00 1E0B(1F09 1E023E15 0703080148) "
               "FC00 0A044ACB1E4C FC00 0C0276A1 FC00 2201D6 FC00 2404C0C1C2C3 FC00"_block);
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=f16db273f40436a852063f864d5072b01ead53151f5a688ea1560492bebedd05");
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), true);
  BOOST_TEST(i.getForwardingHint() == std::vector<Name>({"/H"}), boost::test_tools::per_element());
  BOOST_CHECK_EQUAL(i.hasNonce(), true);
  BOOST_CHECK_EQUAL(i.getNonce(), 0x4acb1e4c);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 30369_ms);
  BOOST_CHECK_EQUAL(*i.getHopLimit(), 214);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2404C0C1C2C3"_block);

  // encode without modification: retain original wire encoding
  BOOST_CHECK_EQUAL(i.wireEncode().value_size(), 91);

  // modify then re-encode:
  // * unrecognized elements after ApplicationParameters are preserved, the rest are discarded;
  // * ForwardingHint is re-encoded as a sequence of Names
  i.setName("/J");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.wireEncode(),
                    "0547 0725(08014A 0220F16DB273F40436A852063F864D5072B01EAD53151F5A688EA1560492BEBEDD05) "
                    "2100 1200 1E05(0703080148) "
                    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3 FC00"_block);

  // modify ApplicationParameters: unrecognized elements are preserved
  i.setApplicationParameters("2402CAFE"_block);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.wireEncode(),
                    "0545 0725(08014A 02205FDA67967EE302FC457E41B7D3D51BA6A9379574D193FD88F64954BF16C2927A) "
                    "2100 1200 1E05(0703080148) "
                    "0A044ACB1E4C 0C0276A1 2201D6 2402CAFE FC00"_block);
}

BOOST_AUTO_TEST_CASE(CriticalElementOutOfOrder)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode(
    "0529 2100 0703080149 1200 1E0B(1F09 1E023E15 0703080148) "
    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode(
    "0529 0703080149 1200 2100 1E0B(1F09 1E023E15 0703080148) "
    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "CanBePrefix element is out of order"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode(
    "0529 0703080149 2100 1E0B(1F09 1E023E15 0703080148) 1200 "
    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "MustBeFresh element is out of order"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode(
    "0529 0703080149 2100 1200 0A044ACB1E4C "
    "1E0B(1F09 1E023E15 0703080148) 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "ForwardingHint element is out of order"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode(
    "0529 0703080149 2100 1200 1E0B(1F09 1E023E15 0703080148) "
    "0C0276A1 0A044ACB1E4C 2201D6 2404C0C1C2C3"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "Nonce element is out of order"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode(
    "0529 0703080149 2100 1200 1E0B(1F09 1E023E15 0703080148) "
    "0A044ACB1E4C 2201D6 0C0276A1 2404C0C1C2C3"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "InterestLifetime element is out of order"s; });
}

BOOST_AUTO_TEST_CASE(NonCriticalElementOutOfOrder)
{
  // duplicate HopLimit
  i.wireDecode("0536 0725(080149 0220FF9100E04EAADCF30674D98026A051BA25F56B69BFA026DCCCD72C6EA0F7315A)"
               "2201D6 2200 2404C0C1C2C3 22020101"_block);
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(*i.getHopLimit(), 214);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2404C0C1C2C3"_block);

  // duplicate ApplicationParameters
  i.wireDecode("0541 0725(080149 0220FF9100E04EAADCF30674D98026A051BA25F56B69BFA026DCCCD72C6EA0F7315A)"
               "2100 1200 0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3 2401EE"_block);
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(*i.getHopLimit(), 214);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2404C0C1C2C3"_block);
}

BOOST_AUTO_TEST_CASE(MissingName)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0500"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode("0502 1200"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
}

BOOST_AUTO_TEST_CASE(BadName)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0502 0700"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Name has zero name components"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode("054C 074A(080149"
    "02200000000000000000000000000000000000000000000000000000000000000000"
    "080132"
    "02200000000000000000000000000000000000000000000000000000000000000000)"_block),
    tlv::Error,
    [] (const auto& e) { return e.what() == "Name has more than one ParametersSha256DigestComponent"s; });
}

BOOST_AUTO_TEST_CASE(BadCanBePrefix)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0508 0703080149 210102"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "CanBePrefix element has non-zero TLV-LENGTH"s; });
}

BOOST_AUTO_TEST_CASE(BadMustBeFresh)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0508 0703080149 120102"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "MustBeFresh element has non-zero TLV-LENGTH"s; });
}

BOOST_AUTO_TEST_CASE(BadNonce)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0507 0703080149 0A00"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Nonce element is malformed"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode("050A 0703080149 0A0304C263"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Nonce element is malformed"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode("050C 0703080149 0A05EFA420B262"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Nonce element is malformed"s; });
}

BOOST_AUTO_TEST_CASE(BadHopLimit)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0507 0703080149 2200"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "HopLimit element is malformed"s; });
  BOOST_CHECK_EXCEPTION(i.wireDecode("0509 0703080149 22021356"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "HopLimit element is malformed"s; });
}

BOOST_AUTO_TEST_CASE(BadParametersDigest)
{
  // ApplicationParameters without ParametersSha256DigestComponent
  Block b1("0509 0703(080149) 2402CAFE"_block);
  // ParametersSha256DigestComponent without ApplicationParameters
  Block b2("0527 0725(080149 0220E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855)"_block);
  // digest mismatch
  Block b3("052B 0725(080149 02200000000000000000000000000000000000000000000000000000000000000000) "
           "2402CAFE"_block);

  BOOST_CHECK_THROW(i.wireDecode(b1), tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(b2), tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(b3), tlv::Error);

  DisableAutoCheckParametersDigest disabler;
  BOOST_CHECK_NO_THROW(i.wireDecode(b1));
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
  BOOST_CHECK_NO_THROW(i.wireDecode(b2));
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
  BOOST_CHECK_NO_THROW(i.wireDecode(b3));
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
}

BOOST_AUTO_TEST_CASE(UnrecognizedNonCriticalElementBeforeName)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0507 FC00 0703080149"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Name element is missing or out of order"s; });
}

BOOST_AUTO_TEST_CASE(UnrecognizedCriticalElement)
{
  BOOST_CHECK_EXCEPTION(i.wireDecode("0507 0703080149 FB00"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Unrecognized element of critical type 251"s; });
  // v0.2 packet with Selectors
  BOOST_CHECK_EXCEPTION(i.wireDecode("0510 0703080149 09030D0101 0A0401000000"_block), tlv::Error,
                        [] (const auto& e) { return e.what() == "Unrecognized element of critical type 9"s; });
}

BOOST_AUTO_TEST_SUITE_END() // Decode

BOOST_AUTO_TEST_CASE(MatchesData)
{
  auto interest = makeInterest("/A");

  auto data = makeData("/A");
  BOOST_CHECK_EQUAL(interest->matchesData(*data), true);

  data->setName("/A/D");
  BOOST_CHECK_EQUAL(interest->matchesData(*data), false); // violates CanBePrefix

  interest->setCanBePrefix(true);
  BOOST_CHECK_EQUAL(interest->matchesData(*data), true);

  interest->setMustBeFresh(true);
  BOOST_CHECK_EQUAL(interest->matchesData(*data), false); // violates MustBeFresh

  data->setFreshnessPeriod(1_s);
  BOOST_CHECK_EQUAL(interest->matchesData(*data), true);

  data->setName("/H/I");
  BOOST_CHECK_EQUAL(interest->matchesData(*data), false); // Name does not match

  data->wireEncode();
  interest = makeInterest(data->getFullName());
  BOOST_CHECK_EQUAL(interest->matchesData(*data), true);

  setNameComponent(*interest, -1,
                   name::Component::fromEscapedString("sha256digest=00000000000000000000000000"
                                                      "00000000000000000000000000000000000000"));
  BOOST_CHECK_EQUAL(interest->matchesData(*data), false); // violates implicit digest
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MatchesInterest, 1)
BOOST_AUTO_TEST_CASE(MatchesInterest)
{
  Interest interest;
  interest.setName("/A")
          .setCanBePrefix(true)
          .setMustBeFresh(true)
          .setForwardingHint({"/H"})
          .setNonce(2228)
          .setInterestLifetime(5_s)
          .setHopLimit(90);

  Interest other;
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false);

  other.setName(interest.getName());
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false);

  other.setCanBePrefix(interest.getCanBePrefix());
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false);

  other.setMustBeFresh(interest.getMustBeFresh());
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false); // will match until #3162 implemented

  auto fh = interest.getForwardingHint();
  other.setForwardingHint(std::vector<Name>(fh.begin(), fh.end()));
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);

  other.setNonce(9336);
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);

  other.setInterestLifetime(3_s);
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);

  other.setHopLimit(31);
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);
}

BOOST_AUTO_TEST_CASE(SetName)
{
  Interest i;
  BOOST_CHECK_EQUAL(i.getName(), "/");
  i.setName("/A/B");
  BOOST_CHECK_EQUAL(i.getName(), "/A/B");
  i.setName("/I/params-sha256=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  BOOST_CHECK_THROW(i.setName("/I"
                              "/params-sha256=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
                              "/params-sha256=0000000000000000000000000000000000000000000000000000000000000000"),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(SetCanBePrefix)
{
  Interest i;
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), false);
  i.setCanBePrefix(true);
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
  i.setCanBePrefix(false);
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), false);
}

BOOST_AUTO_TEST_CASE(SetMustBeFresh)
{
  Interest i;
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  i.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), true);
  i.setMustBeFresh(false);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
}

BOOST_AUTO_TEST_CASE(GetNonce)
{
  unique_ptr<Interest> i1, i2;
  Interest::Nonce nonce1(0), nonce2(0);

  // getNonce automatically assigns a random Nonce.
  // It's possible to assign the same Nonce to two Interest, but it's unlikely to get 100 pairs of
  // identical Nonces in a row.
  int nIterations = 0;
  do {
    i1 = make_unique<Interest>();
    nonce1 = i1->getNonce();
    i2 = make_unique<Interest>();
    nonce2 = i2->getNonce();
  }
  while (nonce1 == nonce2 && ++nIterations < 100);

  BOOST_CHECK_NE(nonce1, nonce2);
  BOOST_CHECK(i1->hasNonce());
  BOOST_CHECK(i2->hasNonce());

  // Once a Nonce is assigned, it should not change.
  BOOST_CHECK_EQUAL(i1->getNonce(), nonce1);
  BOOST_CHECK_EQUAL(i2->getNonce(), nonce2);
}

BOOST_AUTO_TEST_CASE(SetNonce)
{
  Interest i1("/A");
  BOOST_CHECK(!i1.hasNonce());

  i1.setNonce(1);
  i1.wireEncode();
  BOOST_CHECK(i1.hasNonce());
  BOOST_CHECK_EQUAL(i1.getNonce(), 1);

  Interest i2(i1);
  BOOST_CHECK(i2.hasNonce());
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);

  i2.setNonce(2);
  BOOST_CHECK_EQUAL(i2.getNonce(), 2);
  BOOST_CHECK_EQUAL(i1.getNonce(), 1); // should not affect i1's Nonce (Bug #4168)

  i2.setNonce(nullopt);
  BOOST_CHECK(!i2.hasNonce());
}

BOOST_AUTO_TEST_CASE(RefreshNonce)
{
  Interest i;
  BOOST_CHECK(!i.hasNonce());
  i.refreshNonce();
  BOOST_CHECK(!i.hasNonce());

  i.setNonce(1);
  BOOST_CHECK(i.hasNonce());
  i.refreshNonce();
  BOOST_CHECK(i.hasNonce());
  BOOST_CHECK_NE(i.getNonce(), 1);
}

BOOST_AUTO_TEST_CASE(NonceConversions)
{
  Interest i;

  // 4-arg constructor
  Interest::Nonce n1(1, 2, 3, 4);
  i.setNonce(n1);
  BOOST_CHECK_EQUAL(i.getNonce(), 0x01020304);

  // 4-arg constructor + assignment
  n1 = {0xf, 0xe, 0xd, 0xc};
  i.setNonce(n1);
  BOOST_CHECK_EQUAL(i.getNonce(), 0x0f0e0d0c);

  // 1-arg constructor + assignment (implicit conversion)
  Interest::Nonce n2;
  n2 = 42;
  BOOST_CHECK_NE(n1, n2);
  i.setNonce(n2);
  n2 = 21; // should not affect i's Nonce
  BOOST_CHECK_EQUAL(i.getNonce(), 42);
  BOOST_CHECK_EQUAL(i.toUri(), "/?Nonce=0000002a"); // stored in big-endian
}

BOOST_AUTO_TEST_CASE(SetInterestLifetime)
{
  BOOST_CHECK_THROW(Interest("/A", -1_ms), std::invalid_argument);
  BOOST_CHECK_NO_THROW(Interest("/A", 0_ms));

  Interest i;
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK_THROW(i.setInterestLifetime(-1_ms), std::invalid_argument);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  i.setInterestLifetime(0_ms);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 0_ms);
  i.setInterestLifetime(1_ms);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 1_ms);

  i = Interest("/B", 15_s);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 15_s);
}

BOOST_AUTO_TEST_CASE(SetHopLimit)
{
  Interest i;
  BOOST_CHECK(i.getHopLimit() == nullopt);
  i.setHopLimit(42);
  BOOST_CHECK(i.getHopLimit() == 42);
  i.setHopLimit(nullopt);
  BOOST_CHECK(i.getHopLimit() == nullopt);
}

BOOST_AUTO_TEST_CASE(SetApplicationParameters)
{
  const uint8_t PARAMETERS1[] = {0xc1};
  const uint8_t PARAMETERS2[] = {0xc2};

  Interest i;
  BOOST_CHECK(!i.hasApplicationParameters());
  i.setApplicationParameters("2400"_block);
  BOOST_CHECK(i.hasApplicationParameters());
  i.unsetApplicationParameters();
  BOOST_CHECK(!i.hasApplicationParameters());

  // Block overload
  i.setApplicationParameters("2401C0"_block);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2401C0"_block);
  i.setApplicationParameters("8001C1"_block);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "24038001C1"_block);
  BOOST_CHECK_THROW(i.setApplicationParameters(Block{}), std::invalid_argument);

  // span overload
  i.setApplicationParameters(PARAMETERS1);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2401C1"_block);
  i.setApplicationParameters(span<uint8_t>{});
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);

  // raw buffer+size overload (deprecated)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  BOOST_CHECK_THROW(i.setApplicationParameters(nullptr, 42), std::invalid_argument);
#pragma GCC diagnostic pop

  // ConstBufferPtr overload
  i.setApplicationParameters(make_shared<Buffer>(PARAMETERS2, sizeof(PARAMETERS2)));
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2401C2"_block);
  i.setApplicationParameters(make_shared<Buffer>());
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);
  BOOST_CHECK_THROW(i.setApplicationParameters(nullptr), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(SetSignature)
{
  Interest i;
  BOOST_CHECK(i.getSignatureInfo() == nullopt);
  BOOST_CHECK_EQUAL(i.isSigned(), false);

  // Throws because attempting to set InterestSignatureValue without set InterestSignatureInfo
  Block sv1("2E04 01020304"_block);
  auto svBuffer1 = make_shared<Buffer>(sv1.value_begin(), sv1.value_end());
  BOOST_CHECK_THROW(i.setSignatureValue(svBuffer1), tlv::Error);

  // Simple set/get case for InterestSignatureInfo (no prior set)
  SignatureInfo si1(tlv::SignatureSha256WithEcdsa);
  i.setSignatureInfo(si1);
  BOOST_CHECK(i.getSignatureInfo() == si1);
  BOOST_CHECK_EQUAL(i.isSigned(), false);

  // Simple set/get case for InterestSignatureValue (no prior set)
  BOOST_CHECK_EQUAL(i.getSignatureValue().isValid(), false);
  i.setSignatureValue(svBuffer1);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv1);
  BOOST_CHECK_EQUAL(i.isSigned(), true);

  // Throws because attempting to set InterestSignatureValue to nullptr
  BOOST_CHECK_THROW(i.setSignatureValue(nullptr), std::invalid_argument);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv1);
  BOOST_CHECK_EQUAL(i.isSigned(), true);

  // Ensure that wire is not reset if specified InterestSignatureInfo is same
  i.wireEncode();
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  i.setSignatureInfo(si1);
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK(i.getSignatureInfo() == si1);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv1);
  BOOST_CHECK_EQUAL(i.isSigned(), true);

  // Ensure that wire is reset if specified InterestSignatureInfo is different
  i.wireEncode();
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  SignatureInfo si2(tlv::SignatureSha256WithRsa);
  i.setSignatureInfo(si2);
  BOOST_CHECK_EQUAL(i.hasWire(), false);
  BOOST_CHECK(i.getSignatureInfo() == si2);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv1);
  BOOST_CHECK_EQUAL(i.isSigned(), true);

  // Ensure that wire is not reset if specified InterestSignatureValue is same
  i.wireEncode();
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  i.setSignatureValue(svBuffer1);
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK(i.getSignatureInfo() == si2);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv1);
  BOOST_CHECK_EQUAL(i.isSigned(), true);

  // Ensure that wire is reset if specified InterestSignatureValue is different
  i.wireEncode();
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  Block sv2("2E04 99887766"_block);
  auto svBuffer2 = make_shared<Buffer>(sv2.value_begin(), sv2.value_end());
  i.setSignatureValue(svBuffer2);
  BOOST_CHECK_EQUAL(i.hasWire(), false);
  BOOST_CHECK(i.getSignatureInfo() == si2);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv2);
  BOOST_CHECK_EQUAL(i.isSigned(), true);
}

BOOST_AUTO_TEST_CASE(ParametersSha256DigestComponent)
{
  Interest i("/I");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setApplicationParameters("2404C0C1C2C3"_block); // auto-appends ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setApplicationParameters(span<uint8_t>{}); // updates ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=33b67cb5385ceddad93d0ee960679041613bed34b8b4a5e6362fe7539ba2d3ce");
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.unsetApplicationParameters(); // removes ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(), "/I");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setName(Name("/P").appendParametersSha256DigestPlaceholder().append("Q"));
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);

  i.unsetApplicationParameters(); // removes ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(), "/P/Q");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setName(Name("/P").appendParametersSha256DigestPlaceholder().append("Q"));
  i.setApplicationParameters("2404C0C1C2C3"_block); // updates ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/P/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a/Q");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setName("/A/B/C"); // auto-appends ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/A/B/C/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  SignatureInfo si(tlv::SignatureSha256WithEcdsa);
  i.setSignatureInfo(si); // updates ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/A/B/C/params-sha256=6400cae1730c15fd7854b26be05794d53685423c94bc61e59c49bd640d646ae8");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2404 C0C1C2C3"_block);
  BOOST_CHECK(i.getSignatureInfo() == si);

  i.unsetApplicationParameters(); // removes ParametersSha256DigestComponent and InterestSignatureInfo
  BOOST_CHECK(i.getSignatureInfo() == nullopt);
  BOOST_CHECK_EQUAL(i.getSignatureValue().isValid(), false);
  BOOST_CHECK_EQUAL(i.getName(), "/A/B/C");

  i.setSignatureInfo(si); // auto-adds an empty ApplicationParameters element
  BOOST_CHECK_EQUAL(i.getName(),
                    "/A/B/C/params-sha256=d2ac0eb1f60f60ab206fb80bf1d0f73cfef353bbec43ba6ea626117f671ca3bb");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);
  BOOST_CHECK(i.getSignatureInfo() == si);

  Block sv("2E04 01020304"_block);
  i.setSignatureValue(make_shared<Buffer>(sv.value_begin(),
                                          sv.value_end())); // updates ParametersDigestSha256Component
  BOOST_CHECK_EQUAL(i.getName(),
                    "/A/B/C/params-sha256=f649845ef944638390d1c689e2f0618ea02e471eff236110cbeb822d5932d342");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);
  BOOST_CHECK(i.getSignatureInfo() == si);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv);

  i.setApplicationParameters("2404C0C1C2C3"_block); // updates ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/A/B/C/params-sha256=c5d7e567e6b251ddf36f7a6dbed95235b2d4a0b36215bb0f3cc403ac64ad0284");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2404 C0C1C2C3"_block);
  BOOST_CHECK(i.getSignatureInfo() == si);
  BOOST_CHECK_EQUAL(i.getSignatureValue(), sv);
}

BOOST_AUTO_TEST_CASE(ExtractSignedRanges)
{
  Interest i1;
  BOOST_CHECK_EXCEPTION(i1.extractSignedRanges(), tlv::Error, [] (const auto& e) {
    return e.what() == "Name has zero name components"s;
  });
  i1.setName("/test/prefix");
  i1.setNonce(0x01020304);
  SignatureInfo sigInfo(tlv::DigestSha256);
  i1.setSignatureInfo(sigInfo);

  // Test with previously unsigned Interest (no InterestSignatureValue)
  auto ranges1 = i1.extractSignedRanges();
  BOOST_REQUIRE_EQUAL(ranges1.size(), 2);
  const Block& wire1 = i1.wireEncode();
  // Ensure Name range captured properly
  Block nameWithoutDigest1 = i1.getName().getPrefix(-1).wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges1.front().begin(), ranges1.front().end(),
                                nameWithoutDigest1.value_begin(), nameWithoutDigest1.value_end());
  // Ensure parameters range captured properly
  const auto& appParamsWire1 = wire1.find(tlv::ApplicationParameters);
  BOOST_REQUIRE(appParamsWire1 != wire1.elements_end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges1.back().begin(), ranges1.back().end(),
                                appParamsWire1->begin(), wire1.end());

  // Test with Interest with existing InterestSignatureValue
  auto sigValue = make_shared<Buffer>();
  i1.setSignatureValue(sigValue);
  auto ranges2 = i1.extractSignedRanges();
  BOOST_REQUIRE_EQUAL(ranges2.size(), 2);
  const auto& wire2 = i1.wireEncode();
  // Ensure Name range captured properly
  Block nameWithoutDigest2 = i1.getName().getPrefix(-1).wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges2.front().begin(), ranges2.front().end(),
                                nameWithoutDigest2.value_begin(), nameWithoutDigest2.value_end());
  // Ensure parameters range captured properly
  const auto& appParamsWire2 = wire2.find(tlv::ApplicationParameters);
  BOOST_REQUIRE(appParamsWire2 != wire2.elements_end());
  const auto& sigValueWire2 = wire2.find(tlv::InterestSignatureValue);
  BOOST_REQUIRE(sigValueWire2 != wire2.elements_end());
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges2.back().begin(), ranges2.back().end(),
                                appParamsWire2->begin(), sigValueWire2->begin());

  // Test with decoded Interest
  const uint8_t WIRE[] = {
    0x05, 0x6f, // Interest
          0x07, 0x2e, // Name
                0x08, 0x04, // GenericNameComponent
                      0x61, 0x62, 0x63, 0x64,
                0x08, 0x04, // GenericNameComponent
                      0x65, 0x66, 0x67, 0x68,
                0x02, 0x20, // ParametersSha256DigestComponent
                      0x6f, 0x29, 0x58, 0x60, 0x53, 0xee, 0x9f, 0xcc,
                      0xd8, 0xa4, 0x22, 0x12, 0x29, 0x25, 0x28, 0x7c,
                      0x0a, 0x18, 0x43, 0x5f, 0x40, 0x74, 0xc4, 0x0a,
                      0xbb, 0x0d, 0x5b, 0x30, 0xe4, 0xaa, 0x62, 0x20,
          0x12, 0x00, // MustBeFresh
          0x0a, 0x04, // Nonce
                0x4c, 0x1e, 0xcb, 0x4a,
          0x24, 0x04, // ApplicationParameters
                0xc0, 0xc1, 0xc2, 0xc3,
          0x2c, 0x0d, // InterestSignatureInfo
                0x1b, 0x01, // SignatureType
                      0x00,
                0x26, 0x08, // SignatureNonce
                      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
          0x2e, 0x20, // InterestSignatureValue
                0x12, 0x47, 0x1a, 0xe0, 0xf8, 0x72, 0x3a, 0xc1,
                0x15, 0x6c, 0x37, 0x0a, 0x38, 0x71, 0x1e, 0xbe,
                0xbf, 0x28, 0x17, 0xde, 0x9b, 0x2d, 0xd9, 0x4e,
                0x9b, 0x7e, 0x62, 0xf1, 0x17, 0xb8, 0x76, 0xc1,
  };
  Block wire3(WIRE);
  Interest i2(wire3);
  auto ranges3 = i2.extractSignedRanges();
  BOOST_REQUIRE_EQUAL(ranges3.size(), 2);
  // Ensure Name range captured properly
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges3.front().begin(), ranges3.front().end(), &WIRE[4], &WIRE[16]);
  // Ensure parameters range captured properly
  BOOST_CHECK_EQUAL_COLLECTIONS(ranges3.back().begin(), ranges3.back().end(), &WIRE[58], &WIRE[79]);

  // Test failure with missing ParametersSha256DigestComponent
  Interest i3("/a");
  BOOST_CHECK_EXCEPTION(i3.extractSignedRanges(), tlv::Error, [] (const auto& e) {
    return e.what() == "Interest Name must end with a ParametersSha256DigestComponent"s;
  });

  // Test failure with missing InterestSignatureInfo
  i3.setApplicationParameters(span<uint8_t>{});
  BOOST_CHECK_EXCEPTION(i3.extractSignedRanges(), tlv::Error, [] (const auto& e) {
    return e.what() == "Interest missing InterestSignatureInfo"s;
  });
}

BOOST_AUTO_TEST_CASE(ToUri)
{
  Interest i;
  BOOST_CHECK_EQUAL(i.toUri(), "/");

  i.setName("/foo");
  BOOST_CHECK_EQUAL(i.toUri(), "/foo");

  i.setCanBePrefix(true);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix");

  i.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh");

  i.setNonce(0xa1b2c3);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh&Nonce=00a1b2c3");

  i.setInterestLifetime(2_s);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh&Nonce=00a1b2c3&Lifetime=2000");

  i.setHopLimit(18);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh&Nonce=00a1b2c3&Lifetime=2000&HopLimit=18");

  i.setCanBePrefix(false);
  i.setMustBeFresh(false);
  i.setHopLimit(nullopt);
  i.setApplicationParameters("2402CAFE"_block);
  BOOST_CHECK_EQUAL(i.toUri(),
                    "/foo/params-sha256=8621f5e8321f04104640c8d02877d7c5142cad6e203c5effda1783b1a0e476d6"
                    "?Nonce=00a1b2c3&Lifetime=2000");
}

BOOST_AUTO_TEST_SUITE_END() // TestInterest

} // namespace tests
} // namespace ndn
