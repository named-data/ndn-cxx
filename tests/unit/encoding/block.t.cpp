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

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

#include "tests/boost-test.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstring>
#include <sstream>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestBlock)

BOOST_AUTO_TEST_SUITE(Construction)

static const uint8_t TEST_BUFFER[] = {
  0x42, 0x01, 0xfa,
  0x01, 0x01, 0xfb,
  0xfe, 0xff, 0xff, 0xff, 0xff, 0x00, // bug #4726
};

BOOST_AUTO_TEST_CASE(Default)
{
  Block b;

  BOOST_CHECK_EQUAL(b.isValid(), false);
  BOOST_CHECK_EQUAL(b.type(), tlv::Invalid);
  BOOST_CHECK_EQUAL(b.hasValue(), false);
  BOOST_CHECK_EQUAL(b.value_size(), 0);
  BOOST_CHECK_EQUAL(b.value_bytes().empty(), true);
  BOOST_CHECK(b.value() == nullptr);

  BOOST_CHECK_THROW(b.begin(), Block::Error);
  BOOST_CHECK_THROW(b.end(), Block::Error);
  BOOST_CHECK_THROW(b.data(), Block::Error);
  BOOST_CHECK_THROW(b.size(), Block::Error);
  BOOST_CHECK_THROW(b.blockFromValue(), Block::Error);
}

BOOST_AUTO_TEST_CASE(FromEncodingBuffer)
{
  EncodingBuffer encoder;
  BOOST_CHECK_THROW(Block{encoder}, tlv::Error);

  const uint8_t VALUE[] = {0x11, 0x12, 0x13, 0x14};
  size_t length = encoder.prependBytes(VALUE);
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

BOOST_AUTO_TEST_CASE(FromBlock)
{
  const uint8_t buf[] = {0x80, 0x06, 0x81, 0x01, 0x01, 0x82, 0x01, 0x01};
  Block block(buf);

  Block derivedBlock(block, block.begin(), block.end());
  BOOST_CHECK_EQUAL(derivedBlock.data(), block.data()); // pointers should match
  BOOST_CHECK_EQUAL(derivedBlock, block); // blocks should match

  derivedBlock = Block(block, block.begin() + 2, block.begin() + 5);
  BOOST_CHECK(derivedBlock.begin() == block.begin() + 2);
  BOOST_CHECK_EQUAL(derivedBlock, Block(make_span(buf + 2, 3)));

  Buffer otherBuffer(buf, sizeof(buf));
  BOOST_CHECK_THROW(Block(block, otherBuffer.begin(), block.end()), std::invalid_argument);
  BOOST_CHECK_THROW(Block(block, block.begin(), otherBuffer.end()), std::invalid_argument);
  BOOST_CHECK_THROW(Block(block, otherBuffer.begin(), otherBuffer.end()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(FromBlockCopyOnWriteModifyOriginal)
{
  const uint8_t buf[] = {
    0x05, 0x0b, 0x07, 0x03, 0x01, 0x02, 0x03, 0x0a, 0x04, 0x04, 0x05, 0x06, 0x07,
  };

  Block b1(buf);
  Block b2(b1, b1.begin(), b1.end());
  auto buf2 = b2.getBuffer();

  b1.parse();
  b1.remove(tlv::Name);
  b1.encode();

  b2.parse();
  BOOST_TEST(b2 == buf, boost::test_tools::per_element());
  BOOST_TEST(buf2 == b2.getBuffer()); // check pointers
}

BOOST_AUTO_TEST_CASE(FromBlockCopyOnWriteModifyCopy)
{
  const uint8_t buf[] = {
    0x05, 0x0b, 0x07, 0x03, 0x01, 0x02, 0x03, 0x0a, 0x04, 0x04, 0x05, 0x06, 0x07,
  };

  Block b1(buf);
  auto buf1 = b1.getBuffer();
  Block b2(b1, b1.begin(), b1.end());

  b2.parse();
  b2.remove(tlv::Name);
  b2.encode();

  b1.parse();
  BOOST_TEST(b1 == buf, boost::test_tools::per_element());
  BOOST_TEST(buf1 == b1.getBuffer()); // check pointers
}

BOOST_AUTO_TEST_CASE(FromType)
{
  Block b1(4);
  BOOST_CHECK_EQUAL(b1.isValid(), true);
  BOOST_CHECK_EQUAL(b1.type(), 4);
  BOOST_CHECK_EQUAL(b1.size(), 2); // 1-octet TLV-TYPE and 1-octet TLV-LENGTH
  BOOST_CHECK_EQUAL(b1.hasValue(), false);
  BOOST_CHECK_EQUAL(b1.value_size(), 0);
  BOOST_CHECK_EQUAL(b1.value_bytes().empty(), true);
  BOOST_CHECK(b1.value() == nullptr);

  Block b2(258);
  BOOST_CHECK_EQUAL(b2.isValid(), true);
  BOOST_CHECK_EQUAL(b2.type(), 258);
  BOOST_CHECK_EQUAL(b2.size(), 4); // 3-octet TLV-TYPE and 1-octet TLV-LENGTH
  BOOST_CHECK_EQUAL(b2.hasValue(), false);
  BOOST_CHECK_EQUAL(b2.value_size(), 0);
  BOOST_CHECK_EQUAL(b2.value_bytes().empty(), true);
  BOOST_CHECK(b2.value() == nullptr);

  Block b3(tlv::Invalid);
  BOOST_CHECK_EQUAL(b3.isValid(), false);
  BOOST_CHECK_EQUAL(b3.type(), tlv::Invalid);
  BOOST_CHECK_EXCEPTION(b3.size(), Block::Error, [] (const auto& e) {
    return e.what() == "Cannot determine size of invalid block"s;
  });
  BOOST_CHECK_EQUAL(b3.hasValue(), false);
  BOOST_CHECK_EQUAL(b3.value_size(), 0);
  BOOST_CHECK_EQUAL(b3.value_bytes().empty(), true);
  BOOST_CHECK(b3.value() == nullptr);
}

BOOST_AUTO_TEST_CASE(FromTypeAndBuffer)
{
  const uint8_t VALUE[] = {0x11, 0x12, 0x13, 0x14};
  auto bufferPtr = std::make_shared<Buffer>(VALUE, sizeof(VALUE));

  Block b1(42, std::move(bufferPtr));
  BOOST_CHECK_EQUAL(b1.isValid(), true);
  BOOST_CHECK_EQUAL(b1.type(), 42);
  BOOST_CHECK_EQUAL(b1.size(), 6);
  BOOST_CHECK_EQUAL(b1.hasValue(), true);
  BOOST_CHECK_EQUAL(b1.value_size(), sizeof(VALUE));
  BOOST_CHECK_EQUAL(b1.value_bytes().size(), sizeof(VALUE));
  BOOST_CHECK_EQUAL(b1.value_bytes().front(), 0x11);
  BOOST_CHECK(b1.value() != nullptr);

  // empty buffer as TLV-VALUE
  Block b2(63, std::make_shared<Buffer>());
  BOOST_CHECK_EQUAL(b2.isValid(), true);
  BOOST_CHECK_EQUAL(b2.type(), 63);
  BOOST_CHECK_EQUAL(b2.size(), 2);
  BOOST_CHECK_EQUAL(b2.hasValue(), true);
  BOOST_CHECK_EQUAL(b2.value_size(), 0);
  BOOST_CHECK_EQUAL(b2.value_bytes().empty(), true);
  BOOST_CHECK(b2.value() == nullptr);
}

BOOST_AUTO_TEST_CASE(FromTypeAndBlock)
{
  const uint8_t buf[] = {0x80, 0x06, 0x81, 0x01, 0x01, 0x82, 0x01, 0x01};
  Block nested(buf);

  Block b(84, nested);
  BOOST_CHECK_EQUAL(b.isValid(), true);
  BOOST_CHECK_EQUAL(b.type(), 84);
  BOOST_CHECK_EQUAL(b.size(), 10);
  BOOST_CHECK_EQUAL(b.hasValue(), true);
  BOOST_CHECK_EQUAL(b.value_size(), sizeof(buf));
  BOOST_CHECK_EQUAL(b.value_bytes().size(), sizeof(buf));
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0x80);
  BOOST_CHECK(b.value() != nullptr);
}

BOOST_AUTO_TEST_CASE(FromStream)
{
  std::stringstream stream;
  stream.write(reinterpret_cast<const char*>(TEST_BUFFER), sizeof(TEST_BUFFER));
  stream.seekg(0);

  Block b = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b.type(), 66);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 1);
  BOOST_CHECK_EQUAL(*b.data(), 0x42);
  BOOST_CHECK_EQUAL(*b.value(), 0xfa);
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0xfa);

  b = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b.type(), 1);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 1);
  BOOST_CHECK_EQUAL(*b.data(), 0x01);
  BOOST_CHECK_EQUAL(*b.value(), 0xfb);
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0xfb);

  b = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b.type(), 0xffffffff);
  BOOST_CHECK_EQUAL(b.size(), 6);
  BOOST_CHECK_EQUAL(b.value_size(), 0);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 0);
  BOOST_CHECK_EQUAL(*b.data(), 0xfe);
  BOOST_CHECK(b.value() == nullptr);

  BOOST_CHECK(stream.eof());
  BOOST_CHECK_EXCEPTION(Block::fromStream(stream), tlv::Error, [] (const auto& e) {
    return e.what() == "Empty buffer during TLV parsing"s;
  });
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
  BOOST_CHECK_EQUAL(b.elements_size(), 5);
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
  BOOST_CHECK_EQUAL(b1.value_bytes().size(), 0);
  BOOST_CHECK(b1.value() == nullptr);

  Block b2 = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b2.type(), 0x71);
  BOOST_CHECK_EQUAL(b2.value_size(), 3);
  BOOST_CHECK_EQUAL(b2.value_bytes().size(), 3);
  const uint8_t EXPECTED_VALUE2[] = {0x86, 0x11, 0x24};
  BOOST_TEST(b2.value_bytes() == EXPECTED_VALUE2, boost::test_tools::per_element());

  Block b3 = Block::fromStream(stream);
  BOOST_CHECK_EQUAL(b3.type(), 0x72);
  BOOST_CHECK_EQUAL(b3.value_size(), 0);
  BOOST_CHECK_EQUAL(b3.value_bytes().size(), 0);
  BOOST_CHECK(b3.value() == nullptr);

  BOOST_CHECK_EXCEPTION(Block::fromStream(stream), tlv::Error, [] (const auto& e) {
    return e.what() == "Empty buffer during TLV parsing"s;
  });
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

  BOOST_CHECK_EXCEPTION(Block::fromStream(stream), tlv::Error, [] (const auto& e) {
    return e.what() == "TLV-LENGTH from stream exceeds limit"s;
  });
}

BOOST_AUTO_TEST_CASE(FromWireBuffer)
{
  auto buffer = std::make_shared<Buffer>(TEST_BUFFER, sizeof(TEST_BUFFER));

  bool isOk = false;
  Block b;
  std::tie(isOk, b) = Block::fromBuffer(buffer);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 66);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 1);
  BOOST_CHECK_EQUAL(*b.data(), 0x42);
  BOOST_CHECK_EQUAL(*b.value(), 0xfa);
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0xfa);
  size_t offset = b.size();

  std::tie(isOk, b) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 1);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 1);
  BOOST_CHECK_EQUAL(*b.data(), 0x01);
  BOOST_CHECK_EQUAL(*b.value(), 0xfb);
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0xfb);
  offset += b.size();

  std::tie(isOk, b) = Block::fromBuffer(buffer, offset);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 0xffffffff);
  BOOST_CHECK_EQUAL(b.size(), 6);
  BOOST_CHECK_EQUAL(b.value_size(), 0);
  BOOST_CHECK_EQUAL(b.value_bytes().empty(), true);
  BOOST_CHECK_EQUAL(*b.data(), 0xfe);
  BOOST_CHECK(b.value() == nullptr);
}

BOOST_AUTO_TEST_CASE(FromRawBuffer)
{
  bool isOk = false;
  Block b;
  std::tie(isOk, b) = Block::fromBuffer(TEST_BUFFER);
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 66);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 1);
  BOOST_CHECK_EQUAL(*b.data(), 0x42);
  BOOST_CHECK_EQUAL(*b.value(), 0xfa);
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0xfa);
  auto offset = b.size();

  std::tie(isOk, b) = Block::fromBuffer(make_span(TEST_BUFFER).subspan(offset));
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 1);
  BOOST_CHECK_EQUAL(b.size(), 3);
  BOOST_CHECK_EQUAL(b.value_size(), 1);
  BOOST_CHECK_EQUAL(b.value_bytes().size(), 1);
  BOOST_CHECK_EQUAL(*b.data(), 0x01);
  BOOST_CHECK_EQUAL(*b.value(), 0xfb);
  BOOST_CHECK_EQUAL(b.value_bytes().front(), 0xfb);
  offset += b.size();

  std::tie(isOk, b) = Block::fromBuffer(make_span(TEST_BUFFER).subspan(offset));
  BOOST_CHECK(isOk);
  BOOST_CHECK_EQUAL(b.type(), 0xffffffff);
  BOOST_CHECK_EQUAL(b.size(), 6);
  BOOST_CHECK_EQUAL(b.value_size(), 0);
  BOOST_CHECK_EQUAL(b.value_bytes().empty(), true);
  BOOST_CHECK_EQUAL(*b.data(), 0xfe);
  BOOST_CHECK(b.value() == nullptr);
}

static const Buffer MalformedInputs[] = {
  {0x00, 0x00}, // invalid TLV type (zero)
  {0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}, // TLV type too large
  {0x01, 0xff, 0x42, 0x42}, // bad TLV length
  {0x01, 0x02, 0x03}, // truncated TLV value
};

BOOST_DATA_TEST_CASE(Malformed, MalformedInputs)
{
  // constructor from span
  BOOST_CHECK_THROW(Block{sample}, tlv::Error);

  // fromStream()
  std::stringstream stream;
  stream.write(sample.template get<char>(), sample.size());
  stream.seekg(0);
  BOOST_CHECK_THROW(Block::fromStream(stream), tlv::Error);

  // fromBuffer(), ConstBufferPtr overload
  auto buf = std::make_shared<Buffer>(sample.begin(), sample.end());
  bool isOk;
  Block b;
  std::tie(isOk, b) = Block::fromBuffer(buf);
  BOOST_CHECK(!isOk);
  BOOST_CHECK(!b.isValid());

  // fromBuffer(), span overload
  std::tie(isOk, b) = Block::fromBuffer(sample);
  BOOST_CHECK(!isOk);
  BOOST_CHECK(!b.isValid());
}

BOOST_AUTO_TEST_SUITE_END() // Construction

BOOST_AUTO_TEST_CASE(BlockFromValue)
{
  Block b1(301);
  BOOST_CHECK_EXCEPTION(b1.blockFromValue(), Block::Error, [] (const auto& e) {
    return e.what() == "Cannot construct block from empty TLV-VALUE"s;
  });

  Block b2(302, std::make_shared<Buffer>());
  BOOST_CHECK_EXCEPTION(b2.blockFromValue(), Block::Error, [] (const auto& e) {
    return e.what() == "Cannot construct block from empty TLV-VALUE"s;
  });

  b1.encode();
  Block b3(303, b1);
  b3.encode();
  Block nested = b3.blockFromValue();
  BOOST_CHECK_EQUAL(nested.type(), 301);
  BOOST_CHECK_EQUAL(nested.size(), 4);
  BOOST_CHECK_EQUAL(nested.value_size(), 0);
  BOOST_CHECK(nested == b1);
}

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
  Block data(PACKET);
  data.parse();

  BOOST_CHECK_EQUAL(data.elements_size(), 5);
  BOOST_CHECK_EQUAL(data.elements().at(0).type(), 0x07);
  BOOST_CHECK_EQUAL(data.elements().at(0).elements().size(), 0); // parse is not recursive

  BOOST_CHECK(data.get(0x15) == data.elements().at(2));
  BOOST_CHECK_EXCEPTION(data.get(0x01), Block::Error, [] (const auto& e) {
    return e.what() == "No sub-element of type 1 found in block of type 6"s;
  });

  BOOST_CHECK(data.find(0x15) == data.elements_begin() + 2);
  BOOST_CHECK(data.find(0x01) == data.elements_end());

  const uint8_t MALFORMED[] = {
    // TLV-LENGTH of nested element is greater than TLV-LENGTH of enclosing element
    0x05, 0x05, 0x07, 0x07, 0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f
  };
  Block bad(MALFORMED);
  BOOST_CHECK_EXCEPTION(bad.parse(), Block::Error, [] (const auto& e) {
    return e.what() == "TLV-LENGTH of sub-element of type 7 exceeds TLV-VALUE boundary of parent block"s;
  });
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
  block.push_back(makeStringBlock(tlv::Name, "/test-prefix"));
  block.push_back(makeNonNegativeIntegerBlock(tlv::ContentType, 2));
  block.push_back(makeNonNegativeIntegerBlock(tlv::ContentType, 1));

  BOOST_CHECK_EQUAL(5, block.elements_size());
  BOOST_CHECK_NO_THROW(block.remove(tlv::ContentType));
  BOOST_REQUIRE_EQUAL(2, block.elements_size());

  auto elements = block.elements();
  BOOST_CHECK_EQUAL(tlv::FreshnessPeriod, elements[0].type());
  BOOST_CHECK_EQUAL(123, readNonNegativeInteger(elements[0]));
  BOOST_CHECK_EQUAL(tlv::Name, elements[1].type());
  BOOST_CHECK_EQUAL(readString(elements[1]).compare("/test-prefix"), 0);
}

BOOST_AUTO_TEST_SUITE_END() // SubElements

BOOST_AUTO_TEST_CASE(ToAsioConstBuffer)
{
  Block block = "0101A0"_block;
  boost::asio::const_buffer buffer(block);
  BOOST_CHECK_EQUAL(boost::asio::buffer_cast<const uint8_t*>(buffer), block.data());
  BOOST_CHECK_EQUAL(boost::asio::buffer_size(buffer), block.size());
}

BOOST_AUTO_TEST_CASE(Equality)
{
  const uint8_t one[] = {0x08, 0x00};
  Block a(one);
  Block b(one);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  const uint8_t two[] = {0x06, 0x00};
  Block c(two);
  Block d(one);
  BOOST_CHECK_EQUAL(c == d, false);
  BOOST_CHECK_EQUAL(c != d, true);

  const uint8_t three[] = {0x06, 0x01, 0xcc};
  Block e(two);
  Block f(three);
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

BOOST_AUTO_TEST_SUITE(BlockLiteral)

BOOST_AUTO_TEST_CASE(Simple)
{
  Block b0 = "4200"_block;
  BOOST_CHECK_EQUAL(b0.type(), 0x42);
  BOOST_CHECK_EQUAL(b0.value_size(), 0);

  Block b1 = "0101A0"_block;
  BOOST_CHECK_EQUAL(b1.type(), 0x01);
  BOOST_REQUIRE_EQUAL(b1.value_size(), 1);
  BOOST_CHECK_EQUAL(b1.value()[0], 0xA0);
}

BOOST_AUTO_TEST_CASE(Comment)
{
  Block b0 = "a2b0c0d2eBf0G.B 1+"_block;
  BOOST_CHECK_EQUAL(b0.type(), 0x20);
  BOOST_REQUIRE_EQUAL(b0.value_size(), 2);
  BOOST_CHECK_EQUAL(b0.value()[0], 0xB0);
  BOOST_CHECK_EQUAL(b0.value()[1], 0xB1);
}

BOOST_AUTO_TEST_CASE(BadInput)
{
  BOOST_CHECK_THROW(""_block, std::invalid_argument);
  BOOST_CHECK_THROW("1"_block, std::invalid_argument);
  BOOST_CHECK_THROW("333"_block, std::invalid_argument);
  BOOST_CHECK_THROW("xx yy zz"_block, std::invalid_argument); // only comments

  BOOST_CHECK_THROW("0000"_block, tlv::Error); // invalid type
  BOOST_CHECK_THROW("0202C0"_block, tlv::Error); // truncated value
  BOOST_CHECK_THROW("0201C0C1"_block, tlv::Error); // trailing garbage
}

BOOST_AUTO_TEST_SUITE_END() // BlockLiteral

BOOST_AUTO_TEST_SUITE_END() // TestBlock
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace ndn
