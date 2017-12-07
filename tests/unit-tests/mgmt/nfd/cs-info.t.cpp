/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "mgmt/nfd/cs-info.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestCsInfo)

static CsInfo
makeCsInfo()
{
  return CsInfo()
    .setNHits(12951)
    .setNMisses(28179);
}

BOOST_AUTO_TEST_CASE(Encode)
{
  CsInfo csi1 = makeCsInfo();
  Block wire = csi1.wireEncode();

  static const uint8_t EXPECTED[] = {
    0x80, 0x08, // CsInfo
          0x81, 0x02, 0x32, 0x97, // NHits
          0x82, 0x02, 0x6E, 0x13, // NMisses
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(wire.begin(), wire.end(), EXPECTED, EXPECTED + sizeof(EXPECTED));

  CsInfo csi2(wire);
  BOOST_CHECK_EQUAL(csi1, csi2);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  CsInfo csi1, csi2;
  BOOST_CHECK_EQUAL(csi1, csi2);

  csi1 = makeCsInfo();
  BOOST_CHECK_NE(csi1, csi2);
  csi2 = csi1;
  BOOST_CHECK_EQUAL(csi1, csi2);

  csi2.setNHits(8267);
  BOOST_CHECK_NE(csi1, csi2);
}

BOOST_AUTO_TEST_CASE(Print)
{
  CsInfo csi;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(csi), "CS: 0 hits, 0 misses");

  csi = makeCsInfo();
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(csi), "CS: 12951 hits, 28179 misses");

  csi.setNHits(1).setNMisses(1);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(csi), "CS: 1 hit, 1 miss");
}

BOOST_AUTO_TEST_SUITE_END() // TestCsInfo
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
