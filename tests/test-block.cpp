/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "encoding/encoding-buffer.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(TestBlock)

BOOST_AUTO_TEST_CASE (Decode)
{
  uint8_t value[4];

  ndn::EncodingBuffer buffer;
  size_t length = buffer.prependByteArray(value, sizeof(value));
  buffer.prependVarNumber(length);
  buffer.prependVarNumber(0xe0);

  Block block;
  BOOST_REQUIRE_NO_THROW(block = buffer.block());
  BOOST_CHECK_EQUAL(block.type(), 0xe0);
  BOOST_CHECK_EQUAL(block.value_size(), sizeof(value));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
