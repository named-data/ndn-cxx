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

#include "ims/in-memory-storage-fifo.hpp"

#include "boost-test.hpp"
#include "make-interest-data.hpp"

namespace ndn {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Ims)
BOOST_AUTO_TEST_SUITE(TestInMemoryStorageFifo)

BOOST_AUTO_TEST_CASE(ArrivalQueue)
{
  InMemoryStorageFifo ims;

  ims.insert(*makeData("/1"));
  ims.insert(*makeData("/2"));
  ims.insert(*makeData("/3"));

  ims.evictItem();
  BOOST_CHECK_EQUAL(ims.size(), 2);

  shared_ptr<Interest> interest = makeInterest("/1");
  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK(found == nullptr);
}

BOOST_AUTO_TEST_CASE(ArrivalQueue2)
{
  InMemoryStorageFifo ims;

  ims.insert(*makeData("/1"));
  ims.insert(*makeData("/2"));
  ims.insert(*makeData("/3"));

  ims.evictItem();
  BOOST_CHECK_EQUAL(ims.size(), 2);

  shared_ptr<Interest> interest1 = makeInterest("/1");
  shared_ptr<const Data> found1 = ims.find(*interest1);
  BOOST_CHECK(found1 == nullptr);

  ims.insert(*makeData("/4"));

  ims.evictItem();
  BOOST_CHECK_EQUAL(ims.size(), 2);

  shared_ptr<Interest> interest2 = makeInterest("/2");
  shared_ptr<const Data> found2 = ims.find(*interest2);
  BOOST_CHECK(found2 == nullptr);
}

BOOST_AUTO_TEST_SUITE_END() // TestInMemoryStorageFifo
BOOST_AUTO_TEST_SUITE_END() // Ims

} // namespace tests
} // namespace ndn
