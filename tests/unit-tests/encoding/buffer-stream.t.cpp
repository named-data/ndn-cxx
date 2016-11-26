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

#include "encoding/buffer-stream.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestBufferStream)

BOOST_AUTO_TEST_CASE(Empty)
{
  OBufferStream os;

  shared_ptr<Buffer> buf = os.buf();
  BOOST_CHECK_EQUAL(buf->size(), 0);
}

BOOST_AUTO_TEST_CASE(Put)
{
  OBufferStream os;
  os.put(0x33);
  os.put(0x44);

  shared_ptr<Buffer> buf = os.buf();
  BOOST_REQUIRE_EQUAL(buf->size(), 2);
  BOOST_CHECK_EQUAL(buf->at(0), 0x33);
  BOOST_CHECK_EQUAL(buf->at(1), 0x44);
}

BOOST_AUTO_TEST_CASE(Write)
{
  OBufferStream os;
  os.write("\x11\x22", 2);

  shared_ptr<Buffer> buf = os.buf();
  BOOST_REQUIRE_EQUAL(buf->size(), 2);
  BOOST_CHECK_EQUAL(buf->at(0), 0x11);
  BOOST_CHECK_EQUAL(buf->at(1), 0x22);
}

BOOST_AUTO_TEST_CASE(Destructor) // Bug 3727
{
  auto os = make_unique<OBufferStream>();
  *os << 'x';
  os.reset(); // should not cause use-after-free

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // TestBufferStream
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace ndn
