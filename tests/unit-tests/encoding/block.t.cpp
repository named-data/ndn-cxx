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

#include "encoding/block.hpp"
#include "encoding/block-helpers.hpp"

#include "block-literal.hpp"
#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <sstream>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestBlock)

BOOST_AUTO_TEST_SUITE(Construction)

static const uint8_t TEST_BUFFER[] = {
  0x00, 0x01, 0xfa, // ok
  0x01, 0x01, 0xfb, // ok
  0x03, 0x02, 0xff // bad: TLV-LENGTH is 2 but there's only 1-octet TLV-VALUE
};

BOOST_AUTO_TEST_CASE(Empty)
{
  Block b;
  BOOST_CHECK_EQUAL(b.empty(), true);
}

BOOST_AUTO_TEST_CASE(FromEncodingBuffer)
{
  const uint8_t VALUE[4] = {0x11, 0x12, 0x13, 0x14};

  EncodingBuffer encoder;
  size_t length = encoder.prependByteArray(VALUE, sizeof(VALUE));
  encoder.prependVarNumber(length);
  encoder.prependVarNumber(0xe0);

  Block b = encoder.block();
  BOOST_CHECK_EQUAL(b.type(), 0xe0);
  BOOST_CHECK_EQUAL(b.value_size(), sizeof(VALUE));
  BOOST_CHECK_EQUAL_COLLECTIONS(b.value_begin(), b.value_end(),
                                VALUE, VALUE + sizeof(VALUE));

  b = Block(encoder);
  BOOST_CHECK_EQUAL(b.type(), 0xe0);
  BOOST_CHECK_EQUAL(b.value_size(), sizeof(VALUE));
  BOOST_CHECK_EQUAL_COLLECTIONS(b.value_begin(), b.value_end(),
                                VALUE, VALUE + sizeof(VALUE));
}

BOOST_AUTO_TEST_CASE(FromEmptyEncodingBuffer)
{
  EncodingBuffer encoder;
  Block b;
  BOOST_CHECK_THROW(b = Block(encoder), tlv::Error);
}

BOOST_AUTO_TEST_CASE(FromBlock)
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
  BOOST_CHECK_THROW(Block(block, otherBuffer.begin(), block.end()), std::invalid_argument);
  BOOST_CHECK_THROW(Block(block, block.begin(), otherBuffer.end()), std::invalid_argument);
  BOOST_CHECK_THROW(Block(block, otherBuffer.begin(), otherBuffer.end()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(FromBlockCopyOnWriteModifyOriginal)
{
  const uint8_t BUFFER[] = {
    0x05, 0x0b, 0x07, 0x03, 0x01, 0x02, 0x03, 0x0a, 0x04, 0x04, 0x05, 0x06, 0x07,
  };

  Block b1(BUFFER, sizeof(BUFFER));

  Block b2(b1, b1.begin(), b1.end());
  auto buf2 = b2.getBuffer();

  b1.parse();
  b1.remove(tlv::Name);
  b1.encode();

  b2.parse();

  BOOST_CHECK_EQUAL_COLLECTIONS(b2.begin(), b2.end(), BUFFER, BUFFER + sizeof(BUFFER));
  BOOST_CHECK_EQUAL(buf2, b2.getBuffer());
}

BOOST_AUTO_TEST_CASE(FromBlockCopyOnWriteModifyCopy)
{
  const uint8_t BUFFER[] = {
    0x05, 0x0b, 0x07, 0x03, 0x01, 0x02, 0x03, 0x0a, 0x04, 0x04, 0x05, 0x06, 0x07,
  };

  Block b1(BUFFER, sizeof(BUFFER));
  auto buf1 = b1.getBuffer();

  Block b2(b1, b1.begin(), b1.end());

  b2.parse();
  b2.remove(tlv::Name);
  b2.encode();

  b1.parse();
  BOOST_CHECK_EQUAL_COLLECTIONS(b1.begin(), b1.end(), BUFFER, BUFFER + sizeof(BUFFER));
  BOOST_CHECK_EQUAL(buf1, b1.getBuffer());
}

BOOST_AUTO_TEST_CASE(FromType)
{
  Block b1(4);
  BOOST_CHECK_EQUAL(b1.empty(), false);
  BOOST_CHECK_EQUAL(b1.type(), 4);
  BOOST_CHECK_EQUAL(b1.size(), 2); // 1-octet TLV-TYPE and 1-octet TLV-LENGTH
  BOOST_CHECK_EQUAL(b1.value_size(), 0);

  Block b2(258);
  BOOST_CHECK_EQUAL(b2.type(), 258);
  BOOST_CHECK_EQUAL(b2.size(), 4); // 3-octet TLV-TYPE and 1-octet TLV-LENGTH
  BOOST_CHECK_EQUAL(b2.value_size(), 0);
}

BOOST_AUTO_TEST_CASE(FromStream)
{
  std::stringstream stream;
  stream.write(reinterpret_cast<const char*>(TEST_BUFFER), sizeof(TEST_BUFFER));
  stream.seekg(0);

  Block b = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b.type(), 0);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(*b.wire(),  0x00);
  BOOST_CHECK_EQUAL(*b.value(), 0xfa);

  b = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b.type(), 1);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(*b.wire(),  0x01);
  BOOST_CHECK_EQUAL(*b.value(), 0xfb);

  BOOST_CHECK_THROW(Block::fromStream(stream), tlv::Error);
}

BOOST_AUTO_TEST_CASE(FromStreamWhitespace) // Bug 2728
{
  const uint8_t PACKET[] = {
    0x06, 0x20, // Data
          0x07, 0x11, // Name
                0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, // GenericNameComponent 'hello'
                0x08, 0x01, 0x31, // GenericNameComponent '1'
                0x08, 0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64, // GenericNameComponent 'world'
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

  Block b = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b.type(), 6);
  BOOST_CHECK_EQUAL(b.value_size(), 32);
  b.parse();
}

BOOST_AUTO_TEST_CASE(FromStreamZeroLength)
{
  const uint8_t BUFFER[] = {0x70, 0x00,
                            0x71, 0x03, 0x86, 0x11, 0x24,
                            0x72, 0x00};

  std::stringstream stream;
  stream.write(reinterpret_cast<const char*>(BUFFER), sizeof(BUFFER));
  stream.seekg(0);

  Block b1 = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b1.type(), 0x70);
  BOOST_CHECK_EQUAL(b1.value_size(), 0);

  Block b2 = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b2.type(), 0x71);
  BOOST_CHECK_EQUAL(b2.value_size(), 3);
  const uint8_t EXPECTED_VALUE2[] = {0x86, 0x11, 0x24};
  BOOST_CHECK_EQUAL_COLLECTIONS(b2.value_begin(), b2.value_end(),
                                EXPECTED_VALUE2, EXPECTED_VALUE2 + sizeof(EXPECTED_VALUE2));

  Block b3 = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b3.type(), 0x72);
  BOOST_CHECK_EQUAL(b3.value_size(), 0);

  BOOST_CHECK_THROW(Block::fromStream(stream), tlv::Error);
}

BOOST_AUTO_TEST_CASE(FromStreamPacketTooLarge)
{
  const uint8_t BUFFER[] = {0x07, 0xfe, 0x00, 0x01, 0x00, 0x00};

  std::stringstream stream;
  stream.write(reinterpret_cast<const char*>(BUFFER), sizeof(BUFFER));
  for (int i = 0; i < 0x10000; ++i) {
    stream.put('\0');
  }
  stream.seekg(0);

  BOOST_CHECK_THROW(Block::fromStream(stream), tlv::Error);
}

BOOST_AUTO_TEST_CASE(FromWireBuffer)
{
  ConstBufferPtr buffer = make_shared<Buffer>(TEST_BUFFER, sizeof(TEST_BUFFER));

  size_t offset = 0;
  bool isOk = false;
  Block b;
  std::tie(isOk, b) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 0);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(*b.wire(),  0x00);
  BOOST_CHECK_EQUAL(*b.value(), 0xfa);
  offset += b.size();

  std::tie(isOk, b) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 1);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(*b.wire(),  0x01);
  BOOST_CHECK_EQUAL(*b.value(), 0xfb);
  offset += b.size();

  std::tie(isOk, b) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(!isOk);
}

BOOST_AUTO_TEST_CASE(FromRawBuffer)
{
  size_t offset = 0;
  bool isOk = false;
  Block b;
  std::tie(isOk, b) = Block::fromBuffer(TEST_BUFFER + offset, sizeof(TEST_BUFFER) - offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 0);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(*b.wire(),  0x00);
  BOOST_CHECK_EQUAL(*b.value(), 0xfa);
  offset += b.size();

  std::tie(isOk, b) = Block::fromBuffer(TEST_BUFFER + offset, sizeof(TEST_BUFFER) - offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 1);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(*b.wire(),  0x01);
  BOOST_CHECK_EQUAL(*b.value(), 0xfb);
  offset += b.size();

  std::tie(isOk, b) = Block::fromBuffer(TEST_BUFFER + offset, sizeof(TEST_BUFFER) - offset);
  BOOST_CHECK(!isOk);
}

BOOST_AUTO_TEST_SUITE_END() // Construction

BOOST_AUTO_TEST_SUITE(SubElements)

BOOST_AUTO_TEST_CASE(Parse)
{
  const uint8_t PACKET[] = {
    0x06, 0x20, // Data
          0x07, 0x11, // Name
                0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, // GenericNameComponent 'hello'
                0x08, 0x01, 0x31, // GenericNameComponent '1'
                0x08, 0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64, // GenericNameComponent 'world'
          0x14, 0x00, // MetaInfo empty
          0x15, 0x00, // Content empty
          0x16, 0x05, // SignatureInfo
                0x1b, 0x01, 0x01, // SignatureType RSA
                0x1c, 0x00, // KeyLocator empty
          0x17, 0x00 // SignatureValue empty
  };

  Block data(PACKET, sizeof(PACKET));
  data.parse();

  BOOST_CHECK_EQUAL(data.elements_size(), 5);
  BOOST_CHECK_EQUAL(data.elements().at(0).type(), 0x07);
  BOOST_CHECK_EQUAL(data.elements().at(0).elements().size(), 0); // parse is not recursive

  BOOST_CHECK(data.get(0x15) == data.elements().at(2));
  BOOST_CHECK_THROW(data.get(0x01), Block::Error);

  BOOST_CHECK(data.find(0x15) == data.elements_begin() + 2);
  BOOST_CHECK(data.find(0x01) == data.elements_end());
}

BOOST_AUTO_TEST_CASE(InsertBeginning)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::GenericNameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::GenericNameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::GenericNameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(secondBlock);
  masterBlock.push_back(thirdBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 2);
  Block::element_const_iterator it = masterBlock.find(tlv::GenericNameComponent);
  BOOST_CHECK_EQUAL(*it == secondBlock, true);

  it = masterBlock.insert(it, firstBlock);

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 3);
  BOOST_CHECK_EQUAL(*(it + 1) == secondBlock, true);
  BOOST_CHECK_EQUAL(*(masterBlock.elements_begin()) == firstBlock, true);
}

BOOST_AUTO_TEST_CASE(InsertEnd)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::GenericNameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::GenericNameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::GenericNameComponent, "thirdName");

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
  Block firstBlock = makeStringBlock(tlv::GenericNameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::GenericNameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::GenericNameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(thirdBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 2);
  Block::element_const_iterator it = masterBlock.find(tlv::GenericNameComponent);
  BOOST_CHECK_EQUAL(*it == firstBlock, true);

  it = masterBlock.insert(it + 1, secondBlock);

  BOOST_CHECK_EQUAL(*it == secondBlock, true);
  BOOST_CHECK_EQUAL(*(it + 1) == thirdBlock, true);
  BOOST_CHECK_EQUAL(*(it - 1) == firstBlock, true);
}

BOOST_AUTO_TEST_CASE(EraseSingleElement)
{
  Block masterBlock(tlv::Name);
  Block firstBlock = makeStringBlock(tlv::GenericNameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::GenericNameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::GenericNameComponent, "thirdName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(secondBlock);
  masterBlock.push_back(thirdBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 3);
  Block::element_const_iterator it = masterBlock.find(tlv::GenericNameComponent);
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
  Block firstBlock = makeStringBlock(tlv::GenericNameComponent, "firstName");
  Block secondBlock = makeStringBlock(tlv::GenericNameComponent, "secondName");
  Block thirdBlock = makeStringBlock(tlv::GenericNameComponent, "thirdName");
  Block fourthBlock = makeStringBlock(tlv::GenericNameComponent, "fourthName");
  Block fifthBlock = makeStringBlock(tlv::GenericNameComponent, "fifthName");
  Block sixthBlock = makeStringBlock(tlv::GenericNameComponent, "sixthName");

  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 0);
  masterBlock.push_back(firstBlock);
  masterBlock.push_back(secondBlock);
  masterBlock.push_back(thirdBlock);
  masterBlock.push_back(fourthBlock);
  masterBlock.push_back(fifthBlock);
  masterBlock.push_back(sixthBlock);
  BOOST_CHECK_EQUAL(masterBlock.elements_size(), 6);
  Block::element_const_iterator itStart = masterBlock.find(tlv::GenericNameComponent);
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

BOOST_AUTO_TEST_SUITE_END() // SubElements

BOOST_AUTO_TEST_CASE(Equality)
{
  const uint8_t one[] = {0x08, 0x00};
  Block a(one, sizeof(one));
  Block b(one, sizeof(one));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  const uint8_t two[] = {0x06, 0x00};
  Block c(two, sizeof(two));
  Block d(one, sizeof(one));
  BOOST_CHECK_EQUAL(c == d, false);
  BOOST_CHECK_EQUAL(c != d, true);

  const uint8_t three[] = {0x06, 0x01, 0xcc};
  Block e(two, sizeof(two));
  Block f(three, sizeof(three));
  BOOST_CHECK_EQUAL(e == f, false);
  BOOST_CHECK_EQUAL(e != f, true);
}

BOOST_AUTO_TEST_CASE(Print)
{
  // default constructed
  Block b;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "[invalid]");

  // zero length
  b = "0700"_block;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "7[empty]");

  // unparsed
  b = "0E10FF7E4E6B3B21C902660F16ED589FCCCC"_block;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b),
                    "14[16]=FF7E4E6B3B21C902660F16ED589FCCCC");
  // set and restore format flags
  {
    std::ostringstream oss;
    oss << std::showbase << std::hex << 0xd23c4 << b << 0x4981e;
    BOOST_CHECK_EQUAL(oss.str(), "0xd23c414[16]=FF7E4E6B3B21C902660F16ED589FCCCC0x4981e");
  }

  // parsed
  b = "FD010808 0502CADD 59024E42"_block;
  b.parse();
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b),
                    "264[8]={5[2]=CADD,89[2]=4E42}");

  // parsed then modified: print modified sub-elements
  b = "FD010808 0502CADD 59024E42"_block;
  b.parse();
  b.erase(b.elements_begin());
  b.push_back("10022386"_block);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b),
                    "264[8]={89[2]=4E42,16[2]=2386}");
}

BOOST_AUTO_TEST_SUITE_END() // TestBlock
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace ndn
