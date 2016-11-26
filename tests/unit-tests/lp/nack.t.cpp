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
 *
 * @author Eric Newberry <enewberry@email.arizona.edu>
 */

#include "lp/nack.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace lp {
namespace tests {

BOOST_AUTO_TEST_SUITE(Lp)
BOOST_AUTO_TEST_SUITE(TestNack)

BOOST_AUTO_TEST_CASE(Members)
{
  Name name("ndn:/test");
  Interest interest(name);
  Nack nack(interest);

  BOOST_CHECK_EQUAL(nack.getInterest().getName(), name);

  NackHeader header;
  header.setReason(NackReason::CONGESTION);
  nack.setHeader(header);
  BOOST_CHECK_EQUAL(nack.getHeader().getReason(), header.getReason());

  BOOST_CHECK_EQUAL(nack.getHeader().getReason(), nack.getReason());

  nack.setReason(NackReason::DUPLICATE);
  BOOST_CHECK_EQUAL(nack.getReason(), NackReason::DUPLICATE);

  nack.setReason(NackReason::NO_ROUTE);
  BOOST_CHECK_EQUAL(nack.getReason(), NackReason::NO_ROUTE);

  Nack nack2(interest);
  BOOST_CHECK_EQUAL(nack2.getReason(), NackReason::NONE);
}

BOOST_AUTO_TEST_SUITE_END() // TestNack
BOOST_AUTO_TEST_SUITE_END() // Lp

} // namespace tests
} // namespace lp
} // namespace ndn
