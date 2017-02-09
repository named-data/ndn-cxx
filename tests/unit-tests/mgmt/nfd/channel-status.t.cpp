/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "mgmt/nfd/channel-status.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestChannelStatus)

BOOST_AUTO_TEST_CASE(Encode)
{
  ChannelStatus status1;
  status1.setLocalUri("udp4://192.168.2.1");
  Block wire = status1.wireEncode();

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x82, 0x14, 0x81, 0x12, 0x75, 0x64, 0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32,
    0x2e, 0x31, 0x36, 0x38, 0x2e, 0x32, 0x2e, 0x31
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  ChannelStatus status2(wire);
  BOOST_CHECK_EQUAL(status1, status2);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  ChannelStatus status1, status2;

  status1.setLocalUri("udp4://127.0.0.1:6363");
  status2 = status1;
  BOOST_CHECK_EQUAL(status1, status2);

  status2.setLocalUri("dev://eth0");
  BOOST_CHECK_NE(status1, status2);
}

BOOST_AUTO_TEST_CASE(Print)
{
  ChannelStatus status;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(status),
                    "Channel(LocalUri: )");

  status.setLocalUri("udp4://127.0.0.1:6363");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(status),
                    "Channel(LocalUri: udp4://127.0.0.1:6363)");
}

BOOST_AUTO_TEST_SUITE_END() // TestChannelStatus
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
