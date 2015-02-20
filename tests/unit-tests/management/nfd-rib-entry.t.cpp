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

#include "management/nfd-rib-entry.hpp"
#include "management/nfd-control-command.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdRibEntry)

const uint8_t RouteData[] =
{
  0x81, 0x10, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01, 0x64, 0x6c, 0x01, 0x02,
  0x6d, 0x02, 0x27, 0x10
};

const uint8_t RouteInfiniteExpirationPeriod[] =
{
  0x81, 0x0C, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01, 0x64, 0x6c, 0x01, 0x02
};

const uint8_t RibEntryData[] =
{
  // Header + Name
  0x80, 0x34, 0x07, 0x0e, 0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f,
  0x08, 0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64,
  // Route
  0x81, 0x10, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01, 0x64, 0x6c, 0x01, 0x02,
  0x6d, 0x02, 0x27, 0x10,
  // Route
  0x81, 0x10, 0x69, 0x01, 0x02, 0x6f, 0x01, 0x00, 0x6a, 0x01, 0x20, 0x6c, 0x01, 0x01,
  0x6d, 0x02, 0x13, 0x88
};

const uint8_t RibEntryInfiniteExpirationPeriod[] =
{
  // Header + Name
  0x80, 0x30, 0x07, 0x0e, 0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f,
  0x08, 0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64,
  // Route
  0x81, 0x10, 0x69, 0x01, 0x01, 0x6f, 0x01, 0x80, 0x6a, 0x01, 0x64, 0x6c, 0x01, 0x02,
  0x6d, 0x02, 0x27, 0x10,
  // Route with no ExpirationPeriod
  0x81, 0x0C, 0x69, 0x01, 0x02, 0x6f, 0x01, 0x00, 0x6a, 0x01, 0x20, 0x6c, 0x01, 0x01,
};

BOOST_AUTO_TEST_CASE(RouteEncode)
{
  Route route;
  route.setFaceId(1);
  route.setOrigin(128);
  route.setCost(100);
  route.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route.setExpirationPeriod(time::milliseconds(10000));

  const Block& wire = route.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(RouteData,
                                  RouteData + sizeof(RouteData),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(RouteDecode)
{
  Route route;

  BOOST_REQUIRE_NO_THROW(route.wireDecode(Block(RouteData, sizeof(RouteData))));

  BOOST_REQUIRE_EQUAL(route.getFaceId(), 1);
  BOOST_REQUIRE_EQUAL(route.getOrigin(), 128);
  BOOST_REQUIRE_EQUAL(route.getCost(), 100);
  BOOST_REQUIRE_EQUAL(route.getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CAPTURE));
  BOOST_REQUIRE_EQUAL(route.getExpirationPeriod(), time::milliseconds(10000));
  BOOST_REQUIRE_EQUAL(route.hasInfiniteExpirationPeriod(), false);
}

BOOST_AUTO_TEST_CASE(RouteInfiniteExpirationPeriodEncode)
{
  Route route;
  route.setFaceId(1);
  route.setOrigin(128);
  route.setCost(100);
  route.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route.setExpirationPeriod(Route::INFINITE_EXPIRATION_PERIOD);

  const Block& wire = route.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(RouteInfiniteExpirationPeriod,
                                  RouteInfiniteExpirationPeriod + sizeof(RouteInfiniteExpirationPeriod),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(RouteInfiniteExpirationPeriodDecode)
{
  Route route;

  BOOST_REQUIRE_NO_THROW(route.wireDecode(Block(RouteInfiniteExpirationPeriod,
                                                sizeof(RouteInfiniteExpirationPeriod))));

  BOOST_REQUIRE_EQUAL(route.getFaceId(), 1);
  BOOST_REQUIRE_EQUAL(route.getOrigin(), 128);
  BOOST_REQUIRE_EQUAL(route.getCost(), 100);
  BOOST_REQUIRE_EQUAL(route.getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CAPTURE));
  BOOST_REQUIRE_EQUAL(route.getExpirationPeriod(), Route::INFINITE_EXPIRATION_PERIOD);
  BOOST_REQUIRE_EQUAL(route.hasInfiniteExpirationPeriod(), true);
}

BOOST_AUTO_TEST_CASE(RouteOutputStream)
{
  Route route;
  route.setFaceId(1);
  route.setOrigin(128);
  route.setCost(100);
  route.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route.setExpirationPeriod(time::milliseconds(10000));

  std::ostringstream os;
  os << route;

  BOOST_CHECK_EQUAL(os.str(), "Route(FaceId: 1, Origin: 128, Cost: 100, "
                              "Flags: 2, ExpirationPeriod: 10000 milliseconds)");
}

BOOST_AUTO_TEST_CASE(RibEntryEncode)
{
  RibEntry entry;
  entry.setName("/hello/world");

  Route route1;
  route1.setFaceId(1);
  route1.setOrigin(128);
  route1.setCost(100);
  route1.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route1.setExpirationPeriod(time::milliseconds(10000));
  entry.addRoute(route1);

  Route route2;
  route2.setFaceId(2);
  route2.setOrigin(0);
  route2.setCost(32);
  route2.setFlags(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT);
  route2.setExpirationPeriod(time::milliseconds(5000));
  entry.addRoute(route2);

  const Block& wire = entry.wireEncode();

  BOOST_CHECK_EQUAL_COLLECTIONS(RibEntryData,
                                RibEntryData + sizeof(RibEntryData),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(RibEntryDecode)
{
  RibEntry entry;
  BOOST_REQUIRE_NO_THROW(entry.wireDecode(Block(RibEntryData,
                                          sizeof(RibEntryData))));

  BOOST_CHECK_EQUAL(entry.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(entry.getRoutes().size(), 2);

  std::list<Route> routes = entry.getRoutes();

  std::list<Route>::const_iterator it = routes.begin();
  BOOST_CHECK_EQUAL(it->getFaceId(), 1);
  BOOST_CHECK_EQUAL(it->getOrigin(), 128);
  BOOST_CHECK_EQUAL(it->getCost(), 100);
  BOOST_CHECK_EQUAL(it->getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CAPTURE));
  BOOST_CHECK_EQUAL(it->getExpirationPeriod(), time::milliseconds(10000));
  BOOST_CHECK_EQUAL(it->hasInfiniteExpirationPeriod(), false);

  ++it;
  BOOST_CHECK_EQUAL(it->getFaceId(), 2);
  BOOST_CHECK_EQUAL(it->getOrigin(), 0);
  BOOST_CHECK_EQUAL(it->getCost(), 32);
  BOOST_CHECK_EQUAL(it->getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT));
  BOOST_CHECK_EQUAL(it->getExpirationPeriod(), time::milliseconds(5000));
  BOOST_CHECK_EQUAL(it->hasInfiniteExpirationPeriod(), false);
}

BOOST_AUTO_TEST_CASE(RibEntryInfiniteExpirationPeriodEncode)
{
  RibEntry entry;
  entry.setName("/hello/world");

  Route route1;
  route1.setFaceId(1);
  route1.setOrigin(128);
  route1.setCost(100);
  route1.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route1.setExpirationPeriod(time::milliseconds(10000));
  entry.addRoute(route1);

  Route route2;
  route2.setFaceId(2);
  route2.setOrigin(0);
  route2.setCost(32);
  route2.setFlags(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT);
  route2.setExpirationPeriod(Route::INFINITE_EXPIRATION_PERIOD);
  entry.addRoute(route2);

  const Block& wire = entry.wireEncode();

  BOOST_CHECK_EQUAL_COLLECTIONS(RibEntryInfiniteExpirationPeriod,
                                RibEntryInfiniteExpirationPeriod +
                                sizeof(RibEntryInfiniteExpirationPeriod),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(RibEntryInfiniteExpirationPeriodDecode)
{
  RibEntry entry;
  BOOST_REQUIRE_NO_THROW(entry.wireDecode(Block(RibEntryInfiniteExpirationPeriod,
                                          sizeof(RibEntryInfiniteExpirationPeriod))));

  BOOST_CHECK_EQUAL(entry.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(entry.getRoutes().size(), 2);

  std::list<Route> routes = entry.getRoutes();

  std::list<Route>::const_iterator it = routes.begin();
  BOOST_CHECK_EQUAL(it->getFaceId(), 1);
  BOOST_CHECK_EQUAL(it->getOrigin(), 128);
  BOOST_CHECK_EQUAL(it->getCost(), 100);
  BOOST_CHECK_EQUAL(it->getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CAPTURE));
  BOOST_CHECK_EQUAL(it->getExpirationPeriod(), time::milliseconds(10000));
  BOOST_CHECK_EQUAL(it->hasInfiniteExpirationPeriod(), false);

  ++it;
  BOOST_CHECK_EQUAL(it->getFaceId(), 2);
  BOOST_CHECK_EQUAL(it->getOrigin(), 0);
  BOOST_CHECK_EQUAL(it->getCost(), 32);
  BOOST_CHECK_EQUAL(it->getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT));
  BOOST_CHECK_EQUAL(it->getExpirationPeriod(), Route::INFINITE_EXPIRATION_PERIOD);
  BOOST_CHECK_EQUAL(it->hasInfiniteExpirationPeriod(), true);
}

BOOST_AUTO_TEST_CASE(RibEntryClear)
{
  RibEntry entry;
  entry.setName("/hello/world");

  Route route1;
  route1.setFaceId(1);
  route1.setOrigin(128);
  route1.setCost(100);
  route1.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route1.setExpirationPeriod(time::milliseconds(10000));
  entry.addRoute(route1);
  BOOST_REQUIRE_EQUAL(entry.getRoutes().size(), 1);

  std::list<Route> routes = entry.getRoutes();

  std::list<Route>::const_iterator it = routes.begin();
  BOOST_CHECK_EQUAL(it->getFaceId(), 1);
  BOOST_CHECK_EQUAL(it->getOrigin(), 128);
  BOOST_CHECK_EQUAL(it->getCost(), 100);
  BOOST_CHECK_EQUAL(it->getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CAPTURE));
  BOOST_CHECK_EQUAL(it->getExpirationPeriod(), time::milliseconds(10000));
  BOOST_CHECK_EQUAL(it->hasInfiniteExpirationPeriod(), false);

  entry.clearRoutes();
  BOOST_CHECK_EQUAL(entry.getRoutes().size(), 0);

  Route route2;
  route2.setFaceId(2);
  route2.setOrigin(0);
  route2.setCost(32);
  route2.setFlags(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT);
  route2.setExpirationPeriod(Route::INFINITE_EXPIRATION_PERIOD);
  entry.addRoute(route2);
  BOOST_REQUIRE_EQUAL(entry.getRoutes().size(), 1);

  routes = entry.getRoutes();

  it = routes.begin();
  BOOST_CHECK_EQUAL(it->getFaceId(), 2);
  BOOST_CHECK_EQUAL(it->getOrigin(), 0);
  BOOST_CHECK_EQUAL(it->getCost(), 32);
  BOOST_CHECK_EQUAL(it->getFlags(), static_cast<uint64_t>(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT));
  BOOST_CHECK_EQUAL(it->getExpirationPeriod(), Route::INFINITE_EXPIRATION_PERIOD);
  BOOST_CHECK_EQUAL(it->hasInfiniteExpirationPeriod(), true);
}

BOOST_AUTO_TEST_CASE(RibEntryOutputStream)
{
  RibEntry entry;
  entry.setName("/hello/world");

  Route route1;
  route1.setFaceId(1);
  route1.setOrigin(128);
  route1.setCost(100);
  route1.setFlags(ndn::nfd::ROUTE_FLAG_CAPTURE);
  route1.setExpirationPeriod(time::milliseconds(10000));
  entry.addRoute(route1);

  Route route2;
  route2.setFaceId(2);
  route2.setOrigin(0);
  route2.setCost(32);
  route2.setFlags(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT);
  route2.setExpirationPeriod(Route::INFINITE_EXPIRATION_PERIOD);
  entry.addRoute(route2);

  std::ostringstream os;
  os << entry;

  BOOST_CHECK_EQUAL(os.str(), "RibEntry{\n"
                              "  Name: /hello/world\n"
                              "  Route(FaceId: 1, Origin: 128, Cost: 100, "
                              "Flags: 2, ExpirationPeriod: 10000 milliseconds)\n"
                              "  Route(FaceId: 2, Origin: 0, Cost: 32, "
                              "Flags: 1, ExpirationPeriod: Infinity)\n"
                              "}");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
