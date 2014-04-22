/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "util/time.hpp"

#include "boost-test.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(UtilTestTime)

BOOST_AUTO_TEST_CASE(SystemClock)
{
  time::system_clock::TimePoint value = time::system_clock::now();
  time::system_clock::TimePoint referenceTime =
    time::fromUnixTimestamp(time::milliseconds(1390966967032LL));

  BOOST_CHECK_GT(value, referenceTime);

  BOOST_CHECK_EQUAL(time::toIsoString(referenceTime), "20140129T034247.032000");
  BOOST_CHECK_EQUAL(time::toString(referenceTime), "2014-01-29 03:42:47");
  BOOST_CHECK_EQUAL(time::toString(referenceTime), "2014-01-29 03:42:47");

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(time::toString(referenceTime, "%Y. gada %d. %B",
  //                                  std::locale("lv_LV.UTF-8")),
  //                   "2014. gada 29. Janvāris");

  BOOST_CHECK_EQUAL(time::toString(referenceTime, "%Y -- %d -- %B",
                                   std::locale("C")),
                    "2014 -- 29 -- January");

  BOOST_CHECK_EQUAL(time::fromIsoString("20140129T034247.032000"), referenceTime);
  BOOST_CHECK_EQUAL(time::fromIsoString("20140129T034247.032000Z"), referenceTime);
  BOOST_CHECK_EQUAL(time::fromString("2014-01-29 03:42:47"),
                    time::fromUnixTimestamp(time::seconds(1390966967)));

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(time::fromString("2014. gada 29. Janvāris", "%Y. gada %d. %B",
  //                                    std::locale("lv_LV.UTF-8")),
  //                   time::fromUnixTimestamp(time::seconds(1390953600)));

  BOOST_CHECK_EQUAL(time::fromString("2014 -- 29 -- January", "%Y -- %d -- %B",
                                     std::locale("C")),
                    time::fromUnixTimestamp(time::seconds(1390953600)));
}

BOOST_AUTO_TEST_CASE(SteadyClock)
{
  time::steady_clock::TimePoint oldValue = time::steady_clock::now();
  usleep(100);
  time::steady_clock::TimePoint newValue = time::steady_clock::now();

  BOOST_CHECK_GT(newValue, oldValue);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
