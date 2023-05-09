/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/util/segmenter.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"

namespace ndn::tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestSegmenter, KeyChainFixture)

BOOST_AUTO_TEST_CASE(Invalid)
{
  Segmenter segmenter(m_keyChain, security::SigningInfo{});
  std::vector<std::shared_ptr<Data>> v;

  const uint8_t arr[] = {1, 2, 3, 4};
  // maxSegmentSize == 0
  BOOST_CHECK_THROW(v = segmenter.segment(arr, "/foo", 0, 1_s), std::invalid_argument);
  std::istringstream ss1("foo");
  BOOST_CHECK_THROW(v = segmenter.segment(ss1, "/foo", 0, 1_s), std::invalid_argument);

  // negative freshnessPeriod
  BOOST_CHECK_THROW(v = segmenter.segment(arr, "/foo", 100, -1_s), std::invalid_argument);
  std::istringstream ss2("foo");
  BOOST_CHECK_THROW(v = segmenter.segment(ss2, "/foo", 100, -1_s), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(EmptyInput)
{
  Segmenter segmenter(m_keyChain, security::SigningInfo{});

  auto check = [] (const auto& v) {
    BOOST_REQUIRE_EQUAL(v.size(), 1);
    BOOST_TEST(v[0]->getName() == "/empty/seg=0");
    BOOST_TEST(v[0]->getContentType() == tlv::ContentType_Nack);
    BOOST_TEST(v[0]->getFreshnessPeriod() == 1_s);
    BOOST_TEST(v[0]->getFinalBlock().value() == name::Component::fromSegment(0));
    BOOST_TEST(v[0]->getContent().value_size() == 0);
  };

  check(segmenter.segment(span<uint8_t>{}, "/empty", 1000, 1_s, tlv::ContentType_Nack));
  std::istringstream ss;
  check(segmenter.segment(ss, "/empty", 1000, 1_s, tlv::ContentType_Nack));
}

BOOST_AUTO_TEST_CASE(OneSegment)
{
  Segmenter segmenter(m_keyChain, security::SigningInfo{});

  auto check = [] (const auto& v, size_t expectedContentSize) {
    BOOST_REQUIRE_EQUAL(v.size(), 1);
    BOOST_TEST(v[0]->getName() == "/one/seg=0");
    BOOST_TEST(v[0]->getContentType() == tlv::ContentType_Blob);
    BOOST_TEST(v[0]->getFreshnessPeriod() == 1_min);
    BOOST_TEST(v[0]->getFinalBlock().value() == name::Component::fromSegment(0));
    BOOST_TEST(v[0]->getContent().value_size() == expectedContentSize);
  };

  const uint8_t arr1[] = {1, 2, 3, 4};
  check(segmenter.segment(arr1, "/one", 10, 1_min), sizeof(arr1));
  std::istringstream ss1("1234");
  check(segmenter.segment(ss1, "/one", 10, 1_min), 4);

  const uint8_t arr2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check(segmenter.segment(arr2, "/one", 10, 1_min), sizeof(arr2));
  std::istringstream ss2("1234567890");
  check(segmenter.segment(ss2, "/one", 10, 1_min), 10);
}

const uint8_t BLOB[] = { // 300 bytes
  0x06, 0xFD, 0x01, 0xBB, 0x07, 0x33, 0x08, 0x03, 0x6E, 0x64, 0x6E, 0x08, 0x05, 0x73, 0x69, 0x74,
  0x65, 0x31, 0x08, 0x03, 0x4B, 0x45, 0x59, 0x08, 0x11, 0x6B, 0x73, 0x6B, 0x2D, 0x31, 0x34, 0x31,
  0x36, 0x34, 0x32, 0x35, 0x33, 0x37, 0x37, 0x30, 0x39, 0x34, 0x08, 0x04, 0x30, 0x31, 0x32, 0x33,
  0x08, 0x07, 0xFD, 0x00, 0x00, 0x01, 0x49, 0xC9, 0x8B, 0x14, 0x09, 0x18, 0x01, 0x02, 0x19, 0x04,
  0x00, 0x36, 0xEE, 0x80, 0x15, 0xA0, 0x30, 0x81, 0x9D, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48,
  0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8B, 0x00, 0x30, 0x81, 0x87, 0x02,
  0x81, 0x81, 0x00, 0x9E, 0x06, 0x3E, 0x47, 0x85, 0xB2, 0x34, 0x37, 0xAA, 0x85, 0x47, 0xAC, 0x03,
  0x24, 0x83, 0xB5, 0x9C, 0xA8, 0x05, 0x3A, 0x24, 0x1E, 0xEB, 0x89, 0x01, 0xBB, 0xE9, 0x9B, 0xB2,
  0xC3, 0x22, 0xAC, 0x68, 0xE3, 0xF0, 0x6C, 0x02, 0xCE, 0x68, 0xA6, 0xC4, 0xD0, 0xA7, 0x06, 0x90,
  0x9C, 0xAA, 0x1B, 0x08, 0x1D, 0x8B, 0x43, 0x9A, 0x33, 0x67, 0x44, 0x6D, 0x21, 0xA3, 0x1B, 0x88,
  0x9A, 0x97, 0x5E, 0x59, 0xC4, 0x15, 0x0B, 0xD9, 0x2C, 0xBD, 0x51, 0x07, 0x61, 0x82, 0xAD, 0xC1,
  0xB8, 0xD7, 0xBF, 0x9B, 0xCF, 0x7D, 0x24, 0xC2, 0x63, 0xF3, 0x97, 0x17, 0xEB, 0xFE, 0x62, 0x25,
  0xBA, 0x5B, 0x4D, 0x8A, 0xC2, 0x7A, 0xBD, 0x43, 0x8A, 0x8F, 0xB8, 0xF2, 0xF1, 0xC5, 0x6A, 0x30,
  0xD3, 0x50, 0x8C, 0xC8, 0x9A, 0xDF, 0xEF, 0xED, 0x35, 0xE7, 0x7A, 0x62, 0xEA, 0x76, 0x7C, 0xBB,
  0x08, 0x26, 0xC7, 0x02, 0x01, 0x11, 0x16, 0x55, 0x1B, 0x01, 0x01, 0x1C, 0x26, 0x07, 0x24, 0x08,
  0x03, 0x6E, 0x64, 0x6E, 0x08, 0x05, 0x73, 0x69, 0x74, 0x65, 0x31, 0x08, 0x03, 0x4B, 0x45, 0x59,
  0x08, 0x11, 0x6B, 0x73, 0x6B, 0x2D, 0x32, 0x35, 0x31, 0x36, 0x34, 0x32, 0x35, 0x33, 0x37, 0x37,
  0x30, 0x39, 0x34, 0xFD, 0x00, 0xFD, 0x26, 0xFD, 0x00, 0xFE, 0x0F, 0x17, 0x80, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

BOOST_AUTO_TEST_CASE(MultipleSegments)
{
  Segmenter segmenter(m_keyChain, security::SigningInfo{});

  auto check = [] (const auto& v) {
    BOOST_TEST(v.size() == 8);

    size_t segNo = 0;
    for (const auto& seg : v) {
      BOOST_TEST(seg->getName().getPrefix(-1) == "/many");
      BOOST_TEST(seg->getName().at(-1).toSegment() == segNo);
      BOOST_TEST(seg->getContentType() == tlv::ContentType_Blob);
      BOOST_TEST(seg->getFreshnessPeriod() == 30_s);
      BOOST_TEST(seg->getFinalBlock().value() == name::Component::fromSegment(7));
      // last segment is shorter
      const size_t expectedContentSize = segNo == 7 ? (sizeof(BLOB) % 42) : 42;
      BOOST_TEST(seg->getContent().value_bytes() == make_span(BLOB).subspan(segNo * 42, expectedContentSize),
                 boost::test_tools::per_element());
      segNo++;
    }
  };

  check(segmenter.segment(BLOB, "/many", 42, 30_s));
  std::istringstream ss(std::string(reinterpret_cast<const char*>(BLOB), sizeof(BLOB)));
  check(segmenter.segment(ss, "/many", 42, 30_s));
}

BOOST_AUTO_TEST_SUITE_END() // TestSegmenter
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace ndn::tests
