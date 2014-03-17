/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>
#include "face.hpp"
#include "util/scheduler.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(TestFaces)

struct FacesFixture
{
  FacesFixture()
    : dataCount(0)
    , timeoutCount(0)
    , regPrefixId(0)
    , inInterestCount(0)
    , inInterestCount2(0)
    , regFailedCount(0)
  {
  }
  
  void
  onData()
  {
    ++dataCount;
  }

  void
  onTimeout()
  {
    ++timeoutCount;
  }

  void
  onInterest(Face& face)
  {
    ++inInterestCount;

    face.unsetInterestFilter(regPrefixId);
  }

  void
  onInterest2(Face& face)
  {
    ++inInterestCount2;

    face.unsetInterestFilter(regPrefixId2);
  }

  void
  onRegFailed()
  {
    ++regFailedCount;
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

  uint32_t dataCount;
  uint32_t timeoutCount;

  const RegisteredPrefixId* regPrefixId;
  const RegisteredPrefixId* regPrefixId2;
  uint32_t inInterestCount;
  uint32_t inInterestCount2;
  uint32_t regFailedCount;
};

BOOST_FIXTURE_TEST_CASE (Unix, FacesFixture)
{
  Face face;

  face.expressInterest(Interest("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY", time::milliseconds(1000)),
                       ptr_lib::bind(&FacesFixture::onData, this),
                       ptr_lib::bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 0);

  face.expressInterest(Interest("/localhost/non-existing/data/should/not/exist/anywhere", time::milliseconds(50)),
                       ptr_lib::bind(&FacesFixture::onData, this),
                       ptr_lib::bind(&FacesFixture::onTimeout, this));
  
  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 1);
}

BOOST_FIXTURE_TEST_CASE (Tcp, FacesFixture)
{
  Face face("localhost");

  face.expressInterest(Interest("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY", time::milliseconds(1000)),
                       bind(&FacesFixture::onData, this),
                       bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 0);

  face.expressInterest(Interest("/localhost/non-existing/data/should/not/exist/anywhere", time::milliseconds(50)),
                       bind(&FacesFixture::onData, this),
                       bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 1);
}


BOOST_FIXTURE_TEST_CASE (SetFilter, FacesFixture)
{
  Face face;
  Face face2(face.ioService());
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::milliseconds(300),
                          bind(&FacesFixture::terminate, this, func_lib::ref(face)));
  
  regPrefixId = face.setInterestFilter("/Hello/World",
                                       bind(&FacesFixture::onInterest, this, func_lib::ref(face)),
                                       bind(&FacesFixture::onRegFailed, this));

  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               func_lib::ref(face2), Name("/Hello/World/!")));
  
  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(regFailedCount, 0);  
  BOOST_CHECK_EQUAL(inInterestCount, 1);  
  BOOST_CHECK_EQUAL(timeoutCount, 1);
  BOOST_CHECK_EQUAL(dataCount, 0);
}

BOOST_FIXTURE_TEST_CASE (SetTwoFilters, FacesFixture)
{
  Face face;
  Face face2(face.ioService());
  Scheduler scheduler(*face.ioService());
  scheduler.scheduleEvent(time::seconds(1),
                          bind(&FacesFixture::terminate, this, func_lib::ref(face)));
  
  regPrefixId = face.setInterestFilter("/Hello/World",
                                       bind(&FacesFixture::onInterest, this, func_lib::ref(face)),
                                       bind(&FacesFixture::onRegFailed, this));
  
  regPrefixId2 = face.setInterestFilter("/Los/Angeles/Lakers",
                                       bind(&FacesFixture::onInterest2, this, func_lib::ref(face)),
                                       bind(&FacesFixture::onRegFailed, this));


  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::expressInterest, this,
                               func_lib::ref(face2), Name("/Hello/World/!")));
  
  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(regFailedCount, 0);  
  BOOST_CHECK_EQUAL(inInterestCount, 1);  
  BOOST_CHECK_EQUAL(inInterestCount2, 0);  
  BOOST_CHECK_EQUAL(timeoutCount, 1);
  BOOST_CHECK_EQUAL(dataCount, 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
