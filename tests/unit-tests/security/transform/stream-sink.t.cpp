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

#include "security/transform/stream-sink.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestStreamSink)

BOOST_AUTO_TEST_CASE(Basic)
{
  uint8_t in[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x20, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };
  std::ostringstream os;
  StreamSink sink(os);
  BOOST_CHECK_EQUAL(sink.write(in, 4), 4);
  BOOST_CHECK_EQUAL(sink.write(in + 4, 4), 4);
  BOOST_CHECK_EQUAL(sink.write(in + 8, 4), 4);
  BOOST_CHECK_EQUAL(sink.write(in + 12, 4), 4);
  sink.end();
  std::string out = os.str();
  BOOST_CHECK_EQUAL_COLLECTIONS(in, in + sizeof(in), out.begin(), out.end());
  BOOST_CHECK_THROW(sink.write(in + 8, 8), transform::Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestStreamSink
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
