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

#include "lp/packet.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace lp {
namespace tests {

BOOST_AUTO_TEST_SUITE(LpPacket)

BOOST_AUTO_TEST_CASE(FieldAccess)
{
  Packet packet;

  BOOST_CHECK(!packet.has<FragIndexField>());
  BOOST_CHECK_EQUAL(0, packet.count<FragIndexField>());
  BOOST_CHECK_NO_THROW(packet.set<FragIndexField>(1234));
  BOOST_CHECK(packet.has<FragIndexField>());
  BOOST_CHECK_THROW(packet.add<FragIndexField>(5678), std::length_error);
  BOOST_CHECK_EQUAL(1, packet.count<FragIndexField>());
  BOOST_CHECK_EQUAL(1234, packet.get<FragIndexField>(0));
  BOOST_CHECK_THROW(packet.get<FragIndexField>(1), std::out_of_range);
  BOOST_CHECK_THROW(packet.remove<FragIndexField>(1), std::out_of_range);
  BOOST_CHECK_NO_THROW(packet.remove<FragIndexField>(0));
  BOOST_CHECK_EQUAL(0, packet.count<FragIndexField>());
  BOOST_CHECK_NO_THROW(packet.add<FragIndexField>(832));
  std::vector<uint64_t> fragIndexes;
  BOOST_REQUIRE_NO_THROW(fragIndexes = packet.list<FragIndexField>());
  BOOST_CHECK_EQUAL(1, fragIndexes.size());
  BOOST_CHECK_EQUAL(832, fragIndexes.at(0));
  BOOST_CHECK_NO_THROW(packet.clear<FragIndexField>());
  BOOST_CHECK_EQUAL(0, packet.count<FragIndexField>());
}

/// \todo test field access methods with a REPEATABLE field

BOOST_AUTO_TEST_CASE(EncodeFragment)
{
  static const uint8_t expectedBlock[] = {
    0x64, 0x04, // LpPacket
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Buffer buf(2);
  buf[0] = 0x03;
  buf[1] = 0xe8;

  Packet packet;
  BOOST_CHECK_NO_THROW(packet.add<FragmentField>(std::make_pair(buf.begin(), buf.end())));
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = packet.wireEncode());
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(EncodeSubTlv)
{
  static const uint8_t expectedBlock[] = {
    0x64, 0x09, // LpPacket
          0xfd, 0x03, 0x20, 0x05, // Nack
                0xfd, 0x03, 0x21, 0x01, // NackReason
                      0x64,
  };

  NackHeader nack;
  nack.setReason(NackReason::DUPLICATE);

  Packet packet;
  BOOST_CHECK_NO_THROW(packet.add<NackField>(nack));
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = packet.wireEncode());
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(EncodeSortOrder)
{
  static const uint8_t expectedBlock[] = {
    0x64, 0x0a, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0x53, 0x01, // FragCount
                0x01,
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Buffer frag(2);
  frag[0] = 0x03;
  frag[1] = 0xe8;

  Packet packet;
  BOOST_CHECK_NO_THROW(packet.add<FragmentField>(std::make_pair(frag.begin(), frag.end())));
  BOOST_CHECK_NO_THROW(packet.add<FragIndexField>(0));
  BOOST_CHECK_NO_THROW(packet.add<FragCountField>(1));
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = packet.wireEncode());
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(DecodeNormal)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x0a, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0x53, 0x01, // FragCount
                0x01,
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_NO_THROW(packet.wireDecode(wire));
  BOOST_CHECK_EQUAL(1, packet.count<FragmentField>());
  BOOST_CHECK_EQUAL(1, packet.count<FragIndexField>());
  BOOST_CHECK_EQUAL(1, packet.count<FragCountField>());
  Buffer::const_iterator first, last;
  BOOST_REQUIRE_NO_THROW(std::tie(first, last) = packet.get<FragmentField>(0));
  BOOST_CHECK_EQUAL(2, last - first);
  BOOST_CHECK_EQUAL(0x03, *first);
  BOOST_CHECK_EQUAL(0xe8, *(last - 1));
  BOOST_CHECK_EQUAL(0, packet.get<FragIndexField>(0));
  BOOST_CHECK_EQUAL(1, packet.get<FragCountField>(0));
}

BOOST_AUTO_TEST_CASE(DecodeIdle)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x06, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0x53, 0x01, // FragCount
                0x01,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_NO_THROW(packet.wireDecode(wire));
  BOOST_CHECK_EQUAL(0, packet.count<FragmentField>());
  BOOST_CHECK_EQUAL(1, packet.count<FragIndexField>());
  BOOST_CHECK_EQUAL(1, packet.count<FragCountField>());
  BOOST_CHECK_EQUAL(0, packet.get<FragIndexField>(0));
  BOOST_CHECK_EQUAL(1, packet.get<FragCountField>(0));
}

BOOST_AUTO_TEST_CASE(DecodeFragment)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x04, // LpPacket
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_NO_THROW(packet.wireDecode(wire));
  BOOST_CHECK_EQUAL(1, packet.count<FragmentField>());
  BOOST_CHECK_EQUAL(0, packet.count<FragIndexField>());
  Buffer::const_iterator first, last;
  BOOST_REQUIRE_NO_THROW(std::tie(first, last) = packet.get<FragmentField>(0));
  BOOST_CHECK_EQUAL(2, last - first);
  BOOST_CHECK_EQUAL(0x03, *first);
  BOOST_CHECK_EQUAL(0xe8, *(last - 1));
}

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x00, // LpPacket
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_NO_THROW(packet.wireDecode(wire));
  BOOST_CHECK_EQUAL(0, packet.count<FragmentField>());
  BOOST_CHECK_EQUAL(0, packet.count<FragIndexField>());
}

BOOST_AUTO_TEST_CASE(DecodeRepeatedNonRepeatableHeader)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x06, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0x52, 0x01, // FragIndex
                0x01,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_THROW(packet.wireDecode(wire), Packet::Error);
}

BOOST_AUTO_TEST_CASE(DecodeRepeatedFragment)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x08, // LpPacket
          0x50, 0x02, // Fragment
                0x03, 0xe8,
          0x50, 0x02, // Fragment
                0x03, 0xe9,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_THROW(packet.wireDecode(wire), Packet::Error);
}

BOOST_AUTO_TEST_CASE(DecodeWrongOrderAmongHeaders)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x0a, // LpPacket
          0x53, 0x01, // FragCount
                0x01,
          0x52, 0x01, // FragIndex
                0x00,
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_THROW(packet.wireDecode(wire), Packet::Error);
}

BOOST_AUTO_TEST_CASE(DecodeWrongOrderFragment)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x0a, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0x50, 0x02, // Fragment
                0x03, 0xe8,
          0x53, 0x01, // FragCount
                0x01,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_THROW(packet.wireDecode(wire), Packet::Error);
}

BOOST_AUTO_TEST_CASE(DecodeIgnoredHeader)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x0c, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0xfd, 0x03, 0x23, 0x01, // unknown TLV-TYPE 803 (ignored)
                0x02,
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_NO_THROW(packet.wireDecode(wire));
  BOOST_CHECK_EQUAL(1, packet.count<FragmentField>());
  BOOST_CHECK_EQUAL(1, packet.count<FragIndexField>());
}

BOOST_AUTO_TEST_CASE(DecodeUnrecognizedHeader)
{
  static const uint8_t inputBlock[] = {
    0x64, 0x0c, // LpPacket
          0x52, 0x01, // FragIndex
                0x00,
          0xfd, 0x03, 0x22, 0x01, // unknown TLV-TYPE 802 (cannot ignore)
                0x02,
          0x50, 0x02, // Fragment
                0x03, 0xe8,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_THROW(packet.wireDecode(wire), Packet::Error);
}

BOOST_AUTO_TEST_CASE(DecodeBareNetworkLayerPacket)
{
  static const uint8_t inputBlock[] = {
    0x05, 0x0a, // Interest
          0x07, 0x02, // Name
                0x03, 0xe8,
          0x0a, 0x04, // Nonce
                0x01, 0x02, 0x03, 0x04,
  };

  Packet packet;
  Block wire(inputBlock, sizeof(inputBlock));
  BOOST_CHECK_NO_THROW(packet.wireDecode(wire));
  BOOST_CHECK_EQUAL(1, packet.count<FragmentField>());

  static const uint8_t expectedBlock[] = {
    0x64, 0x0e, // LpPacket
          0x50, 0x0c, // Fragment
                0x05, 0x0a, // Interest
                      0x07, 0x02, // Name
                            0x03, 0xe8,
                      0x0a, 0x04, // Nonce
                            0x01, 0x02, 0x03, 0x04,
  };

  Block encoded;
  BOOST_CHECK_NO_THROW(encoded = packet.wireEncode());
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                encoded.begin(), encoded.end());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace lp
} // namespace ndn