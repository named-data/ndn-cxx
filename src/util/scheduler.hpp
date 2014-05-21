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

#ifndef NDN_UTIL_SCHEDULER_HPP
#define NDN_UTIL_SCHEDULER_HPP

#include "../common.hpp"
#include "monotonic_deadline_timer.hpp"

#include <set>

namespace ndn {

struct EventIdImpl; ///< \brief Private storage of information about the event
/**
 * \brief Opaque type (shared_ptr) representing ID of the scheduled event
 */
typedef shared_ptr<EventIdImpl> EventId;

/**
 * \brief Generic scheduler
 */
class Scheduler
{
public:
  typedef function<void()> Event;

  Scheduler(boost::asio::io_service& ioService);

  /**
   * \brief Schedule one time event after the specified delay
   * \returns EventId that can be used to cancel the scheduled event
   */
  EventId
  scheduleEvent(const time::nanoseconds& after, const Event& event);

  /**
   * \brief Schedule periodic event that should be fired every specified period.
   *        First event will be fired after the specified delay.
   * \returns EventId that can be used to cancel the scheduled event
   */
  EventId
  schedulePeriodicEvent(const time::nanoseconds& after,
                        const time::nanoseconds& period,
                        const Event& event);

  /**
   * \brief Cancel scheduled event
   */
  void
  cancelEvent(const EventId& eventId);

private:
  void
  onEvent(const boost::system::error_code& code);

private:
  struct EventInfo
  {
    EventInfo(const time::nanoseconds& after,
              const time::nanoseconds& period,
              const Event& event);

    EventInfo(const time::steady_clock::TimePoint& when, const EventInfo& previousEvent);

    bool
    operator <=(const EventInfo& other) const
    {
      return this->m_scheduledTime <= other.m_scheduledTime;
    }

    bool
    operator <(const EventInfo& other) const
    {
      return this->m_scheduledTime < other.m_scheduledTime;
    }

    time::nanoseconds
    expiresFromNow() const;

    time::steady_clock::TimePoint m_scheduledTime;
    time::nanoseconds m_period;
    Event m_event;
    mutable EventId m_eventId;
  };

  typedef std::multiset<EventInfo> EventQueue;
  friend struct EventIdImpl;

  EventQueue m_events;
  EventQueue::iterator m_scheduledEvent;
  monotonic_deadline_timer m_deadlineTimer;

  bool m_isEventExecuting;
};

} // namespace ndn

#endif // NDN_UTIL_SCHEDULER_HPP
