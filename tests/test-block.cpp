/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "encoding/encoding-buffer.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(TestBlock)

BOOST_AUTO_TEST_CASE (Basic)
{
  EncodingBuffer buffer;
  EncodingEstimator estimator;
  size_t s1, s2;

  // VarNumber checks
  
  s1 = buffer.prependVarNumber(252);
  s2 = estimator.prependVarNumber(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
  buffer = EncodingBuffer();
  
  s1 = buffer.prependVarNumber(253);
  s2 = estimator.prependVarNumber(253);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
  buffer = EncodingBuffer();
  
  s1 = buffer.prependVarNumber(255);
  s2 = estimator.prependVarNumber(255);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(65535);
  s2 = estimator.prependVarNumber(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(65536);
  s2 = estimator.prependVarNumber(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
  buffer = EncodingBuffer();
  
  s1 = buffer.prependVarNumber(4294967295);
  s2 = estimator.prependVarNumber(4294967295);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(4294967296);
  s2 = estimator.prependVarNumber(4294967296);
  BOOST_CHECK_EQUAL(buffer.size(), 9);
  BOOST_CHECK_EQUAL(s1, 9);
  BOOST_CHECK_EQUAL(s2, 9);
  buffer = EncodingBuffer();

  // nonNegativeInteger checks
  
  s1 = buffer.prependNonNegativeInteger(252);
  s2 = estimator.prependNonNegativeInteger(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
  buffer = EncodingBuffer();
  
  s1 = buffer.prependNonNegativeInteger(255);
  s2 = estimator.prependNonNegativeInteger(255);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
  buffer = EncodingBuffer();
  
  s1 = buffer.prependNonNegativeInteger(256);
  s2 = estimator.prependNonNegativeInteger(256);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(65535);
  s2 = estimator.prependNonNegativeInteger(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(65536);
  s2 = estimator.prependNonNegativeInteger(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(4294967295);
  s2 = estimator.prependNonNegativeInteger(4294967295);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(4294967296);
  s2 = estimator.prependNonNegativeInteger(4294967296);
  BOOST_CHECK_EQUAL(buffer.size(), 8);
  BOOST_CHECK_EQUAL(s1, 8);
  BOOST_CHECK_EQUAL(s2, 8);
  buffer = EncodingBuffer();
}

BOOST_AUTO_TEST_CASE (EncodingBufferToBlock)
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

BOOST_AUTO_TEST_CASE (BlockToBuffer)
{
  shared_ptr<Buffer> buf = make_shared<Buffer>(10);
  for (int i = 0; i < 10; i++) (*buf)[i] = i;
  
  Block block(0xab, buf);
  block.encode();

  EncodingBuffer buffer(0,0);
  BOOST_REQUIRE_NO_THROW(buffer = EncodingBuffer(block));
  BOOST_CHECK_EQUAL(buffer.size(), 12);
  BOOST_CHECK_EQUAL(buffer.capacity(), 12);

  (*buf)[1] = 0xe0;
  (*buf)[2] = 2;
  BOOST_REQUIRE_NO_THROW(block = Block(buf, buf->begin() + 1, buf->begin() + 5));
  BOOST_CHECK_EQUAL(block.type(), 0xe0);

  BOOST_REQUIRE_NO_THROW(buffer = EncodingBuffer(block));
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(buffer.capacity(), 10);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
