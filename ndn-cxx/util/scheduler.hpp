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

#ifndef NDN_UTIL_SCHEDULER_HPP
#define NDN_UTIL_SCHEDULER_HPP

#include "ndn-cxx/detail/asio-fwd.hpp"
#include "ndn-cxx/detail/cancel-handle.hpp"
#include "ndn-cxx/util/time.hpp"

#include <boost/system/error_code.hpp>
#include <set>

namespace ndn {

namespace util {
namespace detail {
class SteadyTimer;
} // namespace detail
} // namespace util

namespace scheduler {

class Scheduler;
class EventInfo;

/** \brief Function to be invoked when a scheduled event expires
 */
using EventCallback = std::function<void()>;

/** \brief A handle for a scheduled event.
 *
 *  \code
 *  EventId eid = scheduler.schedule(10_ms, [] { doSomething(); });
 *  eid.cancel(); // cancel the event
 *  \endcode
 *
 *  \note Canceling an expired (executed) or canceled event has no effect.
 *  \warning Canceling an event after the scheduler has been destructed may trigger undefined
 *           behavior.
 */
class EventId : public detail::CancelHandle
{
public:
  /** \brief Constructs an empty EventId
   */
  EventId() noexcept = default;

  /** \brief Determine whether the event is valid.
   *  \retval true The event is valid.
   *  \retval false This EventId is empty, or the event is expired or cancelled.
   */
  explicit
  operator bool() const noexcept;

  /** \brief Determine whether this and other refer to the same event, or are both
   *         empty/expired/cancelled.
   */
  bool
  operator==(const EventId& other) const noexcept;

  bool
  operator!=(const EventId& other) const noexcept
  {
    return !this->operator==(other);
  }

  /** \brief Clear this EventId without canceling.
   *  \post !(*this)
   */
  void
  reset() noexcept;

private:
  EventId(Scheduler& sched, weak_ptr<EventInfo> info);

private:
  weak_ptr<EventInfo> m_info;

  friend class Scheduler;
  friend std::ostream& operator<<(std::ostream& os, const EventId& eventId);
};

std::ostream&
operator<<(std::ostream& os, const EventId& eventId);

/** \brief A scoped handle for a scheduled event.
 *
 *  Upon destruction of this handle, the event is canceled automatically.
 *  Most commonly, the application keeps a ScopedEventId as a class member field, so that it can
 *  cleanup its event when the class instance is destructed.
 *
 *  \code
 *  {
 *    ScopedEventId eid = scheduler.schedule(10_ms, [] { doSomething(); });
 *  } // eid goes out of scope, canceling the event
 *  \endcode
 *
 *  \note Canceling an expired (executed) or canceled event has no effect.
 *  \warning Canceling an event after the scheduler has been destructed may trigger undefined
 *           behavior.
 */
class ScopedEventId : public detail::ScopedCancelHandle
{
public:
  using ScopedCancelHandle::ScopedCancelHandle;
};

/** \brief Generic time-based scheduler
 */
class Scheduler : noncopyable
{
public:
  explicit
  Scheduler(boost::asio::io_service& ioService);

  ~Scheduler();

  /** \brief Schedule a one-time event after the specified delay
   *  \return EventId that can be used to cancel the scheduled event
   */
  EventId
  schedule(time::nanoseconds after, EventCallback callback);

  /** \brief Cancel all scheduled events
   */
  void
  cancelAllEvents();

private:
  void
  cancelImpl(const shared_ptr<EventInfo>& info);

  /** \brief Schedule the next event on the internal timer
   */
  void
  scheduleNext();

  /** \brief Execute expired events
   *
   *  If an event callback throws, the exception is propagated to the thread running the io_service.
   *  In case there are other expired events, they will be processed in the next invocation.
   */
  void
  executeEvent(const boost::system::error_code& code);

private:
  class EventQueueCompare
  {
  public:
    bool
    operator()(const shared_ptr<EventInfo>& a, const shared_ptr<EventInfo>& b) const noexcept;
  };

  using EventQueue = std::multiset<shared_ptr<EventInfo>, EventQueueCompare>;
  EventQueue m_queue;

  unique_ptr<util::detail::SteadyTimer> m_timer;
  bool m_isEventExecuting = false;

  friend EventId;
  friend EventInfo;
};

} // namespace scheduler

using scheduler::Scheduler;

} // namespace ndn

#endif // NDN_UTIL_SCHEDULER_HPP
