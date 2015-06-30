/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "scheduler-scoped-event-id.hpp"

namespace ndn {
namespace util {
namespace scheduler {

#if NDN_CXX_HAVE_IS_NOTHROW_MOVE_CONSTRUCTIBLE
static_assert(std::is_nothrow_move_constructible<ScopedEventId>::value,
              "ScopedEventId must be MoveConstructible with noexcept");
#endif // NDN_CXX_HAVE_IS_NOTHROW_MOVE_CONSTRUCTIBLE

ScopedEventId::ScopedEventId(Scheduler& scheduler)
  : m_scheduler(&scheduler)
{
}

ScopedEventId::ScopedEventId(ScopedEventId&& other) noexcept
  : m_scheduler(other.m_scheduler)
  , m_event(other.m_event)
{
  other.release();
}

ScopedEventId&
ScopedEventId::operator=(const EventId& event)
{
  if (m_event != event) {
    m_scheduler->cancelEvent(m_event);
    m_event = event;
  }
  return *this;
}

ScopedEventId::~ScopedEventId()
{
  m_scheduler->cancelEvent(m_event);
}

void
ScopedEventId::cancel()
{
  m_scheduler->cancelEvent(m_event);
}

void
ScopedEventId::release()
{
  m_event.reset();
}

} // namespace scheduler
} // namespace util
} // namespace ndn
