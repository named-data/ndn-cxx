/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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
#include "ndn-cxx/util/impl/steady-timer.hpp"
#include "ndn-cxx/util/scope.hpp"

namespace ndn {
namespace scheduler {

/** \brief Stores internal information about a scheduled event
 */
class EventInfo : noncopyable
{
public:
  EventInfo(time::nanoseconds after, EventCallback&& cb)
    : callback(std::move(cb))
    , expireTime(time::steady_clock::now() + after)
  {
  }

  NDN_CXX_NODISCARD time::nanoseconds
  expiresFromNow() const
  {
    return std::max(expireTime - time::steady_clock::now(), 0_ns);
  }

public:
  EventCallback callback;
  Scheduler::EventQueue::const_iterator queueIt;
  time::steady_clock::TimePoint expireTime;
  bool isExpired = false;
};

EventId::EventId(Scheduler& sched, weak_ptr<EventInfo> info)
  : CancelHandle([&sched, info] { sched.cancelImpl(info.lock()); })
  , m_info(std::move(info))
{
}

EventId::operator bool() const noexcept
{
  auto sp = m_info.lock();
  return sp != nullptr && !sp->isExpired;
}

void
EventId::reset() noexcept
{
  *this = {};
}

std::ostream&
operator<<(std::ostream& os, const EventId& eventId)
{
  return os << eventId.m_info.lock();
}

bool
Scheduler::EventQueueCompare::operator()(const shared_ptr<EventInfo>& a,
                                         const shared_ptr<EventInfo>& b) const noexcept
{
  return a->expireTime < b->expireTime;
}

Scheduler::Scheduler(boost::asio::io_service& ioService)
  : m_timer(make_unique<util::detail::SteadyTimer>(ioService))
{
}

Scheduler::~Scheduler() = default;

EventId
Scheduler::schedule(time::nanoseconds after, EventCallback callback)
{
  BOOST_ASSERT(callback != nullptr);

  auto i = m_queue.insert(std::make_shared<EventInfo>(after, std::move(callback)));
  (*i)->queueIt = i;

  if (!m_isEventExecuting && i == m_queue.begin()) {
    // the new event is the first one to expire
    scheduleNext();
  }

  return EventId(*this, *i);
}

void
Scheduler::cancelImpl(const shared_ptr<EventInfo>& info)
{
  if (info == nullptr || info->isExpired) {
    return;
  }

  if (info->queueIt == m_queue.begin()) {
    m_timer->cancel();
  }
  m_queue.erase(info->queueIt);

  if (!m_isEventExecuting) {
    scheduleNext();
  }
}

void
Scheduler::cancelAllEvents()
{
  m_queue.clear();
  m_timer->cancel();
}

void
Scheduler::scheduleNext()
{
  if (!m_queue.empty()) {
    m_timer->expires_from_now((*m_queue.begin())->expiresFromNow());
    m_timer->async_wait([this] (const auto& error) { this->executeEvent(error); });
  }
}

void
Scheduler::executeEvent(const boost::system::error_code& error)
{
  if (error) { // e.g., cancelled
    return;
  }

  auto guard = make_scope_exit([this] {
    m_isEventExecuting = false;
    scheduleNext();
  });
  m_isEventExecuting = true;

  // process all expired events
  auto now = time::steady_clock::now();
  while (!m_queue.empty()) {
    auto head = m_queue.begin();
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
} // namespace ndn
