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

#include "util/in-memory-storage-persistent.hpp"
#include "security/key-chain.hpp"

#include "boost-test.hpp"
#include "../make-interest-data.hpp"

namespace ndn {
namespace util {
namespace tests {

BOOST_AUTO_TEST_SUITE(UtilInMemoryStorage)
BOOST_AUTO_TEST_SUITE(Persistent)

BOOST_AUTO_TEST_CASE(GetLimit)
{
  InMemoryStoragePersistent ims;

  BOOST_CHECK_EQUAL(ims.getLimit(), -1);
}

BOOST_AUTO_TEST_CASE(InsertAndDouble)
{
  InMemoryStoragePersistent ims;

  for(int i = 0; i < 11; i++) {
    std::ostringstream convert;
    convert << i;
    Name name("/" + convert.str());
    shared_ptr<Data> data = makeData(name);
    data->setFreshnessPeriod(time::milliseconds(5000));
    signData(data);
    ims.insert(*data);
  }

  BOOST_CHECK_EQUAL(ims.size(), 11);

  BOOST_CHECK_EQUAL(ims.getCapacity(), 20);
}

BOOST_AUTO_TEST_SUITE_END() // Persistent
BOOST_AUTO_TEST_SUITE_END() // UtilInMemoryStorage

} // namespace tests
} // namespace util
} // namespace ndn
