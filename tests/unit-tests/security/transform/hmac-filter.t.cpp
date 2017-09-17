/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "security/transform/hmac-filter.hpp"

#include "encoding/buffer-stream.hpp"
#include "security/transform/buffer-source.hpp"
#include "security/transform/step-source.hpp"
#include "security/transform/stream-sink.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestHmacFilter)

static const uint8_t key[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static const uint8_t data[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static const uint8_t digest[] = {
  0x9f, 0x3a, 0xa2, 0x88, 0x26, 0xb3, 0x74, 0x85,
  0xca, 0x05, 0x01, 0x4d, 0x71, 0x42, 0xb3, 0xea,
  0x3f, 0xfb, 0xda, 0x5a, 0x35, 0xbf, 0xd2, 0x0f,
  0x2f, 0x9c, 0x8f, 0xcc, 0x6d, 0x30, 0x48, 0x54
};

BOOST_AUTO_TEST_CASE(Basic)
{
  OBufferStream os;
  bufferSource(data, sizeof(data)) >> hmacFilter(DigestAlgorithm::SHA256, key, sizeof(key)) >> streamSink(os);

  ConstBufferPtr buf = os.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(digest, digest + sizeof(digest), buf->begin(), buf->end());
}

BOOST_AUTO_TEST_CASE(StepByStep)
{
  OBufferStream os;
  StepSource source;
  source >> hmacFilter(DigestAlgorithm::SHA256, key, sizeof(key)) >> streamSink(os);
  source.write(data, 1);
  source.write(data + 1, 2);
  source.write(data + 3, 3);
  source.write(data + 6, 4);
  source.write(data + 10, 5);
  source.write(data + 15, 1);
  source.end();

  ConstBufferPtr buf = os.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(digest, digest + sizeof(digest), buf->begin(), buf->end());
}

BOOST_AUTO_TEST_CASE(EmptyInput)
{
  const uint8_t digest[] = {
    0x07, 0xEF, 0xF8, 0xB3, 0x26, 0xB7, 0x79, 0x8C,
    0x9C, 0xCF, 0xCB, 0xDB, 0xE5, 0x79, 0x48, 0x9A,
    0xC7, 0x85, 0xA7, 0x99, 0x5A, 0x04, 0x61, 0x8B,
    0x1A, 0x28, 0x13, 0xC2, 0x67, 0x44, 0x77, 0x7D
  };

  OBufferStream os;
  StepSource source;
  source >> hmacFilter(DigestAlgorithm::SHA256, key, sizeof(key)) >> streamSink(os);
  source.end();

  ConstBufferPtr buf = os.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(digest, digest + sizeof(digest), buf->begin(), buf->end());
}

BOOST_AUTO_TEST_CASE(InvalidAlgorithm)
{
  BOOST_CHECK_THROW(HmacFilter(DigestAlgorithm::NONE, key, sizeof(key)), Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestHmacFilter
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
