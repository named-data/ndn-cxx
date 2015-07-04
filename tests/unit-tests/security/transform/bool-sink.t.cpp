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

#include "security/transform/bool-sink.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestBoolSink)

BOOST_AUTO_TEST_CASE(Basic)
{
  uint8_t in1[] = {0x00, 0x01};
  bool value1 = true;
  BoolSink sink1(value1);
  BOOST_CHECK_EQUAL(sink1.write(in1, 1), 1);
  BOOST_CHECK_EQUAL(sink1.write(in1 + 1, 1), 1);
  sink1.end();
  BOOST_CHECK_EQUAL(value1, false);
  BOOST_CHECK_THROW(sink1.write(in1 + 1, 1), transform::Error);

  uint8_t in2[] = {0x01, 0x00};
  bool value2 = false;
  BoolSink sink2(value2);
  BOOST_CHECK_EQUAL(sink2.write(in2, 1), 1);
  BOOST_CHECK_EQUAL(sink2.write(in2 + 1, 1), 1);
  sink2.end();
  BOOST_CHECK_EQUAL(value2, true);
  BOOST_CHECK_THROW(sink2.write(in2 + 1, 1), transform::Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestBoolSink
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
