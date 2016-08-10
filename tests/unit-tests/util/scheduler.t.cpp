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

#include "util/scheduler.hpp"
#include "util/scheduler-scoped-event-id.hpp"

#include "boost-test.hpp"
#include "../unit-test-time-fixture.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace util {
namespace scheduler {
namespace tests {

using namespace ndn::tests;

class SchedulerFixture : public UnitTestTimeFixture
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

BOOST_AUTO_TEST_CASE(Events)
{
  size_t count1 = 0;
  size_t count2 = 0;

  scheduler.scheduleEvent(time::milliseconds(500), [&] {
      ++count1;
      BOOST_CHECK_EQUAL(count2, 1);
    });

  EventId i = scheduler.scheduleEvent(time::seconds(1), [&] {
      BOOST_ERROR("This event should not have been fired");
    });
  scheduler.cancelEvent(i);

  scheduler.scheduleEvent(time::milliseconds(250), [&] {
      BOOST_CHECK_EQUAL(count1, 0);
      ++count2;
    });

  i = scheduler.scheduleEvent(time::milliseconds(50), [&] {
      BOOST_ERROR("This event should not have been fired");
    });
  scheduler.cancelEvent(i);

  advanceClocks(time::milliseconds(25), time::milliseconds(1000));
  BOOST_CHECK_EQUAL(count1, 1);
  BOOST_CHECK_EQUAL(count2, 1);
}

BOOST_AUTO_TEST_CASE(CallbackException)
{
  class MyException : public std::exception
  {
  };
  scheduler.scheduleEvent(time::milliseconds(10), [] { BOOST_THROW_EXCEPTION(MyException()); });

  bool isCallbackInvoked = false;
  scheduler.scheduleEvent(time::milliseconds(20), [&isCallbackInvoked] { isCallbackInvoked = true; });

  BOOST_CHECK_THROW(this->advanceClocks(time::milliseconds(6), 2), MyException);
  this->advanceClocks(time::milliseconds(6), 2);
  BOOST_CHECK(isCallbackInvoked);
}

BOOST_AUTO_TEST_CASE(CancelEmptyEvent)
{
  EventId i;
  scheduler.cancelEvent(i);
}

BOOST_AUTO_TEST_CASE(SelfCancel)
{
  EventId selfEventId;
  selfEventId = scheduler.scheduleEvent(time::milliseconds(100), [&] {
      scheduler.cancelEvent(selfEventId);
    });

  BOOST_REQUIRE_NO_THROW(advanceClocks(time::milliseconds(100), 10));
}

class SelfRescheduleFixture : public SchedulerFixture
{
public:
  SelfRescheduleFixture()
    : count(0)
  {
  }

  void
  reschedule()
  {
    EventId eventId = scheduler.scheduleEvent(time::milliseconds(100),
                                              bind(&SelfRescheduleFixture::reschedule, this));
    scheduler.cancelEvent(selfEventId);
    selfEventId = eventId;

    if (count < 5)
      count++;
    else
      scheduler.cancelEvent(selfEventId);
  }

  void
  reschedule2()
  {
    scheduler.cancelEvent(selfEventId);

    if (count < 5)  {
      selfEventId = scheduler.scheduleEvent(time::milliseconds(100),
                                            bind(&SelfRescheduleFixture::reschedule2, this));
      count++;
    }
  }

  void
  reschedule3()
  {
    scheduler.cancelEvent(selfEventId);

    scheduler.scheduleEvent(time::milliseconds(100), [&] { ++count; });
    scheduler.scheduleEvent(time::milliseconds(100), [&] { ++count; });
    scheduler.scheduleEvent(time::milliseconds(100), [&] { ++count; });
    scheduler.scheduleEvent(time::milliseconds(100), [&] { ++count; });
    scheduler.scheduleEvent(time::milliseconds(100), [&] { ++count; });
    scheduler.scheduleEvent(time::milliseconds(100), [&] { ++count; });
  }

public:
  EventId selfEventId;
  size_t count;
};

BOOST_FIXTURE_TEST_CASE(Reschedule, SelfRescheduleFixture)
{
  selfEventId = scheduler.scheduleEvent(time::seconds(0),
                                        bind(&SelfRescheduleFixture::reschedule, this));

  BOOST_REQUIRE_NO_THROW(advanceClocks(time::milliseconds(50), time::milliseconds(1000)));

  BOOST_CHECK_EQUAL(count, 5);
}

BOOST_FIXTURE_TEST_CASE(Reschedule2, SelfRescheduleFixture)
{
  selfEventId = scheduler.scheduleEvent(time::seconds(0),
                                        bind(&SelfRescheduleFixture::reschedule2, this));

  BOOST_REQUIRE_NO_THROW(advanceClocks(time::milliseconds(50), time::milliseconds(1000)));

  BOOST_CHECK_EQUAL(count, 5);
}

BOOST_FIXTURE_TEST_CASE(Reschedule3, SelfRescheduleFixture)
{
  selfEventId = scheduler.scheduleEvent(time::seconds(0),
                                        bind(&SelfRescheduleFixture::reschedule3, this));

  BOOST_REQUIRE_NO_THROW(advanceClocks(time::milliseconds(50), time::milliseconds(1000)));

  BOOST_CHECK_EQUAL(count, 6);
}

class CancelAllFixture : public SchedulerFixture
{
public:
  CancelAllFixture()
    : count(0)
  {
  }

  void
  event()
  {
    ++count;

    scheduler.scheduleEvent(time::seconds(1), [&] { event(); });
  }

public:
  uint32_t count;
};

BOOST_FIXTURE_TEST_CASE(CancelAll, CancelAllFixture)
{
  scheduler.scheduleEvent(time::milliseconds(500), [&] { scheduler.cancelAllEvents(); });

  scheduler.scheduleEvent(time::seconds(1), [&] { event(); });

  scheduler.scheduleEvent(time::seconds(3), [] {
      BOOST_ERROR("This event should have been cancelled" );
    });

  advanceClocks(time::milliseconds(100), 100);

  BOOST_CHECK_EQUAL(count, 0);
}

BOOST_AUTO_TEST_CASE(CancelAllWithScopedEventId) // Bug 3691
{
  Scheduler sched(io);
  ScopedEventId eid(sched);
  eid = sched.scheduleEvent(time::milliseconds(10), []{});
  sched.cancelAllEvents();
  eid.cancel(); // should not crash
}

BOOST_AUTO_TEST_SUITE(EventId)

using scheduler::EventId;

BOOST_AUTO_TEST_CASE(ConstructEmpty)
{
  EventId eid;
  eid = nullptr;
}

BOOST_AUTO_TEST_CASE(Compare)
{
  EventId eid, eid2;
  BOOST_CHECK_EQUAL(eid == eid2, true);
  BOOST_CHECK_EQUAL(eid != eid2, false);
  BOOST_CHECK_EQUAL(eid == nullptr, true);
  BOOST_CHECK_EQUAL(eid != nullptr, false);

  eid = scheduler.scheduleEvent(time::milliseconds(10), []{});
  BOOST_CHECK_EQUAL(eid == eid2, false);
  BOOST_CHECK_EQUAL(eid != eid2, true);
  BOOST_CHECK_EQUAL(eid == nullptr, false);
  BOOST_CHECK_EQUAL(eid != nullptr, true);

  eid2 = eid;
  BOOST_CHECK_EQUAL(eid == eid2, true);
  BOOST_CHECK_EQUAL(eid != eid2, false);

  eid2 = scheduler.scheduleEvent(time::milliseconds(10), []{});
  BOOST_CHECK_EQUAL(eid == eid2, false);
  BOOST_CHECK_EQUAL(eid != eid2, true);
}

BOOST_AUTO_TEST_CASE(Valid)
{
  EventId eid;
  BOOST_CHECK_EQUAL(static_cast<bool>(eid), false);
  BOOST_CHECK_EQUAL(!eid, true);

  eid = scheduler.scheduleEvent(time::milliseconds(10), []{});
  BOOST_CHECK_EQUAL(static_cast<bool>(eid), true);
  BOOST_CHECK_EQUAL(!eid, false);

  EventId eid2 = eid;
  scheduler.cancelEvent(eid2);
  BOOST_CHECK_EQUAL(static_cast<bool>(eid), false);
  BOOST_CHECK_EQUAL(!eid, true);
  BOOST_CHECK_EQUAL(static_cast<bool>(eid2), false);
  BOOST_CHECK_EQUAL(!eid2, true);
}

BOOST_AUTO_TEST_CASE(DuringCallback)
{
  EventId eid;
  EventId eid2 = scheduler.scheduleEvent(time::milliseconds(20), []{});

  bool isCallbackInvoked = false;
  eid = scheduler.scheduleEvent(time::milliseconds(10), [this, &eid, &eid2, &isCallbackInvoked] {
    isCallbackInvoked = true;

    // eid is "expired" during callback execution
    BOOST_CHECK_EQUAL(static_cast<bool>(eid), false);
    BOOST_CHECK_EQUAL(!eid, true);
    BOOST_CHECK_EQUAL(eid == eid2, false);
    BOOST_CHECK_EQUAL(eid != eid2, true);
    BOOST_CHECK_EQUAL(eid == nullptr, true);
    BOOST_CHECK_EQUAL(eid != nullptr, false);

    scheduler.cancelEvent(eid2);
    BOOST_CHECK_EQUAL(eid == eid2, true);
    BOOST_CHECK_EQUAL(eid != eid2, false);
  });
  this->advanceClocks(time::milliseconds(6), 2);
  BOOST_CHECK(isCallbackInvoked);
}

BOOST_AUTO_TEST_CASE(Reset)
{
  bool isCallbackInvoked = false;
  EventId eid = scheduler.scheduleEvent(time::milliseconds(10),
                                        [&isCallbackInvoked]{ isCallbackInvoked = true; });
  eid.reset();
  BOOST_CHECK_EQUAL(!eid, true);
  BOOST_CHECK_EQUAL(eid == nullptr, true);

  this->advanceClocks(time::milliseconds(6), 2);
  BOOST_CHECK(isCallbackInvoked);
}

BOOST_AUTO_TEST_CASE(ToString)
{
  std::string nullString = boost::lexical_cast<std::string>(shared_ptr<int>());

  EventId eid;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(eid), nullString);

  eid = scheduler.scheduleEvent(time::milliseconds(10), []{});
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
    ScopedEventId se(scheduler);
    se = scheduler.scheduleEvent(time::milliseconds(10), [&] { ++hit; });
  } // se goes out of scope
  this->advanceClocks(time::milliseconds(1), 15);
  BOOST_CHECK_EQUAL(hit, 0);
}

BOOST_AUTO_TEST_CASE(Assign)
{
  int hit1 = 0, hit2 = 0;
  ScopedEventId se1(scheduler);
  se1 = scheduler.scheduleEvent(time::milliseconds(10), [&] { ++hit1; });
  se1 = scheduler.scheduleEvent(time::milliseconds(10), [&] { ++hit2; });
  this->advanceClocks(time::milliseconds(1), 15);
  BOOST_CHECK_EQUAL(hit1, 0);
  BOOST_CHECK_EQUAL(hit2, 1);
}

BOOST_AUTO_TEST_CASE(Release)
{
  int hit = 0;
  {
    ScopedEventId se(scheduler);
    se = scheduler.scheduleEvent(time::milliseconds(10), [&] { ++hit; });
    se.release();
  } // se goes out of scope
  this->advanceClocks(time::milliseconds(1), 15);
  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_CASE(Move)
{
  int hit = 0;
  unique_ptr<scheduler::ScopedEventId> se2;
  {
    ScopedEventId se(scheduler);
    se = scheduler.scheduleEvent(time::milliseconds(10), [&] { ++hit; });
    se2.reset(new ScopedEventId(std::move(se)));
  } // se goes out of scope
  this->advanceClocks(time::milliseconds(1), 15);
  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_SUITE_END() // ScopedEventId

BOOST_AUTO_TEST_SUITE_END() // TestScheduler
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace scheduler
} // namespace util
} // namespace ndn
