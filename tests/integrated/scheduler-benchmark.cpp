/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
#define BOOST_TEST_MODULE Scheduler Benchmark

#include "util/scheduler.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace util {
namespace scheduler {
namespace tests {

BOOST_AUTO_TEST_CASE(ScheduleCancel)
{
  boost::asio::io_service io;
  Scheduler sched(io);

  const int nEvents = 1000000;
  std::vector<EventId> eventIds(nEvents);

  time::steady_clock::TimePoint t1 = time::steady_clock::now();
  for (int i = 0; i < nEvents; ++i) {
    eventIds[i] = sched.scheduleEvent(time::seconds(1), []{});
  }
  time::steady_clock::TimePoint t2 = time::steady_clock::now();
  for (int i = 0; i < nEvents; ++i) {
    sched.cancelEvent(eventIds[i]);
  }
  time::steady_clock::TimePoint t3 = time::steady_clock::now();

  BOOST_TEST_MESSAGE("schedule " << nEvents << " events: " << (t2 - t1));
  BOOST_TEST_MESSAGE("cancel " << nEvents << " events: " << (t3 - t2));
}

BOOST_AUTO_TEST_CASE(Execute)
{
  boost::asio::io_service io;
  Scheduler sched(io);

  const int nEvents = 1000000;
  int nExpired = 0;

  // Events should expire at t1, but execution finishes at t2. The difference is the overhead.
  time::steady_clock::TimePoint t1 = time::steady_clock::now() + time::seconds(5);
  time::steady_clock::TimePoint t2;
  // +1ms ensures this extra event is executed last. In case the overhead is less than 1ms,
  // it will be reported as 1ms.
  sched.scheduleEvent(t1 - time::steady_clock::now() + time::milliseconds(1), [&] {
    t2 = time::steady_clock::now();
    BOOST_REQUIRE_EQUAL(nExpired, nEvents);
  });

  for (int i = 0; i < nEvents; ++i) {
    sched.scheduleEvent(t1 - time::steady_clock::now(), [&] { ++nExpired; });
  }

  io.run();

  BOOST_REQUIRE_EQUAL(nExpired, nEvents);
  BOOST_TEST_MESSAGE("execute " << nEvents << " events: " << (t2 - t1));
}

} // namespace tests
} // namespace scheduler
} // namespace util
} // namespace ndn
