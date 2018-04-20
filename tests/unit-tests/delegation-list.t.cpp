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

#include "delegation-list.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestDelegationList)

const uint8_t DEL1A[] = {
  0x1f, 0x08, // Delegation
        0x1e, 0x01, 0x01, // Preference=1
        0x07, 0x03, 0x08, 0x01, 0x41 // Name=/A
};
const uint8_t DEL1B[] = {
  0x1f, 0x08, // Delegation
        0x1e, 0x01, 0x01, // Preference=1
        0x07, 0x03, 0x08, 0x01, 0x42 // Name=/B
};
const uint8_t DEL2A[] = {
  0x1f, 0x08, // Delegation
        0x1e, 0x01, 0x02, // Preference=2
        0x07, 0x03, 0x08, 0x01, 0x41 // Name=/A
};
const uint8_t DEL2B[] = {
  0x1f, 0x08, // Delegation
        0x1e, 0x01, 0x02, // Preference=2
        0x07, 0x03, 0x08, 0x01, 0x42 // Name=/B
};

Block
makeDelegationListBlock(uint32_t type, std::initializer_list<const uint8_t*> dels)
{
  Block block(type);
  for (const uint8_t* del : dels) {
    block.push_back(Block(del, 2 + del[1]));
  }
  block.encode();
  return block;
}

BOOST_AUTO_TEST_SUITE(Decode)

BOOST_AUTO_TEST_CASE(DecodeUnsorted)
{
  DelegationList dl(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL2B, DEL1A}), false);
  BOOST_CHECK_EQUAL(dl.size(), 3);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(0).name, "/A");
  BOOST_CHECK_EQUAL(dl.at(1).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(1).name, "/B");
  BOOST_CHECK_EQUAL(dl.at(2).preference, 1);
  BOOST_CHECK_EQUAL(dl.at(2).name, "/A");
}

BOOST_AUTO_TEST_CASE(DecodeSorted)
{
  DelegationList dl(makeDelegationListBlock(tlv::Content, {DEL2A, DEL2B, DEL1A}));
  BOOST_CHECK_EQUAL(dl.size(), 3);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 1);
  BOOST_CHECK_EQUAL(dl.at(0).name, "/A");
  BOOST_CHECK_EQUAL(dl.at(1).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(1).name, "/A");
  BOOST_CHECK_EQUAL(dl.at(2).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(2).name, "/B");
}

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::ForwardingHint, {});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_CASE(DecodeBadType)
{
  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::Selectors, {DEL1A, DEL2B});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_CASE(DecodeNotDelegation)
{
  const uint8_t BAD_DEL[] = {
    0x09, 0x00 // Selectors
  };

  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::ForwardingHint, {DEL1A, BAD_DEL});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_CASE(DecodeMissingPreference)
{
  const uint8_t BAD_DEL[] = {
    0x1f, 0x05, // Delegation
          0x07, 0x03, 0x08, 0x01, 0x42 // Name=/B
  };

  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::ForwardingHint, {DEL1A, BAD_DEL});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_CASE(DecodeMissingName)
{
  const uint8_t BAD_DEL[] = {
    0x1f, 0x03, // Delegation
          0x1e, 0x01, 0x02, // Preference=2
  };

  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::ForwardingHint, {DEL1A, BAD_DEL});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_CASE(DecodeUnknownField)
{
  const uint8_t BAD_DEL[] = {
    0x1f, 0x0a, // Delegation
          0x1e, 0x01, 0x02, // Preference=2
          0x09, 0x00, // Selectors
          0x07, 0x03, 0x08, 0x01, 0x42 // Name=/B
  };

  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::ForwardingHint, {DEL1A, BAD_DEL});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_CASE(DecodeWrongOrder)
{
  const uint8_t BAD_DEL[] = {
    0x1f, 0x08, // Delegation
          0x07, 0x03, 0x08, 0x01, 0x42, // Name=/B
          0x1e, 0x01, 0x02 // Preference=2
  };

  DelegationList dl;
  Block block = makeDelegationListBlock(tlv::ForwardingHint, {DEL1A, BAD_DEL});
  BOOST_CHECK_THROW(dl.wireDecode(block), DelegationList::Error);
}

BOOST_AUTO_TEST_SUITE_END() // Decode

BOOST_AUTO_TEST_SUITE(InsertEncode)

BOOST_AUTO_TEST_CASE(InsertSimple)
{
  DelegationList dl;
  BOOST_CHECK_EQUAL(dl.empty(), true);
  dl.insert(2, "/A");
  BOOST_CHECK_EQUAL(dl.empty(), false);
  dl.insert(1, "/B");
  BOOST_CHECK_EQUAL(dl.size(), 2);

  EncodingBuffer encoder;
  dl.wireEncode(encoder);
  BOOST_CHECK_EQUAL(encoder.block(), makeDelegationListBlock(tlv::ForwardingHint, {DEL1B, DEL2A}));
}

BOOST_AUTO_TEST_CASE(InsertReplace)
{
  DelegationList dl({{2, "/A"}});
  dl.insert(Delegation{1, "/A"}, DelegationList::INS_REPLACE);
  BOOST_CHECK_EQUAL(dl.size(), 1);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 1);
  BOOST_CHECK_EQUAL(dl[0].name, "/A");

  EncodingBuffer encoder;
  dl.wireEncode(encoder);
  BOOST_CHECK_EQUAL(encoder.block(), makeDelegationListBlock(tlv::ForwardingHint, {DEL1A}));
}

BOOST_AUTO_TEST_CASE(InsertAppend)
{
  DelegationList dl({{2, "/A"}});
  dl.insert(Delegation{1, "/A"}, DelegationList::INS_APPEND);
  BOOST_CHECK_EQUAL(dl.size(), 2);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 1);
  BOOST_CHECK_EQUAL(dl.at(1).preference, 2);

  EncodingBuffer encoder;
  dl.wireEncode(encoder);
  BOOST_CHECK_EQUAL(encoder.block(), makeDelegationListBlock(tlv::ForwardingHint, {DEL1A, DEL2A}));
}

BOOST_AUTO_TEST_CASE(InsertSkip)
{
  DelegationList dl({{2, "/A"}});
  dl.insert(Delegation{1, "/A"}, DelegationList::INS_SKIP);
  BOOST_CHECK_EQUAL(dl.size(), 1);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 2);

  EncodingBuffer encoder;
  dl.wireEncode(encoder);
  BOOST_CHECK_EQUAL(encoder.block(), makeDelegationListBlock(tlv::ForwardingHint, {DEL2A}));
}

BOOST_AUTO_TEST_CASE(Unsorted)
{
  DelegationList dl(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A}), false);
  dl.insert(1, "/B");
  BOOST_CHECK_EQUAL(dl.size(), 2);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(0).name, "/A");
  BOOST_CHECK_EQUAL(dl.at(1).preference, 1);
  BOOST_CHECK_EQUAL(dl.at(1).name, "/B");

  EncodingBuffer encoder;
  dl.wireEncode(encoder, tlv::Content);
  BOOST_CHECK_EQUAL(encoder.block(), makeDelegationListBlock(tlv::Content, {DEL2A, DEL1B}));
}

BOOST_AUTO_TEST_CASE(EncodeBadType)
{
  DelegationList dl(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A}));
  EncodingBuffer encoder;
  BOOST_CHECK_THROW(dl.wireEncode(encoder, tlv::Selectors), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(EncodeEmpty)
{
  DelegationList dl;
  EncodingBuffer encoder;
  BOOST_CHECK_THROW(dl.wireEncode(encoder), DelegationList::Error);
}

BOOST_AUTO_TEST_SUITE_END() // InsertEncode

BOOST_AUTO_TEST_SUITE(Erase)

BOOST_AUTO_TEST_CASE(EraseNoop)
{
  DelegationList dl;
  dl.insert(1, "/A");
  BOOST_CHECK_EQUAL(dl.erase(2, "/A"), 0);
  BOOST_CHECK_EQUAL(dl.erase(Delegation{1, "/B"}), 0);
  BOOST_CHECK_EQUAL(dl.size(), 1);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 1);
  BOOST_CHECK_EQUAL(dl.at(0).name, "/A");
}

BOOST_AUTO_TEST_CASE(EraseOne)
{
  DelegationList dl;
  dl.insert(1, "/A");
  BOOST_CHECK_EQUAL(dl.erase(1, "/A"), 1);
  BOOST_CHECK_EQUAL(dl.size(), 0);
}

BOOST_AUTO_TEST_CASE(EraseByName)
{
  DelegationList dl;
  dl.insert(1, "/A");
  dl.insert(2, "/A", DelegationList::INS_APPEND);
  BOOST_CHECK_EQUAL(dl.size(), 2);
  BOOST_CHECK_EQUAL(dl.erase("/A"), 2);
  BOOST_CHECK_EQUAL(dl.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // Erase

BOOST_AUTO_TEST_SUITE(Sort)

BOOST_AUTO_TEST_CASE(Noop)
{
  DelegationList dl(makeDelegationListBlock(tlv::ForwardingHint, {DEL1A}));
  BOOST_CHECK_EQUAL(dl.isSorted(), true);
  dl.sort();
  BOOST_CHECK_EQUAL(dl.isSorted(), true);
}

BOOST_AUTO_TEST_CASE(Sort)
{
  DelegationList dl(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL2B, DEL1A}), false);
  BOOST_CHECK_EQUAL(dl.isSorted(), false);
  dl.sort();
  BOOST_CHECK_EQUAL(dl.isSorted(), true);
  BOOST_CHECK_EQUAL(dl.size(), 3);
  BOOST_CHECK_EQUAL(dl.at(0).preference, 1);
  BOOST_CHECK_EQUAL(dl.at(0).name, "/A");
  BOOST_CHECK_EQUAL(dl.at(1).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(1).name, "/A");
  BOOST_CHECK_EQUAL(dl.at(2).preference, 2);
  BOOST_CHECK_EQUAL(dl.at(2).name, "/B");
}

BOOST_AUTO_TEST_SUITE_END() // Sort

BOOST_AUTO_TEST_SUITE(Compare)

BOOST_AUTO_TEST_CASE(Empty)
{
  DelegationList dl1, dl2;
  BOOST_CHECK_EQUAL(dl1, dl2);
}

BOOST_AUTO_TEST_CASE(SortedEqual)
{
  DelegationList dl1(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL1B})),
                 dl2(makeDelegationListBlock(tlv::Content, {DEL1B, DEL2A}));
  BOOST_CHECK_EQUAL(dl1, dl2);
}

BOOST_AUTO_TEST_CASE(SortedUnequal)
{
  DelegationList dl1(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL1B})),
                 dl2(makeDelegationListBlock(tlv::Content, {DEL1A, DEL2B}));
  BOOST_CHECK_NE(dl1, dl2);
}

BOOST_AUTO_TEST_CASE(UnsortedSameOrder)
{
  DelegationList dl1(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL1B}), false),
                 dl2(makeDelegationListBlock(tlv::Content, {DEL2A, DEL1B}), false);
  BOOST_CHECK_EQUAL(dl1, dl2);
}

BOOST_AUTO_TEST_CASE(UnsortedDifferentOrder)
{
  DelegationList dl1(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL1B}), false),
                 dl2(makeDelegationListBlock(tlv::Content, {DEL1B, DEL2A}), false);
  BOOST_CHECK_NE(dl1, dl2);
}

BOOST_AUTO_TEST_SUITE_END() // Compare

BOOST_AUTO_TEST_SUITE(Print)

BOOST_AUTO_TEST_CASE(PrintEmpty)
{
  DelegationList dl;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(dl), "[]");
}

BOOST_AUTO_TEST_CASE(PrintNormal)
{
  DelegationList dl(makeDelegationListBlock(tlv::ForwardingHint, {DEL2A, DEL1B}));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(dl), "[/B(1),/A(2)]");
}

BOOST_AUTO_TEST_SUITE_END() // Print

BOOST_AUTO_TEST_SUITE_END() // TestDelegationList

} // namespace tests
} // namespace ndn
