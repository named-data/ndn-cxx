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

#include "util/scheduler.hpp"

#include "boost-test.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(UtilTestScheduler)

struct SchedulerFixture
{
  SchedulerFixture()
    : count1(0)
    , count2(0)
    , count3(0)
    , count4(0)
  {
  }

  void
  event1()
  {
    BOOST_CHECK_EQUAL(count3, 1);
    ++count1;
  }

  void
  event2()
  {
    ++count2;
  }

  void
  event3()
  {
    BOOST_CHECK_EQUAL(count1, 0);
    ++count3;
  }

  void
  event4()
  {
    ++count4;
  }

  int count1;
  int count2;
  int count3;
  int count4;
};

BOOST_FIXTURE_TEST_CASE(Events, SchedulerFixture)
{
  boost::asio::io_service io;

  Scheduler scheduler(io);
  scheduler.scheduleEvent(time::milliseconds(500), bind(&SchedulerFixture::event1, this));

  EventId i = scheduler.scheduleEvent(time::seconds(1), bind(&SchedulerFixture::event2, this));
  scheduler.cancelEvent(i);

  scheduler.scheduleEvent(time::milliseconds(250), bind(&SchedulerFixture::event3, this));

  i = scheduler.scheduleEvent(time::milliseconds(50), bind(&SchedulerFixture::event2, this));
  scheduler.cancelEvent(i);

  i = scheduler.schedulePeriodicEvent(time::milliseconds(1500), time::milliseconds(500), bind(&SchedulerFixture::event4, this));
  scheduler.scheduleEvent(time::seconds(4), bind(&Scheduler::cancelEvent, &scheduler, i));

  io.run();

  BOOST_CHECK_EQUAL(count1, 1);
  BOOST_CHECK_EQUAL(count2, 0);
  BOOST_CHECK_EQUAL(count3, 1);
  BOOST_CHECK_GE(count4, 2);
}

BOOST_AUTO_TEST_CASE(CancelEmptyEvent)
{
  boost::asio::io_service io;
  Scheduler scheduler(io);

  EventId i;
  scheduler.cancelEvent(i);
}

struct SelfCancelFixture
{
  SelfCancelFixture()
    : m_scheduler(m_io)
  {
  }

  void
  cancelSelf()
  {
    m_scheduler.cancelEvent(m_selfEventId);
  }

  boost::asio::io_service m_io;
  Scheduler m_scheduler;
  EventId m_selfEventId;
};

BOOST_FIXTURE_TEST_CASE(SelfCancel, SelfCancelFixture)
{
  m_selfEventId = m_scheduler.scheduleEvent(time::milliseconds(100),
                                            bind(&SelfCancelFixture::cancelSelf, this));

  BOOST_REQUIRE_NO_THROW(m_io.run());
}

struct SelfRescheduleFixture
{
  SelfRescheduleFixture()
    : m_scheduler(m_io)
    , m_count(0)
  {
  }

  void
  reschedule()
  {
    EventId eventId = m_scheduler.scheduleEvent(time::milliseconds(100),
                                                bind(&SelfRescheduleFixture::reschedule, this));
    m_scheduler.cancelEvent(m_selfEventId);
    m_selfEventId = eventId;

    if(m_count < 5)
      m_count++;
    else
      m_scheduler.cancelEvent(m_selfEventId);
  }

  void
  reschedule2()
  {
    m_scheduler.cancelEvent(m_selfEventId);


    if(m_count < 5)
      {
        m_selfEventId = m_scheduler.scheduleEvent(time::milliseconds(100),
                                                  bind(&SelfRescheduleFixture::reschedule2, this));
        m_count++;
      }
  }

  void
  doNothing()
  {
    m_count++;
  }

  void
  reschedule3()
  {
    m_scheduler.cancelEvent(m_selfEventId);

    m_scheduler.scheduleEvent(time::milliseconds(100),
                              bind(&SelfRescheduleFixture::doNothing, this));
    m_scheduler.scheduleEvent(time::milliseconds(100),
                              bind(&SelfRescheduleFixture::doNothing, this));
    m_scheduler.scheduleEvent(time::milliseconds(100),
                              bind(&SelfRescheduleFixture::doNothing, this));
    m_scheduler.scheduleEvent(time::milliseconds(100),
                              bind(&SelfRescheduleFixture::doNothing, this));
    m_scheduler.scheduleEvent(time::milliseconds(100),
                              bind(&SelfRescheduleFixture::doNothing, this));
    m_scheduler.scheduleEvent(time::milliseconds(100),
                              bind(&SelfRescheduleFixture::doNothing, this));
  }

  boost::asio::io_service m_io;
  Scheduler m_scheduler;
  EventId m_selfEventId;
  int m_count;

};

BOOST_FIXTURE_TEST_CASE(Reschedule, SelfRescheduleFixture)
{
  m_selfEventId = m_scheduler.scheduleEvent(time::seconds(0),
                                            bind(&SelfRescheduleFixture::reschedule, this));

  BOOST_REQUIRE_NO_THROW(m_io.run());

  BOOST_CHECK_EQUAL(m_count, 5);
}

BOOST_FIXTURE_TEST_CASE(Reschedule2, SelfRescheduleFixture)
{
  m_selfEventId = m_scheduler.scheduleEvent(time::seconds(0),
                                            bind(&SelfRescheduleFixture::reschedule2, this));

  BOOST_REQUIRE_NO_THROW(m_io.run());

  BOOST_CHECK_EQUAL(m_count, 5);
}

BOOST_FIXTURE_TEST_CASE(Reschedule3, SelfRescheduleFixture)
{
  m_selfEventId = m_scheduler.scheduleEvent(time::seconds(0),
                                            bind(&SelfRescheduleFixture::reschedule3, this));

  BOOST_REQUIRE_NO_THROW(m_io.run());

  BOOST_CHECK_EQUAL(m_count, 6);
}


BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
