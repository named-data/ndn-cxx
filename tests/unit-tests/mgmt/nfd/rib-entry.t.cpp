/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "mgmt/nfd/rib-entry.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestRibEntry)

static Route
makeRoute()
{
  return Route()
      .setFaceId(1)
      .setOrigin(ROUTE_ORIGIN_NLSR)
      .setCost(100)
      .setFlags(ROUTE_FLAG_CAPTURE);
}

static RibEntry
makeRibEntry()
{
  return RibEntry()
      .setName("/hello/world")
      .addRoute(makeRoute()
                .setExpirationPeriod(10_s));
}

BOOST_AUTO_TEST_CASE(RouteEncode)
{
  Route route1 = makeRoute();
  route1.setExpirationPeriod(10_s);
  const Block& wire = route1.wireEncode();

  static const uint8_t expected[] = {
    0x81, 0x10, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01, 0x64, 0x6c, 0x01, 0x02,
    0x6d, 0x02, 0x27, 0x10
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  Route route2(wire);
  BOOST_CHECK_EQUAL(route1, route2);
}

BOOST_AUTO_TEST_CASE(RouteNoExpirationPeriodEncode)
{
  Route route1 = makeRoute();
  const Block& wire = route1.wireEncode();

  static const uint8_t expected[] = {
    0x81, 0x0C, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01, 0x64, 0x6c, 0x01, 0x02
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  Route route2(wire);
  BOOST_CHECK_EQUAL(route1, route2);
}

BOOST_AUTO_TEST_CASE(RouteExpirationPeriod)
{
  Route route;
  BOOST_CHECK_EQUAL(route.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(route.getExpirationPeriod(), time::milliseconds::max());

  route.setExpirationPeriod(1_min);
  BOOST_CHECK_EQUAL(route.hasExpirationPeriod(), true);
  BOOST_CHECK_EQUAL(route.getExpirationPeriod(), 1_min);

  route.setExpirationPeriod(time::milliseconds::max());
  BOOST_CHECK_EQUAL(route.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(route.getExpirationPeriod(), time::milliseconds::max());

  route.setExpirationPeriod(1_min);
  BOOST_CHECK_EQUAL(route.hasExpirationPeriod(), true);

  route.unsetExpirationPeriod();
  BOOST_CHECK_EQUAL(route.hasExpirationPeriod(), false);
}

BOOST_AUTO_TEST_CASE(RouteEquality)
{
  Route route1, route2;

  route1 = makeRoute();
  route2 = route1;
  BOOST_CHECK_EQUAL(route1, route2);

  route2.setFaceId(42);
  BOOST_CHECK_NE(route1, route2);

  route2 = route1;
  route2.setExpirationPeriod(1_min);
  BOOST_CHECK_NE(route1, route2);
}

BOOST_AUTO_TEST_CASE(RibEntryEncode)
{
  RibEntry entry1 = makeRibEntry();
  entry1.addRoute(Route()
                  .setFaceId(2)
                  .setOrigin(ROUTE_ORIGIN_APP)
                  .setCost(32)
                  .setFlags(ROUTE_FLAG_CHILD_INHERIT)
                  .setExpirationPeriod(5_s));
  const Block& wire = entry1.wireEncode();

  static const uint8_t expected[] = {
    0x80, 0x34, 0x07, 0x0e, 0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x08, 0x05, 0x77,
    0x6f, 0x72, 0x6c, 0x64, 0x81, 0x10, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01,
    0x64, 0x6c, 0x01, 0x02, 0x6d, 0x02, 0x27, 0x10, 0x81, 0x10, 0x69, 0x01, 0x02, 0x6f,
    0x01, 0x00, 0x6a, 0x01, 0x20, 0x6c, 0x01, 0x01, 0x6d, 0x02, 0x13, 0x88
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  RibEntry entry2(wire);
  BOOST_CHECK_EQUAL(entry1, entry2);
}

BOOST_AUTO_TEST_CASE(RibEntryClearRoutes)
{
  RibEntry entry;
  entry.setName("/hello/world");
  BOOST_CHECK_EQUAL(entry.getRoutes().size(), 0);

  Route route1;
  route1.setFaceId(42);
  entry.addRoute(route1);
  BOOST_REQUIRE_EQUAL(entry.getRoutes().size(), 1);
  BOOST_CHECK_EQUAL(entry.getRoutes().front(), route1);

  entry.clearRoutes();
  BOOST_CHECK_EQUAL(entry.getRoutes().size(), 0);
}

BOOST_AUTO_TEST_CASE(RibEntryEquality)
{
  RibEntry entry1, entry2;
  BOOST_CHECK_EQUAL(entry1, entry2);

  entry1 = entry2 = makeRibEntry();
  BOOST_CHECK_EQUAL(entry1, entry2);
  BOOST_CHECK_EQUAL(entry2, entry1);

  entry2.setName("/different/name");
  BOOST_CHECK_NE(entry1, entry2);

  entry2 = entry1;
  std::vector<Route> empty;
  entry2.setRoutes(empty.begin(), empty.end());
  BOOST_CHECK_NE(entry1, entry2);
  BOOST_CHECK_NE(entry2, entry1);

  entry2 = entry1;
  auto r1 = Route()
            .setFaceId(1)
            .setCost(1000);
  entry1.addRoute(r1);
  BOOST_CHECK_NE(entry1, entry2);
  BOOST_CHECK_NE(entry2, entry1);

  auto r42 = Route()
             .setFaceId(42)
             .setCost(42);
  entry1.addRoute(r42);
  entry2.addRoute(r42)
      .addRoute(r1);
  BOOST_CHECK_EQUAL(entry1, entry2); // order of Routes is irrelevant
  BOOST_CHECK_EQUAL(entry2, entry1);

  entry1 = entry2 = makeRibEntry();
  entry1.addRoute(r1)
      .addRoute(r42);
  entry2.addRoute(r42)
      .addRoute(r42);
  BOOST_CHECK_NE(entry1, entry2); // match each Route at most once
  BOOST_CHECK_NE(entry2, entry1);
}

BOOST_AUTO_TEST_CASE(Print)
{
  Route route;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(route),
                    "Route(FaceId: 0, Origin: app, Cost: 0, Flags: 0x1, ExpirationPeriod: infinite)");

  RibEntry entry;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(entry),
                    "RibEntry(Prefix: /,\n"
                    "         Routes: []\n"
                    "         )");

  entry = makeRibEntry();
  entry.addRoute(Route()
                 .setFaceId(2)
                 .setOrigin(ROUTE_ORIGIN_STATIC)
                 .setCost(32)
                 .setFlags(ROUTE_FLAG_CHILD_INHERIT));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(entry),
                    "RibEntry(Prefix: /hello/world,\n"
                    "         Routes: [Route(FaceId: 1, Origin: nlsr, Cost: 100, Flags: 0x2, "
                    "ExpirationPeriod: 10000 milliseconds),\n"
                    "                  Route(FaceId: 2, Origin: static, Cost: 32, Flags: 0x1, "
                    "ExpirationPeriod: infinite)]\n"
                    "         )");
}

BOOST_AUTO_TEST_SUITE_END() // TestRibEntry
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
