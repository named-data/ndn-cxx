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

#define BOOST_TEST_MAIN 1
#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE ndn-cxx Integrated Tests (Face)

#include "face.hpp"
#include "util/scheduler.hpp"
#include "security/key-chain.hpp"

#include "identity-management-fixture.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace tests {

class FacesFixture : public security::IdentityManagementFixture
{
public:
  FacesFixture()
    : nData(0)
    , nTimeouts(0)
    , regPrefixId(0)
    , nInInterests(0)
    , nInInterests2(0)
    , nRegFailures(0)
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

  void
  onInterest(Face& face,
             const Name&, const Interest&)
  {
    ++nInInterests;

    face.unsetInterestFilter(regPrefixId);
  }

  void
  onInterest2(Face& face,
              const Name&, const Interest&)
  {
    ++nInInterests2;

    face.unsetInterestFilter(regPrefixId2);
  }

  void
  onInterestRegex(Face& face,
                  const InterestFilter&, const Interest&)
  {
    ++nInInterests;
  }

  void
  onInterestRegexError(Face& face,
                       const Name&, const Interest&)
  {
    BOOST_FAIL("InterestFilter::Error should have been triggered");
  }

  void
  onRegFailed()
  {
    ++nRegFailures;
  }

  void
  expressInterest(Face& face, const Name& name)
  {
    Interest i(name);
    i.setInterestLifetime(time::milliseconds(50));
    face.expressInterest(i,
                         bind(&FacesFixture::onData, this),
                         bind(&FacesFixture::onTimeout, this));
  }

  void
  terminate(Face& face)
  {
    face.getIoService().stop();
  }

  uint32_t nData;
  uint32_t nTimeouts;

  const RegisteredPrefixId* regPrefixId;
  const RegisteredPrefixId* regPrefixId2;
  uint32_t nInInterests;
  uint32_t nInInterests2;
  uint32_t nRegFailures;
};

BOOST_FIXTURE_TEST_SUITE(TestFaces, FacesFixture)

BOOST_AUTO_TEST_CASE(Unix)
{
  Face face;

  face.expressInterest(Interest("/", time::milliseconds(1000)),
                       bind(&FacesFixture::onData, this),
                       bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(nTimeouts, 0);

  face.expressInterest(Interest("/localhost/non-existing/data/should/not/exist/anywhere",
                                time::milliseconds(50)),
                       bind(&FacesFixture::onData, this),
                       bind(&FacesFixture::onTimeout, this));

  Name veryLongName;
  for (size_t i = 0; i <= MAX_NDN_PACKET_SIZE / 10; i++)
    {
      veryLongName.append("0123456789");
    }

  BOOST_CHECK_THROW(face.expressInterest(veryLongName, OnData(), OnTimeout()), Face::Error);

  shared_ptr<Data> data = make_shared<Data>(veryLongName);
  data->setContent(reinterpret_cast<const uint8_t*>("01234567890"), 10);
  m_keyChain.sign(*data);
  BOOST_CHECK_THROW(face.put(*data), Face::Error);

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

BOOST_AUTO_TEST_CASE(Tcp)
{
  Face face("localhost");

  face.expressInterest(Interest("/", time::milliseconds(1000)),
                       bind(&FacesFixture::onData, this),
                       bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(nTimeouts, 0);

  face.expressInterest(Interest("/localhost/non-existing/data/should/not/exist/anywhere",
                                time::milliseconds(50)),
                       bind(&FacesFixture::onData, this),
                       bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}


BOOST_AUTO_TEST_CASE(SetFilter)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(4),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter("/Hello/World",
                                       bind(&FacesFixture::onInterest, this, ref(face), _1, _2),
                                       RegisterPrefixSuccessCallback(),
                                       bind(&FacesFixture::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/!")));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 1);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
  BOOST_CHECK_EQUAL(nData, 0);
}

BOOST_AUTO_TEST_CASE(SetTwoFilters)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(1),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter("/Hello/World",
                                       bind(&FacesFixture::onInterest, this, ref(face), _1, _2),
                                       RegisterPrefixSuccessCallback(),
                                       bind(&FacesFixture::onRegFailed, this));

  regPrefixId2 = face.setInterestFilter("/Los/Angeles/Lakers",
                                        bind(&FacesFixture::onInterest2, this, ref(face), _1, _2),
                                        RegisterPrefixSuccessCallback(),
                                        bind(&FacesFixture::onRegFailed, this));


  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/!")));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 1);
  BOOST_CHECK_EQUAL(nInInterests2, 0);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
  BOOST_CHECK_EQUAL(nData, 0);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterError)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(4),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                                       bind(&FacesFixture::onInterestRegexError, this,
                                            ref(face), _1, _2),
                                       RegisterPrefixSuccessCallback(),
                                       bind(&FacesFixture::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(300),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/XXX/b/c"))); // should match

  BOOST_REQUIRE_THROW(face.processEvents(), InterestFilter::Error);
}

BOOST_AUTO_TEST_CASE(SetRegexFilter)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(4),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                                       bind(&FacesFixture::onInterestRegex, this,
                                            ref(face), _1, _2),
                                       RegisterPrefixSuccessCallback(),
                                       bind(&FacesFixture::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a"))); // shouldn't match

  scheduler.scheduleEvent(time::milliseconds(300),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b"))); // should match

  scheduler.scheduleEvent(time::milliseconds(400),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b/c"))); // should match

  scheduler.scheduleEvent(time::milliseconds(500),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b/d"))); // should not match

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 2);
  BOOST_CHECK_EQUAL(nTimeouts, 4);
  BOOST_CHECK_EQUAL(nData, 0);
}

class FacesFixture2 : public FacesFixture
{
public:
  void
  checkPrefix(bool doesExist)
  {
    int result = std::system("nfd-status -r | grep /Hello/World >/dev/null");

    if (doesExist) {
      BOOST_CHECK_EQUAL(result, 0);
    }
    else {
      BOOST_CHECK_NE(result, 0);
    }
  }
};

BOOST_FIXTURE_TEST_CASE(RegisterUnregisterPrefix, FacesFixture2)
{
  Face face;
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(4),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter(InterestFilter("/Hello/World"),
                                       bind(&FacesFixture::onInterest, this,
                                            ref(face), _1, _2),
                                       RegisterPrefixSuccessCallback(),
                                       bind(&FacesFixture::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(500),
                          bind(&FacesFixture2::checkPrefix, this, true));

  scheduler.scheduleEvent(time::seconds(1),
    bind(static_cast<void(Face::*)(const RegisteredPrefixId*)>(&Face::unsetInterestFilter),
         &face,
    regPrefixId)); // shouldn't match

  scheduler.scheduleEvent(time::milliseconds(2000),
                          bind(&FacesFixture2::checkPrefix, this, false));

  BOOST_REQUIRE_NO_THROW(face.processEvents());
}


class FacesFixture3 : public FacesFixture2
{
public:
  FacesFixture3()
    : nRegSuccesses(0)
    , nUnregSuccesses(0)
    , nUnregFailures(0)
  {
  }

  void
  onRegSucceeded()
  {
    ++nRegSuccesses;
  }

  void
  onUnregSucceeded()
  {
    ++nUnregSuccesses;
  }

  void
  onUnregFailed()
  {
    ++nUnregFailures;
  }

public:
  uint64_t nRegSuccesses;
  uint64_t nUnregSuccesses;
  uint64_t nUnregFailures;
};

BOOST_FIXTURE_TEST_CASE(RegisterPrefix, FacesFixture3)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(2),
                          bind(&FacesFixture::terminate, this, ref(face)));

  scheduler.scheduleEvent(time::milliseconds(500),
                          bind(&FacesFixture2::checkPrefix, this, true));

  regPrefixId = face.registerPrefix("/Hello/World",
                                    bind(&FacesFixture3::onRegSucceeded, this),
                                    bind(&FacesFixture3::onRegFailed, this));

  scheduler.scheduleEvent(time::seconds(1),
    bind(&Face::unregisterPrefix, &face,
         regPrefixId,
         static_cast<UnregisterPrefixSuccessCallback>(bind(&FacesFixture3::onUnregSucceeded, this)),
         static_cast<UnregisterPrefixFailureCallback>(bind(&FacesFixture3::onUnregFailed, this))));

  scheduler.scheduleEvent(time::milliseconds(2500),
                          bind(&FacesFixture2::checkPrefix, this, false));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  BOOST_CHECK_EQUAL(nUnregFailures, 0);
  BOOST_CHECK_EQUAL(nUnregSuccesses, 1);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterButNoRegister)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(2),
                          bind(&FacesFixture::terminate, this, ref(face)));

  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind(&FacesFixture::onInterestRegex, this,
                              ref(face), _1, _2));

  // prefix is not registered, and also does not match regex
  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a")));

  // matches regex, but prefix is not registered
  scheduler.scheduleEvent(time::milliseconds(300),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b")));

  // matches regex, but prefix is not registered
  scheduler.scheduleEvent(time::milliseconds(400),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b/c")));

  // prefix is not registered, and also does not match regex
  scheduler.scheduleEvent(time::milliseconds(500),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b/d")));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 0);
  BOOST_CHECK_EQUAL(nTimeouts, 4);
  BOOST_CHECK_EQUAL(nData, 0);
}


BOOST_FIXTURE_TEST_CASE(SetRegexFilterAndRegister, FacesFixture3)
{
  Face face;
  Face face2(face.getIoService());
  Scheduler scheduler(face.getIoService());
  scheduler.scheduleEvent(time::seconds(2),
                          bind(&FacesFixture::terminate, this, ref(face)));

  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind(&FacesFixture::onInterestRegex, this,
                              ref(face), _1, _2));

  face.registerPrefix("/Hello/World",
                      bind(&FacesFixture3::onRegSucceeded, this),
                      bind(&FacesFixture3::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a"))); // shouldn't match

  scheduler.scheduleEvent(time::milliseconds(300),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b"))); // should match

  scheduler.scheduleEvent(time::milliseconds(400),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b/c"))); // should match

  scheduler.scheduleEvent(time::milliseconds(500),
                          bind(&FacesFixture::expressInterest, this,
                               ref(face2), Name("/Hello/World/a/b/d"))); // should not match

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  BOOST_CHECK_EQUAL(nInInterests, 2);
  BOOST_CHECK_EQUAL(nTimeouts, 4);
}

BOOST_AUTO_TEST_CASE(ShutdownWhileSendInProgress) // Bug #3136
{
  Face face;
  face.expressInterest(Name("/Hello/World/!"), bind([]{}), bind([]{}));
  BOOST_REQUIRE_NO_THROW(face.processEvents(time::seconds(1)));

  face.expressInterest(Name("/Bye/World/1"), bind([]{}), bind([]{}));
  face.expressInterest(Name("/Bye/World/2"), bind([]{}), bind([]{}));
  face.expressInterest(Name("/Bye/World/3"), bind([]{}), bind([]{}));
  face.shutdown();

  BOOST_REQUIRE_NO_THROW(face.processEvents(time::seconds(1)));
  // should not segfault
}

BOOST_AUTO_TEST_SUITE_END()

} // tests
} // namespace ndn
