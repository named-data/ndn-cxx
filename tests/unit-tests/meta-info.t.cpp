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

#include "meta-info.hpp"

#include "data.hpp"
#include "security/key-chain.hpp"
#include "security/cryptopp.hpp"
#include "encoding/buffer-stream.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestMetaInfo)

const uint8_t MetaInfo1[] = {0x14, 0x04, 0x19, 0x02, 0x27, 0x10};
const uint8_t MetaInfo2[] = {0x14, 0x14, 0x19, 0x02, 0x27, 0x10, 0x1a, 0x0e, 0x08, 0x0c,
                             0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x77, 0x6f, 0x72, 0x6c,
                             0x64, 0x21};
const uint8_t MetaInfo3[] = {0x14, 0x17, 0x18, 0x01, 0x01, 0x19, 0x02, 0x27, 0x10, 0x1a,
                             0x0e, 0x08, 0x0c, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x77,
                             0x6f, 0x72, 0x6c, 0x64, 0x21};

BOOST_AUTO_TEST_CASE(Encode)
{
  MetaInfo meta;
  meta.setType(tlv::ContentType_Blob);
  meta.setFreshnessPeriod(time::seconds(10));

  BOOST_REQUIRE_NO_THROW(meta.wireEncode());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(MetaInfo1, MetaInfo1+sizeof(MetaInfo1),
                                  meta.wireEncode().begin(), meta.wireEncode().end());

  meta.setFinalBlockId(name::Component("hello,world!"));
  BOOST_REQUIRE_NO_THROW(meta.wireEncode());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(MetaInfo2, MetaInfo2+sizeof(MetaInfo2),
                                  meta.wireEncode().begin(), meta.wireEncode().end());

  meta.setType(tlv::ContentType_Link);
  BOOST_REQUIRE_NO_THROW(meta.wireEncode());
  BOOST_REQUIRE_EQUAL_COLLECTIONS(MetaInfo3, MetaInfo3+sizeof(MetaInfo3),
                                  meta.wireEncode().begin(), meta.wireEncode().end());
}

BOOST_AUTO_TEST_CASE(Decode)
{
  MetaInfo meta(Block(MetaInfo1, sizeof(MetaInfo1)));
  BOOST_CHECK_EQUAL(meta.getType(), static_cast<uint32_t>(tlv::ContentType_Blob));
  BOOST_CHECK_EQUAL(meta.getFreshnessPeriod(), time::seconds(10));
  BOOST_CHECK_EQUAL(meta.getFinalBlockId(), name::Component());

  meta.wireDecode(Block(MetaInfo2, sizeof(MetaInfo2)));
  BOOST_CHECK_EQUAL(meta.getType(), static_cast<uint32_t>(tlv::ContentType_Blob));
  BOOST_CHECK_EQUAL(meta.getFreshnessPeriod(), time::seconds(10));
  BOOST_CHECK_EQUAL(meta.getFinalBlockId(), name::Component("hello,world!"));

  meta.wireDecode(Block(MetaInfo3, sizeof(MetaInfo3)));
  BOOST_CHECK_EQUAL(meta.getType(), static_cast<uint32_t>(tlv::ContentType_Link));
  BOOST_CHECK_EQUAL(meta.getFreshnessPeriod(), time::seconds(10));
  BOOST_CHECK_EQUAL(meta.getFinalBlockId(), name::Component("hello,world!"));
}

BOOST_AUTO_TEST_CASE(EqualityChecks)
{
  using namespace time;

  MetaInfo a;
  MetaInfo b;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setFreshnessPeriod(seconds(10));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setFreshnessPeriod(milliseconds(90000));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setFreshnessPeriod(milliseconds(10000));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setType(10);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setType(10);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_CASE(AppMetaInfo)
{
  MetaInfo info1;
  info1.setType(196);
  info1.setFreshnessPeriod(time::milliseconds(3600));
  info1.setFinalBlockId(name::Component("/att/final"));

  uint32_t ints[5] = {128, 129, 130, 131, 132};
  std::string ss[5] = {"h", "hello", "hello, world", "hello, world, alex",
                       "hello, world, alex, I am Xiaoke Jiang"};

  for (int i = 0; i < 5; i++) {
    uint32_t type = 128 + i * 10;
    info1.addAppMetaInfo(makeNonNegativeIntegerBlock(type, ints[i]));
    const std::string& s = ss[i];
    type += 5;
    info1.addAppMetaInfo(makeStringBlock(type, s));
  }

  BOOST_CHECK(info1.findAppMetaInfo(252) == 0);

  info1.addAppMetaInfo(makeNonNegativeIntegerBlock(252, 1000));
  BOOST_CHECK(info1.findAppMetaInfo(252) != 0);

  info1.addAppMetaInfo(makeNonNegativeIntegerBlock(252, 1000));
  BOOST_CHECK(info1.findAppMetaInfo(252) != 0);

  info1.removeAppMetaInfo(252);
  BOOST_CHECK(info1.findAppMetaInfo(252) != 0);

  info1.removeAppMetaInfo(252);
  BOOST_CHECK(info1.findAppMetaInfo(252) == 0);

  // // These octets are obtained by the snippet below.
  // // This check is intended to detect unexpected encoding change in the future.
  // const Block& wire = info1.wireEncode();
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }

  const uint8_t METAINFO[] = {0x14, 0x77, 0x18, 0x01, 0xc4, 0x19, 0x02, 0x0e, 0x10, 0x1a, 0x0c,
                              0x08, 0x0a, 0x2f, 0x61, 0x74, 0x74, 0x2f, 0x66, 0x69, 0x6e, 0x61,
                              0x6c, 0x80, 0x01, 0x80, 0x85, 0x01, 0x68, 0x8a, 0x01, 0x81, 0x8f,
                              0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x94, 0x01, 0x82, 0x99, 0x0c,
                              0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c,
                              0x64, 0x9e, 0x01, 0x83, 0xa3, 0x12, 0x68, 0x65, 0x6c, 0x6c, 0x6f,
                              0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2c, 0x20, 0x61, 0x6c,
                              0x65, 0x78, 0xa8, 0x01, 0x84, 0xad, 0x25, 0x68, 0x65, 0x6c, 0x6c,
                              0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2c, 0x20, 0x61,
                              0x6c, 0x65, 0x78, 0x2c, 0x20, 0x49, 0x20, 0x61, 0x6d, 0x20, 0x58,
                              0x69, 0x61, 0x6f, 0x6b, 0x65, 0x20, 0x4a, 0x69, 0x61, 0x6e, 0x67};

  BOOST_REQUIRE_EQUAL_COLLECTIONS(info1.wireEncode().begin(), info1.wireEncode().end(),
                                  METAINFO, METAINFO + sizeof(METAINFO));

  MetaInfo info2;
  info2.wireDecode(Block(METAINFO, sizeof(METAINFO)));

  for (int i = 0; i < 5; i++) {
    uint32_t tlvType = 128 + i * 10;
    const Block* block = info2.findAppMetaInfo(tlvType);
    BOOST_REQUIRE(block != 0);
    BOOST_CHECK_EQUAL(readNonNegativeInteger(*block), ints[i]);
    tlvType += 5;

    block = info2.findAppMetaInfo(tlvType);
    BOOST_REQUIRE(block != 0);

    std::string s3 = std::string(reinterpret_cast<const char*>(block->value()),
                                 block->value_size());
    BOOST_CHECK_EQUAL(s3, ss[i]);
  }
}

BOOST_AUTO_TEST_CASE(AppMetaInfoTypeRange)
{
  MetaInfo info;

  BOOST_CHECK_NO_THROW(info.addAppMetaInfo(makeNonNegativeIntegerBlock(128, 1000)));
  BOOST_CHECK_NO_THROW(info.addAppMetaInfo(makeNonNegativeIntegerBlock(252, 1000)));

  BOOST_CHECK_THROW(info.addAppMetaInfo(makeNonNegativeIntegerBlock(127, 1000)), MetaInfo::Error);
  BOOST_CHECK_THROW(info.addAppMetaInfo(makeNonNegativeIntegerBlock(253, 1000)), MetaInfo::Error);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
