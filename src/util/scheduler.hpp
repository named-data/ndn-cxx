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

#ifndef NDN_UTIL_SCHEDULER_HPP
#define NDN_UTIL_SCHEDULER_HPP

#include "../common.hpp"
#include "monotonic_deadline_timer.hpp"

#include <set>

namespace ndn {
namespace util {
namespace scheduler {

/**
 * \brief Function to be invoked when a scheduled event expires
 */
typedef function<void()> EventCallback;

/**
 * \brief Stores internal information about a scheduled event
 */
class EventInfo;

/**
 * \brief Identifies a scheduled event
 */
class EventId
{
public:
  /**
   * \brief Constructs an empty EventId
   * \note EventId is implicitly convertible from nullptr.
   */
  EventId(std::nullptr_t = nullptr)
  {
  }

  /**
   * \retval true The event is valid.
   * \retval false This EventId is empty, or the event is expired or cancelled.
   */
  explicit
  operator bool() const
  {
    return !this->operator!();
  }

  /**
   * \retval true This EventId is empty, or the event is expired or cancelled.
   * \retval false The event is valid.
   */
  bool
  operator!() const;

  /**
   * \return whether this and other refer to the same event, or are both empty/expired/cancelled
   */
  bool
  operator==(const EventId& other) const;

  bool
  operator!=(const EventId& other) const
  {
    return !this->operator==(other);
  }

  /**
   * \brief clear this EventId
   * \note This does not cancel the event.
   * \post !(*this)
   */
  void
  reset()
  {
    m_info.reset();
  }

private:
  explicit
  EventId(const weak_ptr<EventInfo>& info)
    : m_info(info)
  {
  }

private:
  weak_ptr<EventInfo> m_info;

  friend class Scheduler;
  friend std::ostream& operator<<(std::ostream& os, const EventId& eventId);
};

std::ostream&
operator<<(std::ostream& os, const EventId& eventId);

class EventQueueCompare
{
public:
  bool
  operator()(const shared_ptr<EventInfo>& a, const shared_ptr<EventInfo>& b) const;
};

typedef std::multiset<shared_ptr<EventInfo>, EventQueueCompare> EventQueue;

/**
 * \brief Generic scheduler
 */
class Scheduler : noncopyable
{
public:
  /**
   * \deprecated use EventCallback
   */
  typedef EventCallback Event;

  explicit
  Scheduler(boost::asio::io_service& ioService);

  /**
   * \brief Schedule a one-time event after the specified delay
   * \return EventId that can be used to cancel the scheduled event
   */
  EventId
  scheduleEvent(const time::nanoseconds& after, const EventCallback& callback);

  /**
   * \brief Cancel a scheduled event
   */
  void
  cancelEvent(const EventId& eventId);

  /**
   * \brief Cancel all scheduled events
   */
  void
  cancelAllEvents();

private:
  /**
   * \brief Schedule the next event on the deadline timer
   */
  void
  scheduleNext();

  /**
   * \brief Execute expired events
   * \note If an event callback throws, the exception is propagated to the thread running the
   *       io_service. In case there are other expired events, they will be processed in the next
   *       invocation of this method.
   */
  void
  executeEvent(const boost::system::error_code& code);

private:
  monotonic_deadline_timer m_deadlineTimer;
  EventQueue m_queue;
  bool m_isEventExecuting;
};

} // namespace scheduler

using util::scheduler::Scheduler;

} // namespace util

// for backwards compatibility
using util::scheduler::Scheduler;
using util::scheduler::EventId;

} // namespace ndn

#endif // NDN_UTIL_SCHEDULER_HPP
