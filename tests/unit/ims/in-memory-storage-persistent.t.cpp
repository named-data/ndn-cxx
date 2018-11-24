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

#include "ndn-cxx/ims/in-memory-storage-persistent.hpp"

#include "tests/boost-test.hpp"
#include "tests/make-interest-data.hpp"

namespace ndn {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Ims)
BOOST_AUTO_TEST_SUITE(TestInMemoryStoragePersistent)

BOOST_AUTO_TEST_CASE(GetLimit)
{
  InMemoryStoragePersistent ims;

  BOOST_CHECK_EQUAL(ims.getLimit(), -1);
}

BOOST_AUTO_TEST_CASE(InsertAndDouble)
{
  InMemoryStoragePersistent ims;
  size_t initialCapacity = ims.getCapacity();

  for (size_t i = 0; i < initialCapacity + 1; i++) {
    shared_ptr<Data> data = makeData(to_string(i));
    data->setFreshnessPeriod(5000_ms);
    signData(data);
    ims.insert(*data);
  }

  BOOST_CHECK_EQUAL(ims.size(), initialCapacity + 1);

  BOOST_CHECK_EQUAL(ims.getCapacity(), initialCapacity * 2);
}

BOOST_AUTO_TEST_CASE(EraseAndShrink)
{
  InMemoryStoragePersistent ims;

  auto capacity = ims.getCapacity() * 2;
  ims.setCapacity(capacity);

  Name name("/1");
  shared_ptr<Data> data = makeData(name);
  data->setFreshnessPeriod(5000_ms);
  signData(data);
  ims.insert(*data);
  BOOST_CHECK_EQUAL(ims.size(), 1);

  ims.erase(name);

  BOOST_CHECK_EQUAL(ims.size(), 0);
  BOOST_CHECK_EQUAL(ims.getCapacity(), capacity / 2);
}

BOOST_AUTO_TEST_SUITE_END() // TestInMemoryStoragePersistent
BOOST_AUTO_TEST_SUITE_END() // Ims

} // namespace tests
} // namespace ndn
