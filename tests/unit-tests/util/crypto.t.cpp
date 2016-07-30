/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "util/crypto.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace crypto {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestCrypto)

BOOST_AUTO_TEST_CASE(Basic)
{
  const std::string testString = "Hello, world!";
  ConstBufferPtr result;
  BOOST_CHECK_NO_THROW(result = computeSha256Digest(reinterpret_cast<const uint8_t*>(testString.data()),
                                                    testString.size()));

  BOOST_CHECK_EQUAL(result->size(), SHA256_DIGEST_SIZE);

  const uint8_t expectedSha256[] = {0x31, 0x5f, 0x5b, 0xdb, 0x76, 0xd0, 0x78, 0xc4,
                                    0x3b, 0x8a, 0xc0, 0x06, 0x4e, 0x4a, 0x01, 0x64,
                                    0x61, 0x2b, 0x1f, 0xce, 0x77, 0xc8, 0x69, 0x34,
                                    0x5b, 0xfc, 0x94, 0xc7, 0x58, 0x94, 0xed, 0xd3};
  BOOST_CHECK_EQUAL_COLLECTIONS(result->begin(), result->end(),
                                expectedSha256, expectedSha256 + sizeof(expectedSha256));
}

BOOST_AUTO_TEST_SUITE_END() // TestCrypto
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace crypto
} // namespace ndn
