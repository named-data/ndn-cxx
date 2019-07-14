/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/util/scheduler.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/unit-test-time-fixture.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace scheduler {
namespace tests {

class SchedulerFixture : public ndn::tests::UnitTestTimeFixture
{
public:
  SchedulerFixture()
    : scheduler(io)
  {
  }

public:
  Scheduler scheduler;
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestScheduler, SchedulerFixture)

BOOST_AUTO_TEST_SUITE(General)

BOOST_AUTO_TEST_CASE(Events)
{
  size_t count1 = 0;
  size_t count2 = 0;

  scheduler.schedule(500_ms, [&] {
    ++count1;
    BOOST_CHECK_EQUAL(count2, 1);
  });

  EventId i = scheduler.schedule(1_s, [] { BOOST_ERROR("This event should not have been fired"); });
  i.cancel();

  scheduler.schedule(250_ms, [&] {
    BOOST_CHECK_EQUAL(count1, 0);
    ++count2;
  });

  i = scheduler.schedule(50_ms, [&] { BOOST_ERROR("This event should not have been fired"); });
  i.cancel();

  advanceClocks(25_ms, 1000_ms);
  BOOST_CHECK_EQUAL(count1, 1);
  BOOST_CHECK_EQUAL(count2, 1);
}

BOOST_AUTO_TEST_CASE(CallbackException)
{
  class MyException : public std::exception
  {
  };
  scheduler.schedule(10_ms, [] {
    // use plain 'throw' to ensure that Scheduler does not depend on the
    // internal machinery of NDN_THROW and that it can catch all exceptions
    // regardless of how they are thrown by the application
    throw MyException{};
  });

  bool isCallbackInvoked = false;
  scheduler.schedule(20_ms, [&isCallbackInvoked] { isCallbackInvoked = true; });

  BOOST_CHECK_THROW(this->advanceClocks(6_ms, 2), MyException);
  this->advanceClocks(6_ms, 2);
  BOOST_CHECK(isCallbackInvoked);
}

BOOST_AUTO_TEST_CASE(CancelEmptyEvent)
{
  EventId i;
  i.cancel();

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SelfCancel)
{
  EventId selfEventId;
  selfEventId = scheduler.schedule(100_ms, [&] { selfEventId.cancel(); });
  BOOST_REQUIRE_NO_THROW(advanceClocks(100_ms, 10));
}

class SelfRescheduleFixture : public SchedulerFixture
{
public:
  void
  reschedule()
  {
    EventId eventId = scheduler.schedule(100_ms, bind(&SelfRescheduleFixture::reschedule, this));
    selfEventId.cancel();
    selfEventId = eventId;

    if (count < 5)
      count++;
    else
      selfEventId.cancel();
  }

  void
  reschedule2()
  {
    selfEventId.cancel();

    if (count < 5)  {
      selfEventId = scheduler.schedule(100_ms, bind(&SelfRescheduleFixture::reschedule2, this));
      count++;
    }
  }

  void
  reschedule3()
  {
    selfEventId.cancel();

    scheduler.schedule(100_ms, [&] { ++count; });
    scheduler.schedule(100_ms, [&] { ++count; });
    scheduler.schedule(100_ms, [&] { ++count; });
    scheduler.schedule(100_ms, [&] { ++count; });
    scheduler.schedule(100_ms, [&] { ++count; });
    scheduler.schedule(100_ms, [&] { ++count; });
  }

public:
  EventId selfEventId;
  size_t count = 0;
};

BOOST_FIXTURE_TEST_CASE(Reschedule, SelfRescheduleFixture)
{
  selfEventId = scheduler.schedule(0_s, bind(&SelfRescheduleFixture::reschedule, this));
  BOOST_REQUIRE_NO_THROW(advanceClocks(50_ms, 1000_ms));
  BOOST_CHECK_EQUAL(count, 5);
}

BOOST_FIXTURE_TEST_CASE(Reschedule2, SelfRescheduleFixture)
{
  selfEventId = scheduler.schedule(0_s, bind(&SelfRescheduleFixture::reschedule2, this));
  BOOST_REQUIRE_NO_THROW(advanceClocks(50_ms, 1000_ms));
  BOOST_CHECK_EQUAL(count, 5);
}

BOOST_FIXTURE_TEST_CASE(Reschedule3, SelfRescheduleFixture)
{
  selfEventId = scheduler.schedule(0_s, bind(&SelfRescheduleFixture::reschedule3, this));
  BOOST_REQUIRE_NO_THROW(advanceClocks(50_ms, 1000_ms));
  BOOST_CHECK_EQUAL(count, 6);
}

class CancelAllFixture : public SchedulerFixture
{
public:
  void
  event()
  {
    ++count;
    scheduler.schedule(1_s, [&] { event(); });
  }

public:
  uint32_t count = 0;
};

BOOST_FIXTURE_TEST_CASE(CancelAll, CancelAllFixture)
{
  scheduler.schedule(500_ms, [&] { scheduler.cancelAllEvents(); });
  scheduler.schedule(1_s, [&] { event(); });
  scheduler.schedule(3_s, [] { BOOST_ERROR("This event should have been cancelled" ); });

  advanceClocks(100_ms, 100);
  BOOST_CHECK_EQUAL(count, 0);
}

BOOST_AUTO_TEST_CASE(CancelAllWithScopedEventId) // Bug 3691
{
  Scheduler sched(io);
  ScopedEventId eid = sched.schedule(10_ms, []{});
  sched.cancelAllEvents();
  eid.cancel(); // should not crash

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // General

BOOST_AUTO_TEST_SUITE(EventId)

using scheduler::EventId;

BOOST_AUTO_TEST_CASE(ConstructEmpty)
{
  EventId eid;
  BOOST_CHECK(!eid);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  EventId eid, eid2;
  BOOST_CHECK(eid == eid2);

  eid = scheduler.schedule(10_ms, []{});
  BOOST_CHECK(eid != eid2);

  eid2 = scheduler.schedule(10_ms, []{});
  BOOST_CHECK(eid != eid2);

  eid.cancel();
  BOOST_CHECK(eid != eid2);
  BOOST_CHECK(eid == EventId{});

  eid2.cancel();
  BOOST_CHECK(eid == eid2);

  eid = eid2 = scheduler.schedule(20_ms, []{});
  BOOST_CHECK(eid == eid2);
  BOOST_CHECK(eid != EventId{});

  eid.cancel();
  BOOST_CHECK(eid == eid2);
  BOOST_CHECK(eid == EventId{});
}

BOOST_AUTO_TEST_CASE(OperatorBool)
{
  EventId eid;
  BOOST_CHECK_EQUAL(static_cast<bool>(eid), false);
  BOOST_CHECK_EQUAL(!eid, true);

  eid = scheduler.schedule(10_ms, []{});
  BOOST_CHECK_EQUAL(static_cast<bool>(eid), true);
  BOOST_CHECK_EQUAL(!eid, false);

  EventId eid2 = eid;
  eid2.cancel();
  BOOST_CHECK(!eid);
  BOOST_CHECK(!eid2);
}

BOOST_AUTO_TEST_CASE(DuringCallback)
{
  EventId eid;
  EventId eid2 = scheduler.schedule(20_ms, []{});

  bool isCallbackInvoked = false;
  eid = scheduler.schedule(10_ms, [&eid, &eid2, &isCallbackInvoked] {
    isCallbackInvoked = true;

    // eid is "expired" during callback execution
    BOOST_CHECK(!eid);
    BOOST_CHECK_NE(eid, eid2);

    eid2.cancel();
    BOOST_CHECK_EQUAL(eid, eid2);
  });

  this->advanceClocks(6_ms, 2);
  BOOST_CHECK(isCallbackInvoked);
}

BOOST_AUTO_TEST_CASE(Reset)
{
  bool isCallbackInvoked = false;
  EventId eid = scheduler.schedule(10_ms, [&isCallbackInvoked] { isCallbackInvoked = true; });
  eid.reset();
  BOOST_CHECK(!eid);

  this->advanceClocks(6_ms, 2);
  BOOST_CHECK(isCallbackInvoked);
}

BOOST_AUTO_TEST_CASE(ToString)
{
  std::string nullString = boost::lexical_cast<std::string>(shared_ptr<int>());

  EventId eid;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(eid), nullString);

  eid = scheduler.schedule(10_ms, []{});
  BOOST_TEST_MESSAGE("eid=" << eid);
  BOOST_CHECK_NE(boost::lexical_cast<std::string>(eid), nullString);
}

BOOST_AUTO_TEST_SUITE_END() // EventId

BOOST_AUTO_TEST_SUITE(ScopedEventId)

using scheduler::ScopedEventId;

BOOST_AUTO_TEST_CASE(Destruct)
{
  int hit = 0;
  {
    ScopedEventId se = scheduler.schedule(10_ms, [&] { ++hit; });
  } // se goes out of scope
  this->advanceClocks(1_ms, 15);
  BOOST_CHECK_EQUAL(hit, 0);
}

BOOST_AUTO_TEST_CASE(Assign)
{
  int hit1 = 0, hit2 = 0;
  ScopedEventId se1 = scheduler.schedule(10_ms, [&] { ++hit1; });
  se1 = scheduler.schedule(10_ms, [&] { ++hit2; });
  this->advanceClocks(1_ms, 15);
  BOOST_CHECK_EQUAL(hit1, 0);
  BOOST_CHECK_EQUAL(hit2, 1);
}

BOOST_AUTO_TEST_CASE(Release)
{
  int hit = 0;
  {
    ScopedEventId se = scheduler.schedule(10_ms, [&] { ++hit; });
    se.release();
  } // se goes out of scope
  this->advanceClocks(1_ms, 15);
  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_CASE(Move)
{
  int hit = 0;
  unique_ptr<ScopedEventId> se2;
  {
    ScopedEventId se = scheduler.schedule(10_ms, [&] { ++hit; });
    se2 = make_unique<ScopedEventId>(std::move(se)); // move constructor
  } // se goes out of scope
  this->advanceClocks(1_ms, 15);
  BOOST_CHECK_EQUAL(hit, 1);

  ScopedEventId se3;
  {
    ScopedEventId se = scheduler.schedule(10_ms, [&] { ++hit; });
    se3 = std::move(se); // move assignment
  } // se goes out of scope
  this->advanceClocks(1_ms, 15);
  BOOST_CHECK_EQUAL(hit, 2);
}

BOOST_AUTO_TEST_CASE(OperatorBool)
{
  ScopedEventId se;
  BOOST_CHECK_EQUAL(static_cast<bool>(se), false);
  BOOST_CHECK_EQUAL(!se, true);

  se = scheduler.schedule(10_ms, []{});
  BOOST_CHECK_EQUAL(static_cast<bool>(se), true);
  BOOST_CHECK_EQUAL(!se, false);

  se.cancel();
  BOOST_CHECK(!se);

  se = scheduler.schedule(10_ms, []{});
  BOOST_CHECK(se);

  se.release();
  BOOST_CHECK(!se);
}

BOOST_AUTO_TEST_SUITE_END() // ScopedEventId

BOOST_AUTO_TEST_SUITE_END() // TestScheduler
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace scheduler
} // namespace ndn
