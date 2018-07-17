/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_UTIL_SCHEDULER_SCOPED_EVENT_ID_HPP
#define NDN_UTIL_SCHEDULER_SCOPED_EVENT_ID_HPP

#include "scheduler.hpp"

namespace ndn {
namespace util {
namespace scheduler {

/** \brief Event that is automatically cancelled upon destruction.
 */
class ScopedEventId
{
public:
  /** \brief Construct ScopedEventId tied to the specified scheduler.
   *  \param scheduler Scheduler to which the event is tied.  Behavior is undefined if
   *                   \p scheduler is destructed before an uncanceled ScopedEventId.
   */
  explicit
  ScopedEventId(Scheduler& scheduler) noexcept;

  ScopedEventId(const ScopedEventId&) = delete;

  ScopedEventId&
  operator=(const ScopedEventId&) = delete;

  /** \brief Move constructor.
   */
  ScopedEventId(ScopedEventId&&) noexcept;

  /** \brief Move assignment operator.
   */
  ScopedEventId&
  operator=(ScopedEventId&&) noexcept;

  /** \brief Assign an event.
   *
   *  If a different event has been assigned to this instance previously,
   *  that event will be cancelled immediately.
   *
   *  \note The caller should ensure that this ScopedEventId is tied to the correct Scheduler.
   *        Behavior is undefined when assigning an event scheduled in another Scheduler instance.
   */
  ScopedEventId&
  operator=(EventId event);

  /** \brief Destructor, automatically cancels the event.
   */
  ~ScopedEventId();

  /** \brief Manually cancel the event.
   */
  void
  cancel();

  /** \brief Release the event so that it won't be canceled when this ScopedEventId is destructed.
   */
  void
  release() noexcept;

private:
  Scheduler* m_scheduler; // pointer to allow move semantics
  EventId m_event;
};

inline
ScopedEventId::ScopedEventId(ScopedEventId&&) noexcept = default;

inline ScopedEventId&
ScopedEventId::operator=(ScopedEventId&&) noexcept = default;

} // namespace scheduler
} // namespace util
} // namespace ndn

#endif // NDN_UTIL_SCHEDULER_SCOPED_EVENT_ID_HPP
