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

#include "encoding/encoding-buffer.hpp"
#include "encoding/buffer-stream.hpp"
#include "encoding/block-helpers.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(EncodingBlock)

class BasicBlockFixture
{
public:
  EncodingBuffer buffer;
  EncodingEstimator estimator;
};

BOOST_FIXTURE_TEST_SUITE(Basic, BasicBlockFixture)

BOOST_AUTO_TEST_CASE(VarNumberOneByte1)
{
  size_t s1 = buffer.prependVarNumber(252);
  size_t s2 = estimator.prependVarNumber(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
}

BOOST_AUTO_TEST_CASE(VarNumberOneByte2)
{
  size_t s1 = buffer.prependVarNumber(253);
  size_t s2 = estimator.prependVarNumber(253);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
}

BOOST_AUTO_TEST_CASE(VarNumberThreeBytes1)
{
  size_t s1 = buffer.prependVarNumber(255);
  size_t s2 = estimator.prependVarNumber(255);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
}

BOOST_AUTO_TEST_CASE(VarNumberThreeBytes2)
{
  size_t s1 = buffer.prependVarNumber(65535);
  size_t s2 = estimator.prependVarNumber(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
}

BOOST_AUTO_TEST_CASE(VarNumberFiveBytes1)
{
  size_t s1 = buffer.prependVarNumber(65536);
  size_t s2 = estimator.prependVarNumber(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
}

BOOST_AUTO_TEST_CASE(VarNumberFiveBytes2)
{
  size_t s1 = buffer.prependVarNumber(4294967295LL);
  size_t s2 = estimator.prependVarNumber(4294967295LL);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
}

BOOST_AUTO_TEST_CASE(VarNumberNineBytes)
{
  size_t s1 = buffer.prependVarNumber(4294967296LL);
  size_t s2 = estimator.prependVarNumber(4294967296LL);
  BOOST_CHECK_EQUAL(buffer.size(), 9);
  BOOST_CHECK_EQUAL(s1, 9);
  BOOST_CHECK_EQUAL(s2, 9);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberOneByte1)
{
  size_t s1 = buffer.prependNonNegativeInteger(252);
  size_t s2 = estimator.prependNonNegativeInteger(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberOneByte2)
{
  size_t s1 = buffer.prependNonNegativeInteger(255);
  size_t s2 = estimator.prependNonNegativeInteger(255);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberTwoBytes1)
{
  size_t s1 = buffer.prependNonNegativeInteger(256);
  size_t s2 = estimator.prependNonNegativeInteger(256);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberTwoBytes2)
{
  size_t s1 = buffer.prependNonNegativeInteger(65535);
  size_t s2 = estimator.prependNonNegativeInteger(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberFourBytes1)
{
  size_t s1 = buffer.prependNonNegativeInteger(65536);
  size_t s2 = estimator.prependNonNegativeInteger(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberFourBytes2)
{
  size_t s1 = buffer.prependNonNegativeInteger(4294967295LL);
  size_t s2 = estimator.prependNonNegativeInteger(4294967295LL);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberEightBytes)
{
  size_t s1 = buffer.prependNonNegativeInteger(4294967296LL);
  size_t s2 = estimator.prependNonNegativeInteger(4294967296LL);
  BOOST_CHECK_EQUAL(buffer.size(), 8);
  BOOST_CHECK_EQUAL(s1, 8);
  BOOST_CHECK_EQUAL(s2, 8);
}

BOOST_AUTO_TEST_SUITE_END() // Basic

BOOST_AUTO_TEST_CASE(BlockFromBlock)
{
  static uint8_t buffer[] = {0x80, 0x06, 0x81, 0x01, 0x01, 0x82, 0x01, 0x01};
  Block block(buffer, sizeof(buffer));

  Block derivedBlock(block, block.begin(), block.end());
  BOOST_CHECK_EQUAL(derivedBlock.wire(), block.wire()); // pointers should match
  BOOST_CHECK(derivedBlock == block); // blocks should match

  derivedBlock = Block(block, block.begin() + 2, block.begin() + 5);
  BOOST_CHECK(derivedBlock.begin() == block.begin() + 2);
  BOOST_CHECK(derivedBlock == Block(buffer + 2, 3));

  Buffer otherBuffer(buffer, sizeof(buffer));
  BOOST_CHECK_THROW(Block(block, otherBuffer.begin(), block.end()), Block::Error);
  BOOST_CHECK_THROW(Block(block, block.begin(), otherBuffer.end()), Block::Error);
  BOOST_CHECK_THROW(Block(block, otherBuffer.begin(), otherBuffer.end()), Block::Error);
}

BOOST_AUTO_TEST_CASE(BlockFromBlockCopyOnWriteModifyOrig)
{
  static uint8_t buffer[] = {
    0x05, 0x0b, 0x07, 0x03, 0x01, 0x02, 0x03, 0x0a, 0x04, 0x04, 0x05, 0x06, 0x07,
  };

  Block block1(buffer, sizeof(buffer));

  Block block2(block1, block1.begin(), block1.end());
  auto buf2 = block2.getBuffer();

  block1.parse();
  block1.remove(tlv::Name);
  block1.encode();

  block2.parse();

  BOOST_CHECK_EQUAL_COLLECTIONS(block2.begin(), block2.end(), buffer, buffer + sizeof(buffer));
  BOOST_CHECK_EQUAL(buf2, block2.getBuffer());
}

BOOST_AUTO_TEST_CASE(BlockFromBlockCopyOnWriteModifyCopy)
{
  static uint8_t buffer[] = {
    0x05, 0x0b, 0x07, 0x03, 0x01, 0x02, 0x03, 0x0a, 0x04, 0x04, 0x05, 0x06, 0x07,
  };

  Block block1(buffer, sizeof(buffer));
  auto buf1 = block1.getBuffer();

  Block block2(block1, block1.begin(), block1.end());

  block2.parse();
  block2.remove(tlv::Name);
  block2.encode();

  block1.parse();

  BOOST_CHECK_EQUAL_COLLECTIONS(block1.begin(), block1.end(), buffer, buffer + sizeof(buffer));
  BOOST_CHECK_EQUAL(buf1, block1.getBuffer());
}

BOOST_AUTO_TEST_CASE(EncodingBufferToBlock)
{
  uint8_t value[4];

  EncodingBuffer buffer;
  size_t length = buffer.prependByteArray(value, sizeof(value));
  buffer.prependVarNumber(length);
  buffer.prependVarNumber(0xe0);

  Block block;
  BOOST_REQUIRE_NO_THROW(block = buffer.block());
  BOOST_CHECK_EQUAL(block.type(), 0xe0);
  BOOST_CHECK_EQUAL(block.value_size(), sizeof(value));

  BOOST_REQUIRE_NO_THROW(block = Block(buffer));
  BOOST_CHECK_EQUAL(block.type(), 0xe0);
  BOOST_CHECK_EQUAL(block.value_size(), sizeof(value));
}

BOOST_AUTO_TEST_CASE(BlockToBuffer)
{
  shared_ptr<Buffer> buf = make_shared<Buffer>(10);
  for (int i = 0; i < 10; i++) (*buf)[i] = i;

  Block block(0xab, buf);
  block.encode();

  {
    BOOST_REQUIRE_NO_THROW(EncodingBuffer(block));
    EncodingBuffer buffer(block);
    BOOST_CHECK_EQUAL(buffer.size(), 12);
    BOOST_CHECK_EQUAL(buffer.capacity(), 12);
  }

  (*buf)[1] = 0xe0;
  (*buf)[2] = 2;
  BOOST_REQUIRE_NO_THROW(block = Block(buf, buf->begin() + 1, buf->begin() + 5));
  BOOST_CHECK_EQUAL(block.type(), 0xe0);

  {
    BOOST_REQUIRE_NO_THROW(EncodingBuffer(block));
    EncodingBuffer buffer(block);
    BOOST_CHECK_EQUAL(buffer.size(), 4);
    BOOST_CHECK_EQUAL(buffer.capacity(), 10);
  }
}

BOOST_AUTO_TEST_CASE(FromBuffer)
{
  const uint8_t TEST_BUFFER[] = {0x00, 0x01, 0xfa, // ok
                                 0x01, 0x01, 0xfb, // ok
                                 0x03, 0x02, 0xff}; // not ok
  BufferPtr buffer(new Buffer(TEST_BUFFER, sizeof(TEST_BUFFER)));

  // using BufferPtr (avoids memory copy)
  size_t offset = 0;
  bool isOk = false;
  Block testBlock;
  std::tie(isOk, testBlock) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(testBlock.type(), 0);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x00);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfa);
  offset += testBlock.size();

  std::tie(isOk, testBlock) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(testBlock.type(), 1);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x01);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfb);
  offset += testBlock.size();

  std::tie(isOk, testBlock) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(!isOk);

  // just buffer, copies memory
  offset = 0;
  std::tie(isOk, testBlock) = Block::fromBuffer(TEST_BUFFER + offset,
                                                sizeof(TEST_BUFFER) - offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(testBlock.type(), 0);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x00);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfa);
  offset += testBlock.size();

  std::tie(isOk, testBlock) = Block::fromBuffer(TEST_BUFFER + offset,
                                                sizeof(TEST_BUFFER) - offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(testBlock.type(), 1);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x01);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfb);
  offset += testBlock.size();

  std::tie(isOk, testBlock) = Block::fromBuffer(TEST_BUFFER + offset,
                                                sizeof(TEST_BUFFER) - offset);
  BOOST_CHECK(!isOk);
}

BOOST_AUTO_TEST_CASE(FromStream)
{
  const uint8_t TEST_BUFFER[] = {0x00, 0x01, 0xfa, // ok
                                 0x01, 0x01, 0xfb, // ok
                                 0x03, 0x02, 0xff}; // not ok

  typedef boost::iostreams::stream<boost::iostreams::array_source> ArrayStream;
  ArrayStream stream(reinterpret_cast<const char*>(TEST_BUFFER), sizeof(TEST_BUFFER));


  Block testBlock;
  BOOST_REQUIRE_NO_THROW(testBlock = Block::fromStream(stream));
  BOOST_CHECK_EQUAL(testBlock.type(), 0);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x00);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfa);

  BOOST_REQUIRE_NO_THROW(testBlock = Block::fromStream(stream));
  BOOST_CHECK_EQUAL(testBlock.type(), 1);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x01);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfb);

  BOOST_CHECK_THROW(Block::fromStream(stream), tlv::Error);
}

BOOST_AUTO_TEST_CASE(FromStreamWhitespace) // Bug 2728
{
  uint8_t PACKET[] = {
    0x06, 0x20, // Data
          0x07, 0x11, // Name
                0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, // NameComponent 'hello'
                0x08, 0x01, 0x31, // NameComponent '1'
                0x08, 0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64, // NameComponent 'world'
          0x14, 0x00, // MetaInfo empty
          0x15, 0x00, // Content empty
          0x16, 0x05, // SignatureInfo
                 0x1b, 0x01, 0x01, // SignatureType RSA
                 0x1c, 0x00, // KeyLocator empty
          0x17, 0x00 // SignatureValue empty
  };
  // TLV-LENGTH of <Data> is 0x20 which happens to be ASCII whitespace

  std::stringstream stream;
  stream.write(reinterpret_cast<const char*>(PACKET), sizeof(PACKET));
  stream.seekg(0);

  Block block = Block::fromStream(stream);
  BOOST_CHECK_NO_THROW(block.parse());
}

BOOST_AUTO_TEST_CASE(FromStreamZeroLength) // Bug 2729
{
  uint8_t BUFFER[] = { 0x07, 0x00 }; // TLV-LENGTH is zero

  std::stringstream stream;
  stream.write(reinterpret_cast<const char*>(BUFFER), sizeof(BUFFER));
  stream.seekg(0);

  Block block;
  BOOST_CHECK_NO_THROW(block = Block::fromStream(stream));
  BOOST_CHECK_EQUAL(block.type(), 0x07);
  BOOST_CHECK_EQUAL(block.value_size(), 0);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Block>));

  Block a("\x08\x00", 2);
  Block b("\x08\x00", 2);;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  Block c("\x06\x00", 2);
  Block d("\x08\x00", 2);;
  BOOST_CHECK_EQUAL(c == d, false);
  BOOST_CHECK_EQUAL(c != d, true);

  Block e("\x06\x00", 2);
  Block f("\x06\x01\xcc", 3);;
  BOOST_CHECK_EQUAL(e == f, false);
  BOOST_CHECK_EQUAL(e != f, true);
}

BOOST_AUTO_TEST_CASE(InsertBeginning)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::NameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::NameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::NameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(secondBlock);
  masterBlock.push_back(thirdBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 2);
  Block::element_const_iterator it = masterBlock.find(tlv::NameComponent);
  BOOST_CHECK_EQUAL(*it == secondBlock, true);

  it = masterBlock.insert(it, firstBlock);

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 3);
  BOOST_CHECK_EQUAL(*(it + 1) == secondBlock, true);
  BOOST_CHECK_EQUAL(*(masterBlock.elements_begin()) == firstBlock, true);
}

BOOST_AUTO_TEST_CASE(InsertEnd)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::NameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::NameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::NameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(secondBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 2);
  Block::element_const_iterator it = masterBlock.elements_end();
  BOOST_CHECK_EQUAL(*(it - 1) == secondBlock, true);

  it = masterBlock.insert(it, thirdBlock);

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 3);
  BOOST_CHECK_EQUAL(*(it - 1) == secondBlock, true);
  BOOST_CHECK_EQUAL(*(masterBlock.elements_end() - 1) == thirdBlock, true);
}

BOOST_AUTO_TEST_CASE(InsertMiddle)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::NameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::NameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::NameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(thirdBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 2);
  Block::element_const_iterator it = masterBlock.find(tlv::NameComponent);
  BOOST_CHECK_EQUAL(*it == firstBlock, true);

  it = masterBlock.insert(it+1, secondBlock);

  BOOST_CHECK_EQUAL(*it == secondBlock, true);
  BOOST_CHECK_EQUAL(*(it + 1) == thirdBlock, true);
  BOOST_CHECK_EQUAL(*(it - 1) == firstBlock, true);
}

BOOST_AUTO_TEST_CASE(EraseSingleElement)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::NameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::NameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::NameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(secondBlock);
  masterBlock.push_back(thirdBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 3);
  Block::element_const_iterator it = masterBlock.find(tlv::NameComponent);
  it++;
  BOOST_CHECK_EQUAL(*it == secondBlock, true);

  it = masterBlock.erase(it);

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 2);
  BOOST_CHECK_EQUAL(*(it) == thirdBlock, true);
  BOOST_CHECK_EQUAL(*(it - 1) == firstBlock, true);
}

BOOST_AUTO_TEST_CASE(EraseRange)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::NameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::NameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::NameComponent, "thirdName");
  Block fourthBlock = makeStringBlock(tlv::NameComponent, "fourthName");
  Block fifthBlock = makeStringBlock(tlv::NameComponent, "fifthName");
  Block sixthBlock = makeStringBlock(tlv::NameComponent, "sixthName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(secondBlock);
  masterBlock.push_back(thirdBlock);
  masterBlock.push_back(fourthBlock);
  masterBlock.push_back(fifthBlock);
  masterBlock.push_back(sixthBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 6);
  Block::element_const_iterator itStart = masterBlock.find(tlv::NameComponent);
  itStart++;
  Block::element_const_iterator itEnd = itStart + 3;
  BOOST_CHECK_EQUAL(*itStart == secondBlock, true);
  BOOST_CHECK_EQUAL(*itEnd == fifthBlock, true);

  Block::element_const_iterator newIt = masterBlock.erase(itStart, itEnd);

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 3);
  BOOST_CHECK_EQUAL(*(newIt) == fifthBlock, true);
  BOOST_CHECK_EQUAL(*(newIt - 1) == firstBlock, true);
}

BOOST_AUTO_TEST_CASE(Remove)
{
  Block block(tlv::Data);
  block.push_back(makeNonNegativeIntegerBlock(tlv::ContentType, 0));
  block.push_back(makeNonNegativeIntegerBlock(tlv::FreshnessPeriod, 123));
  block.push_back(makeStringBlock(tlv::Name, "ndn:/test-prefix"));
  block.push_back(makeNonNegativeIntegerBlock(tlv::ContentType, 2));
  block.push_back(makeNonNegativeIntegerBlock(tlv::ContentType, 1));

  BOOST_CHECK_EQUAL(5, block.elements_size());
  BOOST_REQUIRE_NO_THROW(block.remove(tlv::ContentType));
  BOOST_CHECK_EQUAL(2, block.elements_size());

  Block::element_container elements = block.elements();

  BOOST_CHECK_EQUAL(tlv::FreshnessPeriod, elements[0].type());
  BOOST_CHECK_EQUAL(123, readNonNegativeInteger(elements[0]));
  BOOST_CHECK_EQUAL(tlv::Name, elements[1].type());
  BOOST_CHECK(readString(elements[1]).compare("ndn:/test-prefix") == 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
