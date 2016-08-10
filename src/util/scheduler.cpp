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

#include "scheduler.hpp"
#include <boost/scope_exit.hpp>

namespace ndn {
namespace util {
namespace scheduler {

class EventInfo : noncopyable
{
public:
  EventInfo(time::nanoseconds after, const EventCallback& callback)
    : expireTime(time::steady_clock::now() + after)
    , isExpired(false)
    , callback(callback)
  {
  }

  time::nanoseconds
  expiresFromNow() const
  {
    return std::max(expireTime - time::steady_clock::now(), time::nanoseconds::zero());
  }

public:
  time::steady_clock::TimePoint expireTime;
  bool isExpired;
  EventCallback callback;
  EventQueue::const_iterator queueIt;
};

bool
EventId::operator!() const
{
  return m_info.expired() || m_info.lock()->isExpired;
}

bool
EventId::operator==(const EventId& other) const
{
  return (!(*this) && !other) ||
         !(m_info.owner_before(other.m_info) || other.m_info.owner_before(m_info));
}

std::ostream&
operator<<(std::ostream& os, const EventId& eventId)
{
  return os << eventId.m_info.lock();
}

bool
EventQueueCompare::operator()(const shared_ptr<EventInfo>& a, const shared_ptr<EventInfo>& b) const
{
  return a->expireTime < b->expireTime;
}

Scheduler::Scheduler(boost::asio::io_service& ioService)
  : m_deadlineTimer(ioService)
  , m_isEventExecuting(false)
{
}

EventId
Scheduler::scheduleEvent(const time::nanoseconds& after, const EventCallback& callback)
{
  BOOST_ASSERT(callback != nullptr);

  EventQueue::iterator i = m_queue.insert(make_shared<EventInfo>(after, callback));
  (*i)->queueIt = i;

  if (!m_isEventExecuting && i == m_queue.begin()) {
    // the new event is the first one to expire
    this->scheduleNext();
  }

  return EventId(*i);
}

void
Scheduler::cancelEvent(const EventId& eventId)
{
  shared_ptr<EventInfo> info = eventId.m_info.lock();
  if (info == nullptr || info->isExpired) {
    return; // event already expired or cancelled
  }

  if (info->queueIt == m_queue.begin()) {
    m_deadlineTimer.cancel();
  }
  m_queue.erase(info->queueIt);

  if (!m_isEventExecuting) {
    this->scheduleNext();
  }
}

void
Scheduler::cancelAllEvents()
{
  m_queue.clear();
  m_deadlineTimer.cancel();
}

void
Scheduler::scheduleNext()
{
  if (!m_queue.empty()) {
    m_deadlineTimer.expires_from_now((*m_queue.begin())->expiresFromNow());
    m_deadlineTimer.async_wait(bind(&Scheduler::executeEvent, this, _1));
  }
}

void
Scheduler::executeEvent(const boost::system::error_code& error)
{
  if (error) { // e.g., cancelled
    return;
  }

  m_isEventExecuting = true;

  BOOST_SCOPE_EXIT_ALL(this) {
    m_isEventExecuting = false;
    this->scheduleNext();
  };

  // process all expired events
  time::steady_clock::TimePoint now = time::steady_clock::now();
  while (!m_queue.empty()) {
    EventQueue::iterator head = m_queue.begin();
    shared_ptr<EventInfo> info = *head;
    if (info->expireTime > now) {
      break;
    }

    m_queue.erase(head);
    info->isExpired = true;
    info->callback();
  }
}

} // namespace scheduler
} // namespace util
} // namespace ndn
