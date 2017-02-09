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

#include "mgmt/nfd/strategy-choice.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestStrategyChoice)

BOOST_AUTO_TEST_CASE(Encode)
{
  StrategyChoice sc1;
  sc1.setName("/hello/world")
     .setStrategy("/some/non/existing/strategy/name");
  Block wire = sc1.wireEncode();

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x80, 0x39, 0x07, 0x0e, 0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x08, 0x05, 0x77,
    0x6f, 0x72, 0x6c, 0x64, 0x6b, 0x27, 0x07, 0x25, 0x08, 0x04, 0x73, 0x6f, 0x6d, 0x65,
    0x08, 0x03, 0x6e, 0x6f, 0x6e, 0x08, 0x08, 0x65, 0x78, 0x69, 0x73, 0x74, 0x69, 0x6e,
    0x67, 0x08, 0x08, 0x73, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x08, 0x04, 0x6e,
    0x61, 0x6d, 0x65
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  StrategyChoice sc2(wire);
  BOOST_CHECK_EQUAL(sc1, sc2);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  StrategyChoice sc1, sc2;

  sc1.setName("/A")
     .setStrategy("/strategyP");
  sc2 = sc1;
  BOOST_CHECK_EQUAL(sc1, sc2);

  sc2.setName("/B");
  BOOST_CHECK_NE(sc1, sc2);

  sc2 = sc1;
  sc2.setStrategy("/strategyQ");
  BOOST_CHECK_NE(sc1, sc2);
}

BOOST_AUTO_TEST_CASE(Print)
{
  StrategyChoice sc;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(sc),
                    "StrategyChoice(Name: /, Strategy: /)");

  sc.setName("/A")
    .setStrategy("/strategyP");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(sc),
                    "StrategyChoice(Name: /A, Strategy: /strategyP)");
}

BOOST_AUTO_TEST_SUITE_END() // TestStrategyChoice
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
