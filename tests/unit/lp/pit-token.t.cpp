/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/lp/pit-token.hpp"
#include "ndn-cxx/lp/packet.hpp"

#include "tests/boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace lp {
namespace tests {

BOOST_AUTO_TEST_SUITE(Lp)
BOOST_AUTO_TEST_SUITE(TestPitToken)

BOOST_AUTO_TEST_CASE(Decode)
{
  Packet pkt("6405 pit-token=6200 fragment=5001C0"_block);
  BOOST_CHECK_THROW(PitToken(pkt.get<PitTokenField>()), ndn::tlv::Error);

  pkt.wireDecode("6406 pit-token=6201A0 fragment=5001C0"_block);
  PitToken pitToken1(pkt.get<PitTokenField>());
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(pitToken1), "a0");

  pkt.wireDecode("640A pit-token=6205A0A1A2A3A4 fragment=5001C0"_block);
  PitToken pitToken5(pkt.get<PitTokenField>());
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(pitToken5), "a0a1a2a3a4");

  pkt.wireDecode("640D pit-token=6208A0A1A2A3A4A5A6A7 fragment=5001C0"_block);
  PitToken pitToken8(pkt.get<PitTokenField>());
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(pitToken8), "a0a1a2a3a4a5a6a7");

  pkt.wireDecode("6425 pit-token=6220A0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
                 " fragment=5001C0"_block);
  PitToken pitToken32(pkt.get<PitTokenField>());
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(pitToken32),
                    "a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebf");

  pkt.wireDecode("6426 pit-token=6221A0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0"
                 " fragment=5001C0"_block);
  BOOST_CHECK_THROW(PitToken(pkt.get<PitTokenField>()), ndn::tlv::Error);

  BOOST_CHECK_EQUAL(pitToken1, pitToken1);
  BOOST_CHECK_NE(pitToken1, pitToken5);

  pkt.wireDecode("640A pit-token=6205B0B1B2B3B4 fragment=5001C0"_block);
  PitToken pitToken5b(pkt.get<PitTokenField>());
  BOOST_CHECK_NE(pitToken5, pitToken5b);

  pkt.set<PitTokenField>(pitToken5);
  BOOST_CHECK_EQUAL(pkt.wireEncode(), "640A pit-token=6205A0A1A2A3A4 fragment=5001C0"_block);
  PitToken pitToken5a(pkt.get<PitTokenField>());
  BOOST_CHECK_EQUAL(pitToken5, pitToken5a);
}

BOOST_AUTO_TEST_SUITE_END() // TestPitToken
BOOST_AUTO_TEST_SUITE_END() // Lp

} // namespace tests
} // namespace lp
} // namespace ndn
