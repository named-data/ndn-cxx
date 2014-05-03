/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "face.hpp"
#include "util/scheduler.hpp"

#include "boost-test.hpp"

namespace ndn {


class FacesFixture
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
    face.shutdown();
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
  Face face2(face.ioService());
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::milliseconds(300),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter("/Hello/World",
                                       bind(&FacesFixture::onInterest, this, ref(face), _1, _2),
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
  Face face2(face.ioService());
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::seconds(1),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter("/Hello/World",
                                       bind(&FacesFixture::onInterest, this, ref(face), _1, _2),
                                       bind(&FacesFixture::onRegFailed, this));

  regPrefixId2 = face.setInterestFilter("/Los/Angeles/Lakers",
                                        bind(&FacesFixture::onInterest2, this, ref(face), _1, _2),
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
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::seconds(1),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                                       bind(&FacesFixture::onInterestRegexError, this,
                                            ref(face), _1, _2),
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
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::seconds(2),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                                       bind(&FacesFixture::onInterestRegex, this,
                                            ref(face), _1, _2),
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
    int result = std::system("nfd-status | grep /Hello/World >/dev/null");

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
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::seconds(2),
                          bind(&FacesFixture::terminate, this, ref(face)));

  regPrefixId = face.setInterestFilter(InterestFilter("/Hello/World"),
                                       bind(&FacesFixture::onInterest, this,
                                            ref(face), _1, _2),
                                       bind(&FacesFixture::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(500),
                          bind(&FacesFixture2::checkPrefix, this, true));

  scheduler.scheduleEvent(time::seconds(1),
                          bind(&Face::unsetInterestFilter, &face,
                               regPrefixId)); // shouldn't match

  scheduler.scheduleEvent(time::milliseconds(1500),
                          bind(&FacesFixture2::checkPrefix, this, false));

  BOOST_REQUIRE_NO_THROW(face.processEvents());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
