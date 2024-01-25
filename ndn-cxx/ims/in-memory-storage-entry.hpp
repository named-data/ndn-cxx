/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_IMS_IN_MEMORY_STORAGE_ENTRY_HPP
#define NDN_CXX_IMS_IN_MEMORY_STORAGE_ENTRY_HPP

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/util/scheduler.hpp"

namespace ndn {

/**
 * @brief Represents an in-memory storage entry.
 */
class InMemoryStorageEntry : noncopyable
{
public:
  /** @brief Returns the name of the Data packet stored in the in-memory storage entry
   */
  const Name&
  getName() const
  {
    return m_dataPacket->getName();
  }

  /** @brief Returns the full name (including implicit digest) of the Data packet stored
   *         in the in-memory storage entry
   */
  const Name&
  getFullName() const
  {
    return m_dataPacket->getFullName();
  }

  /** @brief Returns the Data packet stored in the in-memory storage entry
   */
  const Data&
  getData() const
  {
    return *m_dataPacket;
  }

  /** @brief Changes the content of in-memory storage entry
   *
   *  This method also allows data to satisfy Interest with MustBeFresh
   */
  void
  setData(const Data& data)
  {
    m_dataPacket = data.shared_from_this();
    m_isFresh = true;
  }

  /** @brief Schedule an event to mark this entry as non-fresh.
   */
  void
  scheduleMarkStale(Scheduler& sched, time::nanoseconds after);

  /**
   * @brief Check if the data can satisfy an Interest with MustBeFresh.
   */
  bool
  isFresh() const
  {
    return m_isFresh;
  }

  /**
   * @brief Releases reference counts on shared objects.
   */
  void
  release();

private:
  shared_ptr<const Data> m_dataPacket;
  scheduler::ScopedEventId m_markStaleEventId;
  bool m_isFresh = true;
};

} // namespace ndn

#endif // NDN_CXX_IMS_IN_MEMORY_STORAGE_ENTRY_HPP
