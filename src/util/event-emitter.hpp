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

#ifndef NDN_UTIL_EVENT_EMITTER_HPP
#define NDN_UTIL_EVENT_EMITTER_HPP

#include "../common.hpp"
#include <vector>

namespace ndn {
namespace util {

/** \brief provides a lightweight event system
 *
 *  To declare an event:
 *    EventEmitter<TArgs> onEventName;
 *  To subscribe to an event:
 *    eventSource->onEventName += eventHandler;
 *    Multiple functions can subscribe to the same event.
 *  To trigger an event:
 *    onEventName(args);
 *  To clear event subscriptions:
 *    onEventName.clear();
 *
 *  \deprecated use Signal instead
 */

template<typename ...TArgs>
class EventEmitter : noncopyable
{
public:
  /** \brief represents a handler that can subscribe to the event
   */
  typedef function<void(const TArgs&...)> Handler;

  /** \brief subscribes to the event
   */
  void
  operator+=(const Handler& handler);

  /** \return true if there is no subscription, false otherwise
   */
  bool
  isEmpty() const;

  /** \brief clears all subscriptions
   */
  void
  clear();

  /** \brief triggers the event
   */
  void
  operator()(const TArgs&...args) const;

private:
  std::vector<Handler> m_handlers;
};

template<typename ...TArgs>
inline void
EventEmitter<TArgs...>::operator+=(const Handler& handler)
{
  m_handlers.push_back(handler);
}

template<typename ...TArgs>
inline bool
EventEmitter<TArgs...>::isEmpty() const
{
  return m_handlers.empty();
}

template<typename ...TArgs>
inline void
EventEmitter<TArgs...>::clear()
{
  return m_handlers.clear();
}

template<typename ...TArgs>
inline void
EventEmitter<TArgs...>::operator()(const TArgs&...args) const
{
  for (const Handler& handler : m_handlers) {
    handler(args...);
    if (m_handlers.empty()) // .clear has been called
      return;
  }
}


} // namespace util
} // namespace ndn

#endif // NDN_UTIL_EVENT_EMITTER_HPP
