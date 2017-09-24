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

#include "packet-base.hpp"

#include "../boost-test.hpp"
#include "interest.hpp"
#include "lp/tags.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestPacketBase)

BOOST_AUTO_TEST_CASE(CongestionMark)
{
  Interest interest;

  BOOST_CHECK_EQUAL(interest.getCongestionMark(), 0);

  auto tag = interest.getTag<lp::CongestionMarkTag>();
  BOOST_CHECK(!tag);

  interest.setCongestionMark(true);
  tag = interest.getTag<lp::CongestionMarkTag>();
  BOOST_REQUIRE(tag);
  BOOST_CHECK_EQUAL(*tag, 1);

  interest.setCongestionMark(false);
  tag = interest.getTag<lp::CongestionMarkTag>();
  BOOST_CHECK(!tag);

  interest.setCongestionMark(300);
  tag = interest.getTag<lp::CongestionMarkTag>();
  BOOST_REQUIRE(tag);
  BOOST_CHECK_EQUAL(*tag, 300);

  interest.setCongestionMark(0);
  tag = interest.getTag<lp::CongestionMarkTag>();
  BOOST_CHECK(!tag);
}

BOOST_AUTO_TEST_SUITE_END() // TestPacketBase

} // namespace tests
} // namespace ndn
