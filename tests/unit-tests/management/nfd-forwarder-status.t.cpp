/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "management/nfd-forwarder-status.hpp"
#include "data.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdForwarderStatus)

BOOST_AUTO_TEST_CASE(Encode)
{
  ForwarderStatus status1;
  status1.setNfdVersion("0.2.0-65-g75ab6b7");
  status1.setStartTimestamp(time::fromUnixTimestamp(time::milliseconds(375193249325LL)));
  status1.setCurrentTimestamp(time::fromUnixTimestamp(time::milliseconds(886109034272LL)));
  status1.setNNameTreeEntries(1849943160);
  status1.setNFibEntries(621739748);
  status1.setNPitEntries(482129741);
  status1.setNMeasurementsEntries(1771725298);
  status1.setNCsEntries(1264968688);
  status1.setNInInterests(612811615);
  status1.setNInDatas(1843576050);
  status1.setNOutInterests(952144445);
  status1.setNOutDatas(138198826);

  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = status1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  //for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //  printf("0x%02x, ", *it);
  //}
  static const uint8_t expected[] = {
    0x15, 0x5d, 0x80, 0x11, 0x30, 0x2e, 0x32, 0x2e, 0x30, 0x2d, 0x36, 0x35,
    0x2d, 0x67, 0x37, 0x35, 0x61, 0x62, 0x36, 0x62, 0x37, 0x81, 0x08, 0x00,
    0x00, 0x00, 0x57, 0x5b, 0x42, 0xa6, 0x2d, 0x82, 0x08, 0x00, 0x00, 0x00,
    0xce, 0x50, 0x36, 0xd7, 0x20, 0x83, 0x04, 0x6e, 0x43, 0xe4, 0x78, 0x84,
    0x04, 0x25, 0x0e, 0xfe, 0xe4, 0x85, 0x04, 0x1c, 0xbc, 0xb7, 0x4d, 0x86,
    0x04, 0x69, 0x9a, 0x61, 0xf2, 0x87, 0x04, 0x4b, 0x65, 0xe3, 0xf0, 0x90,
    0x04, 0x24, 0x86, 0xc3, 0x5f, 0x91, 0x04, 0x6d, 0xe2, 0xbc, 0xf2, 0x92,
    0x04, 0x38, 0xc0, 0x92, 0x3d, 0x93, 0x04, 0x08, 0x3c, 0xbf, 0x2a
  };
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                  wire.begin(), wire.end());

  Data data;
  data.setContent(wire);

  BOOST_REQUIRE_NO_THROW(ForwarderStatus(data.getContent()));
  ForwarderStatus status2(data.getContent());
  BOOST_CHECK_EQUAL(status1.getNfdVersion(), status2.getNfdVersion());
  BOOST_CHECK_EQUAL(status1.getStartTimestamp(), status2.getStartTimestamp());
  BOOST_CHECK_EQUAL(status1.getCurrentTimestamp(), status2.getCurrentTimestamp());
  BOOST_CHECK_EQUAL(status1.getNNameTreeEntries(), status2.getNNameTreeEntries());
  BOOST_CHECK_EQUAL(status1.getNFibEntries(), status2.getNFibEntries());
  BOOST_CHECK_EQUAL(status1.getNPitEntries(), status2.getNPitEntries());
  BOOST_CHECK_EQUAL(status1.getNMeasurementsEntries(), status2.getNMeasurementsEntries());
  BOOST_CHECK_EQUAL(status1.getNCsEntries(), status2.getNCsEntries());
  BOOST_CHECK_EQUAL(status1.getNInInterests(), status2.getNInInterests());
  BOOST_CHECK_EQUAL(status1.getNInDatas(), status2.getNInDatas());
  BOOST_CHECK_EQUAL(status1.getNOutInterests(), status2.getNOutInterests());
  BOOST_CHECK_EQUAL(status1.getNOutDatas(), status2.getNOutDatas());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
