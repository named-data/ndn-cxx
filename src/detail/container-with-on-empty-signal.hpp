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

#ifndef NDN_DETAIL_CONTAINER_WITH_ON_EMPTY_SIGNAL_HPP
#define NDN_DETAIL_CONTAINER_WITH_ON_EMPTY_SIGNAL_HPP

#include "../common.hpp"
#include "../util/signal.hpp"

namespace ndn {

/**
 * @brief A simple container that will fire up onEmpty signal when there are no entries left
 */
template<class T>
class ContainerWithOnEmptySignal
{
public:
  typedef std::list<T> Base;
  typedef typename Base::value_type value_type;
  typedef typename Base::iterator iterator;

  iterator
  begin()
  {
    return m_container.begin();
  }

  iterator
  end()
  {
    return m_container.end();
  }

  size_t
  size()
  {
    return m_container.size();
  }

  bool
  empty()
  {
    return m_container.empty();
  }

  iterator
  erase(iterator item)
  {
    iterator next = m_container.erase(item);
    if (empty()) {
      this->onEmpty();
    }
    return next;
  }

  void
  clear()
  {
    m_container.clear();
    this->onEmpty();
  }

  std::pair<iterator, bool>
  insert(const value_type& value)
  {
    return {m_container.insert(end(), value), true};
  }

  template<class Predicate>
  void remove_if(Predicate p)
  {
    m_container.remove_if(p);
    if (empty()) {
      this->onEmpty();
    }
  }

public:
  Base m_container;

  /**
   * @brief Signal to be fired when container becomes empty
   */
  util::Signal<ContainerWithOnEmptySignal<T>> onEmpty;
};

} // namespace ndn

#endif // NDN_DETAIL_CONTAINER_WITH_ON_EMPTY_SIGNAL_HPP
