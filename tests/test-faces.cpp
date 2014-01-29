/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>
#include "face.hpp"

using namespace std;
using namespace ndn;

BOOST_AUTO_TEST_SUITE(TestFaces)

struct FacesFixture
{
  FacesFixture()
    : dataCount(0)
    , timeoutCount(0)
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
  onInterest()
  {
  }

  void
  onRegFailed()
  {
  }

  uint32_t dataCount;
  uint32_t timeoutCount;
};

BOOST_FIXTURE_TEST_CASE (Unix, FacesFixture)
{
  Face face;

  face.expressInterest(Interest("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY", 1000),
                       ptr_lib::bind(&FacesFixture::onData, this),
                       ptr_lib::bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 0);

  face.expressInterest(Interest("/localhost/non-existing/data/should/not/exist/anywhere", 50),
                       ptr_lib::bind(&FacesFixture::onData, this),
                       ptr_lib::bind(&FacesFixture::onTimeout, this));
  
  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 1);
}

BOOST_FIXTURE_TEST_CASE (Tcp, FacesFixture)
{
  Face face("localhost");

  face.expressInterest(Interest("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY", 1000),
                       ptr_lib::bind(&FacesFixture::onData, this),
                       ptr_lib::bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 0);

  face.expressInterest(Interest("/localhost/non-existing/data/should/not/exist/anywhere", 50),
                       ptr_lib::bind(&FacesFixture::onData, this),
                       ptr_lib::bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face.processEvents());

  BOOST_CHECK_EQUAL(dataCount, 1);
  BOOST_CHECK_EQUAL(timeoutCount, 1);
}

BOOST_AUTO_TEST_SUITE_END()
