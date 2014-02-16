/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "encoding/encoding-buffer.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(TestBlock)

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
