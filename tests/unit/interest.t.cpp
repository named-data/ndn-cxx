/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "tests/boost-test.hpp"
#include "tests/make-interest-data.hpp"

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
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i.hasNonce(), false);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.getApplicationParameters().isValid(), false);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
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
                0x01, 0x00, 0x00, 0x00,
  };

  Interest i1;
  i1.setName("/local/ndn/prefix");
  i1.setCanBePrefix(false);
  i1.setNonce(1);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  Block wire1 = i1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i2.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i2.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i2.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i2.hasNonce(), true);
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i2.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i2.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i2.getApplicationParameters().isValid(), false);
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
                0x01, 0x00, 0x00, 0x00,
          0x24, 0x04, // ApplicationParameters
                0xc0, 0xc1, 0xc2, 0xc3
  };

  Interest i1;
  i1.setName("/local/ndn/prefix");
  i1.setCanBePrefix(false);
  i1.setNonce(1);
  i1.setApplicationParameters("2404C0C1C2C3"_block);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  Block wire1 = i1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(),
                    "/local/ndn/prefix/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i2.getCanBePrefix(), false);
  BOOST_CHECK_EQUAL(i2.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i2.getForwardingHint().empty(), true);
  BOOST_CHECK_EQUAL(i2.hasNonce(), true);
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK(i2.getHopLimit() == nullopt);
  BOOST_CHECK_EQUAL(i2.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i2.getApplicationParameters(), "2404C0C1C2C3"_block);
}

BOOST_AUTO_TEST_CASE(Full)
{
  const uint8_t WIRE[] = {
    0x05, 0x5c, // Interest
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
          0x1e, 0x0b, // ForwardingHint
                0x1f, 0x09, // Delegation List
                      0x1e, 0x02,
                            0x3e, 0x15,
                      0x07, 0x03,
                            0x08, 0x01, 0x48,
          0x0a, 0x04, // Nonce
                0x4a, 0xcb, 0x1e, 0x4c,
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
  i1.setForwardingHint(DelegationList({{15893, "/H"}}));
  i1.setNonce(0x4c1ecb4a);
  i1.setInterestLifetime(30369_ms);
  i1.setHopLimit(220);
  i1.setApplicationParameters("2404C0C1C2C3"_block);
  BOOST_CHECK_EQUAL(i1.isParametersDigestValid(), true);

  Block wire1 = i1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(),
                    "/local/ndn/prefix/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i2.getCanBePrefix(), true);
  BOOST_CHECK_EQUAL(i2.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(i2.getForwardingHint(), DelegationList({{15893, "/H"}}));
  BOOST_CHECK_EQUAL(i2.hasNonce(), true);
  BOOST_CHECK_EQUAL(i2.getNonce(), 0x4c1ecb4a);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), 30369_ms);
  BOOST_CHECK_EQUAL(*i2.getHopLimit(), 220);
  BOOST_CHECK_EQUAL(i2.getApplicationParameters(), "2404C0C1C2C3"_block);
}

BOOST_AUTO_TEST_CASE(MissingApplicationParameters)
{
  Interest i;
  i.setName(Name("/A").appendParametersSha256DigestPlaceholder());
  i.setCanBePrefix(false);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
  BOOST_CHECK_THROW(i.wireEncode(), tlv::Error);
}

BOOST_AUTO_TEST_CASE(MissingParametersSha256DigestComponent)
{
  // there's no way to create an Interest that fails this check via programmatic construction,
  // so we have to decode an invalid Interest and force reencoding

  DisableAutoCheckParametersDigest disabler;
  Interest i("050F 0703(080149) 0A04F000F000 2402CAFE"_block);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), false);
  BOOST_CHECK_NO_THROW(i.wireEncode()); // this succeeds because it uses the cached wire encoding

  i.setNonce(42); // trigger reencoding
  BOOST_CHECK_THROW(i.wireEncode(), tlv::Error); // now the check fails while attempting to reencode
}

BOOST_AUTO_TEST_SUITE_END() // Encode

class DecodeFixture
{
protected:
  DecodeFixture()
  {
    // initialize all elements to non-empty, to verify wireDecode clears them
    i.setName("/A");
    i.setForwardingHint({{10309, "/F"}});
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
  BOOST_CHECK_THROW(i.wireDecode("4202CAFE"_block), tlv::Error);
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
  i.setNonce(0x54657c95);
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
  BOOST_CHECK_EQUAL(i.getForwardingHint(), DelegationList({{15893, "/H"}}));
  BOOST_CHECK_EQUAL(i.hasNonce(), true);
  BOOST_CHECK_EQUAL(i.getNonce(), 0x4c1ecb4a);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 30369_ms);
  BOOST_CHECK_EQUAL(*i.getHopLimit(), 214);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), false);
  BOOST_CHECK_EQUAL(i.getApplicationParameters().isValid(), false);

  // encode without modification: retain original wire encoding
  BOOST_CHECK_EQUAL(i.wireEncode().value_size(), 49);

  // modify then re-encode: unrecognized elements are discarded
  i.setName("/J");
  BOOST_CHECK_EQUAL(i.wireEncode(),
                    "0523 0703(08014A) "
                    "2100 1200 1E0B(1F09 1E023E15 0703080148) "
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
  BOOST_CHECK_EQUAL(i.getForwardingHint(), DelegationList({{15893, "/H"}}));
  BOOST_CHECK_EQUAL(i.hasNonce(), true);
  BOOST_CHECK_EQUAL(i.getNonce(), 0x4c1ecb4a);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 30369_ms);
  BOOST_CHECK_EQUAL(*i.getHopLimit(), 214);
  BOOST_CHECK_EQUAL(i.hasApplicationParameters(), true);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2404C0C1C2C3"_block);

  // encode without modification: retain original wire encoding
  BOOST_CHECK_EQUAL(i.wireEncode().value_size(), 91);

  // modify then re-encode: unrecognized elements after ApplicationParameters
  //                        are preserved, the rest are discarded
  i.setName("/J");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.wireEncode(),
                    "054D 0725(08014A 0220F16DB273F40436A852063F864D5072B01EAD53151F5A688EA1560492BEBEDD05) "
                    "2100 1200 1E0B(1F09 1E023E15 0703080148) "
                    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3 FC00"_block);

  // modify ApplicationParameters: unrecognized elements are preserved
  i.setApplicationParameters("2402CAFE"_block);
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);
  BOOST_CHECK_EQUAL(i.wireEncode(),
                    "054B 0725(08014A 02205FDA67967EE302FC457E41B7D3D51BA6A9379574D193FD88F64954BF16C2927A) "
                    "2100 1200 1E0B(1F09 1E023E15 0703080148) "
                    "0A044ACB1E4C 0C0276A1 2201D6 2402CAFE FC00"_block);
}

BOOST_AUTO_TEST_CASE(CriticalElementOutOfOrder)
{
  BOOST_CHECK_THROW(i.wireDecode(
    "0529 2100 0703080149 1200 1E0B(1F09 1E023E15 0703080148) "
    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(
    "0529 0703080149 1200 2100 1E0B(1F09 1E023E15 0703080148) "
    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(
    "0529 0703080149 2100 1E0B(1F09 1E023E15 0703080148) 1200 "
    "0A044ACB1E4C 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(
    "0529 0703080149 2100 1200 0A044ACB1E4C "
    "1E0B(1F09 1E023E15 0703080148) 0C0276A1 2201D6 2404C0C1C2C3"_block),
    tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(
    "0529 0703080149 2100 1200 1E0B(1F09 1E023E15 0703080148) "
    "0C0276A1 0A044ACB1E4C 2201D6 2404C0C1C2C3"_block),
    tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode(
    "0529 0703080149 2100 1200 1E0B(1F09 1E023E15 0703080148) "
    "0A044ACB1E4C 2201D6 0C0276A1 2404C0C1C2C3"_block),
    tlv::Error);
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
  BOOST_CHECK_THROW(i.wireDecode("0500"_block), tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode("0502 1200"_block), tlv::Error);
}

BOOST_AUTO_TEST_CASE(BadName)
{
  // empty
  BOOST_CHECK_THROW(i.wireDecode("0502 0700"_block), tlv::Error);

  // more than one ParametersSha256DigestComponent
  BOOST_CHECK_THROW(i.wireDecode("054C 074A(080149"
                                 "02200000000000000000000000000000000000000000000000000000000000000000"
                                 "080132"
                                 "02200000000000000000000000000000000000000000000000000000000000000000)"_block),
                    tlv::Error);
}

BOOST_AUTO_TEST_CASE(BadCanBePrefix)
{
  BOOST_CHECK_THROW(i.wireDecode("0508 0703080149 210102"_block), tlv::Error);
}

BOOST_AUTO_TEST_CASE(BadMustBeFresh)
{
  BOOST_CHECK_THROW(i.wireDecode("0508 0703080149 120102"_block), tlv::Error);
}

BOOST_AUTO_TEST_CASE(BadNonce)
{
  BOOST_CHECK_THROW(i.wireDecode("0507 0703080149 0A00"_block), tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode("050A 0703080149 0A0304C263"_block), tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode("050C 0703080149 0A05EFA420B262"_block), tlv::Error);
}

BOOST_AUTO_TEST_CASE(BadHopLimit)
{
  BOOST_CHECK_THROW(i.wireDecode("0507 0703080149 2200"_block), tlv::Error);
  BOOST_CHECK_THROW(i.wireDecode("0509 0703080149 22021356"_block), tlv::Error);
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
  BOOST_CHECK_THROW(i.wireDecode("0507 FC00 0703080149"_block), tlv::Error);
}

BOOST_AUTO_TEST_CASE(UnrecognizedCriticalElement)
{
  BOOST_CHECK_THROW(i.wireDecode("0507 0703080149 FB00"_block), tlv::Error);
  // v0.2 packet with Selectors
  BOOST_CHECK_THROW(i.wireDecode("0507 0703080149 09030D0101 0A0401000000"_block), tlv::Error);
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

  setNameComponent(*interest, -1, name::Component::fromEscapedString(
                     "sha256digest=0000000000000000000000000000000000000000000000000000000000000000"));
  BOOST_CHECK_EQUAL(interest->matchesData(*data), false); // violates implicit digest
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MatchesInterest, 1)
BOOST_AUTO_TEST_CASE(MatchesInterest)
{
  Interest interest;
  interest.setName("/A")
          .setCanBePrefix(true)
          .setMustBeFresh(true)
          .setForwardingHint({{1, "/H"}})
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

  other.setForwardingHint(interest.getForwardingHint());
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
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
  i.setCanBePrefix(false);
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), false);
  i.setCanBePrefix(true);
  BOOST_CHECK_EQUAL(i.getCanBePrefix(), true);
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

BOOST_AUTO_TEST_CASE(ModifyForwardingHint)
{
  Interest i("/I");
  i.setCanBePrefix(false);
  i.setForwardingHint({{1, "/A"}});
  i.wireEncode();
  BOOST_CHECK(i.hasWire());

  i.modifyForwardingHint([] (DelegationList& fh) { fh.insert(2, "/B"); });
  BOOST_CHECK(!i.hasWire());
  BOOST_CHECK_EQUAL(i.getForwardingHint(), DelegationList({{1, "/A"}, {2, "/B"}}));
}

BOOST_AUTO_TEST_CASE(GetNonce)
{
  unique_ptr<Interest> i1, i2;

  // getNonce automatically assigns a random Nonce.
  // It's possible to assign the same Nonce to two Interest, but it's unlikely to get 100 pairs of
  // same Nonces in a row.
  int nIterations = 0;
  uint32_t nonce1 = 0, nonce2 = 0;
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
}

BOOST_AUTO_TEST_CASE(SetNonce)
{
  Interest i1("/A");
  i1.setCanBePrefix(false);
  i1.setNonce(1);
  i1.wireEncode();
  BOOST_CHECK_EQUAL(i1.getNonce(), 1);

  Interest i2(i1);
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);

  i2.setNonce(2);
  BOOST_CHECK_EQUAL(i2.getNonce(), 2);
  BOOST_CHECK_EQUAL(i1.getNonce(), 1); // should not affect i1 Nonce (Bug #4168)
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
  i.setApplicationParameters(Block{});
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);
  i.setApplicationParameters("2401C0"_block);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2401C0"_block);
  i.setApplicationParameters("8001C1"_block);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "24038001C1"_block);

  // raw buffer+size overload
  i.setApplicationParameters(PARAMETERS1, sizeof(PARAMETERS1));
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2401C1"_block);
  i.setApplicationParameters(nullptr, 0);
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);
  BOOST_CHECK_THROW(i.setApplicationParameters(nullptr, 42), std::invalid_argument);

  // ConstBufferPtr overload
  i.setApplicationParameters(make_shared<Buffer>(PARAMETERS2, sizeof(PARAMETERS2)));
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2401C2"_block);
  i.setApplicationParameters(make_shared<Buffer>());
  BOOST_CHECK_EQUAL(i.getApplicationParameters(), "2400"_block);
  BOOST_CHECK_THROW(i.setApplicationParameters(nullptr), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ParametersSha256DigestComponent)
{
  Interest i("/I");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setApplicationParameters("2404C0C1C2C3"_block); // auto-appends ParametersSha256DigestComponent
  BOOST_CHECK_EQUAL(i.getName(),
                    "/I/params-sha256=ff9100e04eaadcf30674d98026a051ba25f56b69bfa026dcccd72c6ea0f7315a");
  BOOST_CHECK_EQUAL(i.isParametersDigestValid(), true);

  i.setApplicationParameters(nullptr, 0); // updates ParametersSha256DigestComponent
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
}

BOOST_AUTO_TEST_CASE(ToUri)
{
  Interest i;
  i.setCanBePrefix(false);
  BOOST_CHECK_EQUAL(i.toUri(), "/");

  i.setName("/foo");
  BOOST_CHECK_EQUAL(i.toUri(), "/foo");

  i.setCanBePrefix(true);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix");

  i.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh");

  i.setNonce(1234);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh&Nonce=1234");

  i.setInterestLifetime(2_s);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh&Nonce=1234&Lifetime=2000");

  i.setHopLimit(18);
  BOOST_CHECK_EQUAL(i.toUri(), "/foo?CanBePrefix&MustBeFresh&Nonce=1234&Lifetime=2000&HopLimit=18");

  i.setCanBePrefix(false);
  i.setMustBeFresh(false);
  i.setHopLimit(nullopt);
  i.setApplicationParameters("2402CAFE"_block);
  BOOST_CHECK_EQUAL(i.toUri(),
                    "/foo/params-sha256=8621f5e8321f04104640c8d02877d7c5142cad6e203c5effda1783b1a0e476d6"
                    "?Nonce=1234&Lifetime=2000");
}

BOOST_AUTO_TEST_SUITE_END() // TestInterest

} // namespace tests
} // namespace ndn
