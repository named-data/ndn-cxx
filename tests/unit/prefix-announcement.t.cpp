/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "tests/boost-test.hpp"
#include "tests/identity-management-fixture.hpp"
#include "tests/make-interest-data.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestPrefixAnnouncement)

static Data
makePrefixAnnData()
{
  return Data(
    "067A 071A announced-name=/net/example 08036E6574 08076578616D706C65"
    "          keyword-prefix-ann=20025041 version=0802FD01 segment=08020000"
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
  BOOST_CHECK_THROW(PrefixAnnouncement pa0(data0), tlv::Error);

  // Name has no "32=PA" keyword
  Data data1 = makePrefixAnnData();
  setNameComponent(data1, -3, name::Component::fromEscapedString("32=not-PA"));
  BOOST_CHECK_THROW(PrefixAnnouncement pa1(data1), tlv::Error);

  // Name has no version component
  Data data2 = makePrefixAnnData();
  setNameComponent(data2, -2, "not-version");
  BOOST_CHECK_THROW(PrefixAnnouncement pa2(data2), tlv::Error);

  // Name has no segment number component
  Data data3 = makePrefixAnnData();
  setNameComponent(data3, -2, "not-segment");
  BOOST_CHECK_THROW(PrefixAnnouncement pa3(data3), tlv::Error);

  // Content has no ExpirationPeriod element
  Data data4 = makePrefixAnnData();
  Block payload4 = data4.getContent();
  payload4.parse();
  payload4.remove(tlv::nfd::ExpirationPeriod);
  payload4.encode();
  data4.setContent(payload4);
  BOOST_CHECK_THROW(PrefixAnnouncement pa4(data4), tlv::Error);

  // ExpirationPeriod is malformed
  Data data5 = makePrefixAnnData();
  Block payload5 = data5.getContent();
  payload5.parse();
  payload5.remove(tlv::nfd::ExpirationPeriod);
  payload5.push_back("6D03010101"_block);
  payload5.encode();
  data5.setContent(payload5);
  BOOST_CHECK_THROW(PrefixAnnouncement pa5(data5), tlv::Error);

  // ValidityPeriod is malformed
  Data data6 = makePrefixAnnData();
  Block payload6 = data6.getContent();
  payload6.parse();
  payload6.remove(tlv::ValidityPeriod);
  payload6.push_back("FD00FD00"_block);
  payload6.encode();
  data6.setContent(payload6);
  BOOST_CHECK_THROW(PrefixAnnouncement pa6(data6), tlv::Error);

  // Content has unrecognized critical element
  Data data7 = makePrefixAnnData();
  Block payload7 = data7.getContent();
  payload7.parse();
  payload7.push_back("0200"_block);
  payload7.encode();
  data7.setContent(payload7);
  BOOST_CHECK_THROW(PrefixAnnouncement pa7(data7), tlv::Error);
}

BOOST_FIXTURE_TEST_CASE(EncodeEmpty, IdentityManagementFixture)
{
  PrefixAnnouncement pa;
  BOOST_CHECK(!pa.getData());
  BOOST_CHECK_EQUAL(pa.getAnnouncedName(), "/");
  BOOST_CHECK_EQUAL(pa.getExpiration(), 0_ms);
  BOOST_CHECK(!pa.getValidityPeriod());

  const Data& data = pa.toData(m_keyChain, signingWithSha256(), 5);
  BOOST_CHECK_EQUAL(data.getName(), "/32=PA/%FD%05/%00%00");
  BOOST_CHECK_EQUAL(data.getContentType(), tlv::ContentType_PrefixAnn);
  BOOST_REQUIRE(pa.getData());
  BOOST_CHECK_EQUAL(*pa.getData(), data);

  PrefixAnnouncement decoded(data);
  BOOST_CHECK_EQUAL(decoded.getAnnouncedName(), "/");
  BOOST_CHECK_EQUAL(decoded.getExpiration(), 0_s);
  BOOST_CHECK(!decoded.getValidityPeriod());

  BOOST_CHECK_EQUAL(pa, decoded);
}

BOOST_FIXTURE_TEST_CASE(EncodeNoValidity, IdentityManagementFixture)
{
  PrefixAnnouncement pa;
  pa.setAnnouncedName("/net/example");
  BOOST_CHECK_THROW(pa.setExpiration(-1_ms), std::invalid_argument);
  pa.setExpiration(1_h);

  const Data& data = pa.toData(m_keyChain, signingWithSha256(), 1);
  BOOST_CHECK_EQUAL(data.getName(), "/net/example/32=PA/%FD%01/%00%00");
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

BOOST_FIXTURE_TEST_CASE(EncodeWithValidity, IdentityManagementFixture)
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

BOOST_AUTO_TEST_SUITE_END() // TestPrefixAnnouncement

} // namespace tests
} // namespace ndn
