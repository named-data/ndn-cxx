/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "face.hpp"
#include "security/key-chain.hpp"

#include "boost-test.hpp"
#include "util/dummy-client-face.hpp"

namespace ndn {
namespace tests {

using ndn::util::DummyClientFace;
using ndn::util::makeDummyClientFace;

BOOST_AUTO_TEST_SUITE(TestsFace)

class Fixture
{
public:
  Fixture()
    : face(makeDummyClientFace())
    , nData(0)
    , nTimeouts(0)
  {
  }

  void
  onData()
  {
    ++nData;
  }

  void
  onTimeout()
  {
    ++nTimeouts;
  }

public:
  shared_ptr<DummyClientFace> face;
  uint32_t nData;
  uint32_t nTimeouts;
};

BOOST_FIXTURE_TEST_CASE(ExpressInterest, Fixture)
{
  face->expressInterest(Interest("/test/interest", time::seconds(10)),
                        bind(&Fixture::onData, this),
                        bind(&Fixture::onTimeout, this));
  face->processEvents(time::milliseconds(100));

  BOOST_CHECK_EQUAL(face->sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(RemovePendingInterest, Fixture)
{
  const PendingInterestId* interestId =
    face->expressInterest(Interest("/test/interest", time::seconds(10)),
                          bind(&Fixture::onData, this),
                          bind(&Fixture::onTimeout, this));

  shared_ptr<Data> data = make_shared<Data>("/test/interest/replied");
  const uint8_t buffer[] = "Hello, world!";
  data->setContent(buffer, sizeof(buffer));
  KeyChain keyChain;
  keyChain.sign(*data);
  face->receive(*data);

  face->removePendingInterest(interestId);
  face->processEvents(time::milliseconds(100));
  BOOST_CHECK_EQUAL(nData, 0);
  BOOST_CHECK_EQUAL(nTimeouts, 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
