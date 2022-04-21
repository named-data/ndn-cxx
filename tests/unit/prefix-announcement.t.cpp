/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/prefix-announcement.hpp"
#include "ndn-cxx/encoding/tlv-nfd.hpp"

#include "tests/key-chain-fixture.hpp"
#include "tests/test-common.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestPrefixAnnouncement)

static Data
makePrefixAnnData()
{
  return Data(
    "0678 0718 announced-name=/net/example 08036E6574 08076578616D706C65"
    "          keyword-prefix-ann=20025041 version=360101 segment=320100"
    "     1403 content-type=prefix-ann 180105"
    "     1530 expire in one hour 6D040036EE80"
    "          validity FD00FD26 FD00FE0F323031383130333054303030303030"
    "                            FD00FF0F323031383131323454323335393539"
    "     1603 1B0100 signature"
    "     1720 0000000000000000000000000000000000000000000000000000000000000000"_block);
}

BOOST_AUTO_TEST_CASE(DecodeGood)
{
  Data data0 = makePrefixAnnData();
  PrefixAnnouncement pa0(data0);
  BOOST_CHECK_EQUAL(pa0.getAnnouncedName(), "/net/example");
  BOOST_CHECK_EQUAL(pa0.getExpiration(), 1_h);
  BOOST_CHECK(pa0.getValidityPeriod());
  BOOST_CHECK_EQUAL(*pa0.getValidityPeriod(),
                    security::ValidityPeriod(time::fromIsoString("20181030T000000"),
                                             time::fromIsoString("20181124T235959")));

  // reorder ExpirationPeriod and ValidityPeriod, unrecognized non-critical element
  Data data1 = makePrefixAnnData();
  Block payload1 = data1.getContent();
  payload1.parse();
  Block expirationElement = payload1.get(tlv::nfd::ExpirationPeriod);
  payload1.remove(tlv::nfd::ExpirationPeriod);
  payload1.push_back(expirationElement);
  payload1.push_back("2000"_block);
  payload1.encode();
  data1.setContent(payload1);
  PrefixAnnouncement pa1(data1);
  BOOST_CHECK_EQUAL(pa1.getAnnouncedName(), "/net/example");
  BOOST_CHECK_EQUAL(pa1.getExpiration(), 1_h);
  BOOST_CHECK(pa1.getValidityPeriod());
  BOOST_CHECK_EQUAL(*pa1.getValidityPeriod(),
                    security::ValidityPeriod(time::fromIsoString("20181030T000000"),
                                             time::fromIsoString("20181124T235959")));

  // no ValidityPeriod
  Data data2 = makePrefixAnnData();
  Block payload2 = data2.getContent();
  payload2.parse();
  payload2.remove(tlv::ValidityPeriod);
  payload2.encode();
  data2.setContent(payload2);
  PrefixAnnouncement pa2(data2);
  BOOST_CHECK_EQUAL(pa2.getAnnouncedName(), "/net/example");
  BOOST_CHECK_EQUAL(pa2.getExpiration(), 1_h);
  BOOST_CHECK(!pa2.getValidityPeriod());
}

BOOST_AUTO_TEST_CASE(DecodeBad)
{
  // wrong ContentType
  Data data0 = makePrefixAnnData();
  data0.setContentType(tlv::ContentType_Blob);
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data0}, tlv::Error, [] (const auto& e) {
    return e.what() == "Data is not a prefix announcement: ContentType is 0"s;
  });

  // Name has no "32=PA" keyword
  Data data1 = makePrefixAnnData();
  setNameComponent(data1, -3, name::Component::fromEscapedString("32=not-PA"));
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data1}, tlv::Error, [] (const auto& e) {
    return e.what() == "Data is not a prefix announcement: wrong name structure"s;
  });

  // Name has no version component
  Data data2 = makePrefixAnnData();
  setNameComponent(data2, -2, "not-version");
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data2}, tlv::Error, [] (const auto& e) {
    return e.what() == "Data is not a prefix announcement: wrong name structure"s;
  });

  // Name has no segment number component
  Data data3 = makePrefixAnnData();
  setNameComponent(data3, -2, "not-segment");
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data3}, tlv::Error, [] (const auto& e) {
    return e.what() == "Data is not a prefix announcement: wrong name structure"s;
  });

  // Data without Content
  Data data4 = makePrefixAnnData();
  data4.unsetContent();
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data4}, tlv::Error, [] (const auto& e) {
    return e.what() == "Prefix announcement is empty"s;
  });

  // Content has no ExpirationPeriod element
  Data data5 = makePrefixAnnData();
  Block payload5 = data5.getContent();
  payload5.parse();
  payload5.remove(tlv::nfd::ExpirationPeriod);
  data5.setContent(payload5);
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data5}, tlv::Error, [] (const auto& e) {
    return e.what() == "No sub-element of type 109 found in block of type 21"s;
  });

  // ExpirationPeriod is malformed
  Data data6 = makePrefixAnnData();
  Block payload6 = data6.getContent();
  payload6.parse();
  payload6.remove(tlv::nfd::ExpirationPeriod);
  payload6.push_back("6D03010101"_block);
  data6.setContent(payload6);
  BOOST_CHECK_THROW(PrefixAnnouncement{data6}, tlv::Error);

  // ValidityPeriod is malformed
  Data data7 = makePrefixAnnData();
  Block payload7 = data7.getContent();
  payload7.parse();
  payload7.remove(tlv::ValidityPeriod);
  payload7.push_back("FD00FD00"_block);
  data7.setContent(payload7);
  BOOST_CHECK_THROW(PrefixAnnouncement{data7}, tlv::Error);

  // Content has unrecognized critical element
  Data data8 = makePrefixAnnData();
  Block payload8 = data8.getContent();
  payload8.parse();
  payload8.push_back("0200"_block);
  data8.setContent(payload8);
  BOOST_CHECK_EXCEPTION(PrefixAnnouncement{data8}, tlv::Error, [] (const auto& e) {
    return e.what() == "Unrecognized element of critical type 2"s;
  });
}

BOOST_FIXTURE_TEST_CASE(EncodeEmpty, KeyChainFixture)
{
  PrefixAnnouncement pa;
  BOOST_CHECK(!pa.getData());
  BOOST_CHECK_EQUAL(pa.getAnnouncedName(), "/");
  BOOST_CHECK_EQUAL(pa.getExpiration(), 0_ms);
  BOOST_CHECK(!pa.getValidityPeriod());

  const Data& data = pa.toData(m_keyChain, signingWithSha256(), 5);
  BOOST_CHECK_EQUAL(data.getName(), "/32=PA/v=5/seg=0");
  BOOST_CHECK_EQUAL(data.getContentType(), tlv::ContentType_PrefixAnn);
  BOOST_REQUIRE(pa.getData());
  BOOST_CHECK_EQUAL(*pa.getData(), data);

  PrefixAnnouncement decoded(data);
  BOOST_CHECK_EQUAL(decoded.getAnnouncedName(), "/");
  BOOST_CHECK_EQUAL(decoded.getExpiration(), 0_s);
  BOOST_CHECK(!decoded.getValidityPeriod());

  BOOST_CHECK_EQUAL(pa, decoded);
}

BOOST_FIXTURE_TEST_CASE(EncodeNoValidity, KeyChainFixture)
{
  PrefixAnnouncement pa;
  pa.setAnnouncedName("/net/example");
  BOOST_CHECK_THROW(pa.setExpiration(-1_ms), std::invalid_argument);
  pa.setExpiration(1_h);

  const Data& data = pa.toData(m_keyChain, signingWithSha256(), 1);
  BOOST_CHECK_EQUAL(data.getName(), "/net/example/32=PA/v=1/seg=0");
  BOOST_CHECK_EQUAL(data.getContentType(), tlv::ContentType_PrefixAnn);

  const Block& payload = data.getContent();
  payload.parse();
  BOOST_CHECK_EQUAL(readNonNegativeInteger(payload.get(tlv::nfd::ExpirationPeriod)), 3600000);
  BOOST_CHECK(payload.find(tlv::ValidityPeriod) == payload.elements_end());

  PrefixAnnouncement decoded(data);
  BOOST_CHECK_EQUAL(decoded.getAnnouncedName(), "/net/example");
  BOOST_CHECK_EQUAL(decoded.getExpiration(), 1_h);
  BOOST_CHECK(!decoded.getValidityPeriod());

  BOOST_CHECK_EQUAL(pa, decoded);
}

BOOST_FIXTURE_TEST_CASE(EncodeWithValidity, KeyChainFixture)
{
  PrefixAnnouncement pa;
  pa.setAnnouncedName("/net/example");
  pa.setExpiration(1_h);
  security::ValidityPeriod validity(time::fromIsoString("20181030T000000"),
                                    time::fromIsoString("20181124T235959"));
  pa.setValidityPeriod(validity);

  const Data& data = pa.toData(m_keyChain);
  const Block& payload = data.getContent();
  payload.parse();
  BOOST_CHECK_EQUAL(readNonNegativeInteger(payload.get(tlv::nfd::ExpirationPeriod)), 3600000);
  BOOST_CHECK_EQUAL(payload.get(tlv::ValidityPeriod), validity.wireEncode());

  PrefixAnnouncement decoded(data);
  BOOST_CHECK_EQUAL(decoded.getAnnouncedName(), "/net/example");
  BOOST_CHECK_EQUAL(decoded.getExpiration(), 1_h);
  BOOST_REQUIRE(decoded.getValidityPeriod());
  BOOST_CHECK_EQUAL(*decoded.getValidityPeriod(), validity);

  BOOST_CHECK_EQUAL(pa, decoded);
}

BOOST_AUTO_TEST_CASE(Modify)
{
  PrefixAnnouncement pa(makePrefixAnnData());

  PrefixAnnouncement pa0(pa);
  BOOST_REQUIRE(pa0.getData());
  BOOST_CHECK_EQUAL(*pa0.getData(), makePrefixAnnData());
  pa0.setAnnouncedName("/com/example");
  BOOST_CHECK(!pa0.getData());
  BOOST_CHECK_NE(pa0, pa);

  PrefixAnnouncement pa1(makePrefixAnnData());
  pa1.setExpiration(5_min);
  BOOST_CHECK(!pa1.getData());
  BOOST_CHECK_NE(pa1, pa);

  PrefixAnnouncement pa2(makePrefixAnnData());
  pa2.setValidityPeriod(security::ValidityPeriod(time::fromIsoString("20180118T000000"),
                                                 time::fromIsoString("20180212T235959")));
  BOOST_CHECK(!pa2.getData());
  BOOST_CHECK_NE(pa2, pa);
}

BOOST_AUTO_TEST_CASE(KeywordComponent)
{
  BOOST_CHECK_EQUAL(PrefixAnnouncement::getKeywordComponent().wireEncode(),
                    "20 02 5041"_block);
  BOOST_CHECK_EQUAL(PrefixAnnouncement::getKeywordComponent().toUri(name::UriFormat::CANONICAL),
                    "32=PA");
}

BOOST_AUTO_TEST_SUITE_END() // TestPrefixAnnouncement

} // namespace tests
} // namespace ndn
