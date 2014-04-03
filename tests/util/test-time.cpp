/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "util/time.hpp"

#include "boost-test.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestTime)

BOOST_AUTO_TEST_CASE (SystemClock)
{
  time::system_clock::TimePoint value = time::system_clock::now();
  time::system_clock::TimePoint referenceTime =
    time::fromUnixTimestamp(time::milliseconds(1390966967032));

  BOOST_CHECK_GT(value, referenceTime);

  BOOST_CHECK_EQUAL(time::toIsoString(referenceTime), "20140129T034247.032000");
  BOOST_CHECK_EQUAL(time::fromIsoString("20140129T034247.032000"), referenceTime);

  BOOST_CHECK_EQUAL(time::fromIsoString("20140129T034247.032000Z"), referenceTime);
}

BOOST_AUTO_TEST_CASE (SteadyClock)
{
  time::steady_clock::TimePoint oldValue = time::steady_clock::now();
  usleep(100);
  time::steady_clock::TimePoint newValue = time::steady_clock::now();

  BOOST_CHECK_GT(newValue, oldValue);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
