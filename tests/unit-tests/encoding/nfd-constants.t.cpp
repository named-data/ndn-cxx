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

#include "encoding/nfd-constants.hpp"

#include "boost-test.hpp"

#include <boost/lexical_cast.hpp>
#include <sstream>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestNfdConstants)

BOOST_AUTO_TEST_CASE(PrintFaceScope)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_SCOPE_NONE), "none");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_SCOPE_NON_LOCAL), "non-local");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_SCOPE_LOCAL), "local");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<FaceScope>(126)), "126");
}

BOOST_AUTO_TEST_CASE(PrintFacePersistency)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_PERSISTENCY_NONE), "none");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_PERSISTENCY_ON_DEMAND), "on-demand");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_PERSISTENCY_PERSISTENT), "persistent");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_PERSISTENCY_PERMANENT), "permanent");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<FacePersistency>(110)), "110");
}

BOOST_AUTO_TEST_CASE(PrintLinkType)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(LINK_TYPE_NONE), "none");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(LINK_TYPE_POINT_TO_POINT), "point-to-point");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(LINK_TYPE_MULTI_ACCESS), "multi-access");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(LINK_TYPE_AD_HOC), "adhoc");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<LinkType>(104)), "104");
}

BOOST_AUTO_TEST_CASE(PrintFaceEventKind)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_EVENT_NONE), "none");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_EVENT_CREATED), "created");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_EVENT_DESTROYED), "destroyed");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_EVENT_UP), "up");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(FACE_EVENT_DOWN), "down");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<FaceEventKind>(175)), "175");
}

BOOST_AUTO_TEST_CASE(ParseRouteOrigin)
{
  auto expectSuccess = [] (const std::string& input, RouteOrigin expected) {
    std::istringstream is(input);
    RouteOrigin routeOrigin;
    is >> routeOrigin;

    BOOST_TEST_MESSAGE("parsing " << input);
    BOOST_CHECK_EQUAL(routeOrigin, expected);
  };

  auto expectFail = [] (const std::string& input) {
    std::istringstream is(input);
    RouteOrigin routeOrigin;
    is >> routeOrigin;

    BOOST_TEST_MESSAGE("parsing " << input);
    BOOST_CHECK(is.fail());
    BOOST_CHECK_EQUAL(routeOrigin, ROUTE_ORIGIN_NONE);
  };

  expectSuccess("none", ROUTE_ORIGIN_NONE);
  expectSuccess("App", ROUTE_ORIGIN_APP);
  expectSuccess("AutoReg", ROUTE_ORIGIN_AUTOREG);
  expectSuccess("Client", ROUTE_ORIGIN_CLIENT);
  expectSuccess("AutoConf", ROUTE_ORIGIN_AUTOCONF);
  expectSuccess("NLSR", ROUTE_ORIGIN_NLSR);
  expectSuccess("SelfLearning", ROUTE_ORIGIN_SELFLEARNING);
  expectSuccess("static", ROUTE_ORIGIN_STATIC);
  expectSuccess("27", static_cast<RouteOrigin>(27));

  expectSuccess(" app", ROUTE_ORIGIN_APP);
  expectSuccess("app ", ROUTE_ORIGIN_APP);
  expectSuccess(" app ", ROUTE_ORIGIN_APP);

  expectFail("unrecognized");
  expectFail("-1");
  expectFail("0.1");
  expectFail("65537");
  expectFail("");
}

BOOST_AUTO_TEST_CASE(PrintRouteOrigin)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_NONE), "none");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_APP), "app");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_AUTOREG), "autoreg");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_CLIENT), "client");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_AUTOCONF), "autoconf");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_NLSR), "nlsr");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_SELFLEARNING), "selflearning");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_ORIGIN_STATIC), "static");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<RouteOrigin>(27)), "27");
}

BOOST_AUTO_TEST_CASE(PrintRouteFlags)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_FLAGS_NONE), "none");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_FLAG_CHILD_INHERIT), "child-inherit");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(ROUTE_FLAG_CAPTURE), "capture");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<RouteFlags>(
                    ROUTE_FLAG_CHILD_INHERIT | ROUTE_FLAG_CAPTURE)), "child-inherit|capture");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<RouteFlags>(
                    ROUTE_FLAG_CAPTURE | 0x9c)), "capture|0x9c");
}

BOOST_AUTO_TEST_SUITE_END() // TestNfdConstants
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace nfd
} // namespace ndn
