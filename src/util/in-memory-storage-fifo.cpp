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

#include "in-memory-storage-fifo.hpp"

namespace ndn {
namespace util {

InMemoryStorageFifo::InMemoryStorageFifo(size_t limit)
  : InMemoryStorage(limit)
{
}

InMemoryStorageFifo::~InMemoryStorageFifo()
{
}

void
InMemoryStorageFifo::afterInsert(InMemoryStorageEntry* entry)
{
  BOOST_ASSERT(m_cleanupIndex.size() <= size());
  m_cleanupIndex.insert(entry);
}

bool
InMemoryStorageFifo::evictItem()
{
  if (!m_cleanupIndex.get<byArrival>().empty()) {
    CleanupIndex::index<byArrival>::type::iterator it = m_cleanupIndex.get<byArrival>().begin();
    eraseImpl((*it)->getFullName());
    m_cleanupIndex.get<byArrival>().erase(it);
    return true;
  }

  return false;
}

void
InMemoryStorageFifo::beforeErase(InMemoryStorageEntry* entry)
{
  CleanupIndex::index<byEntity>::type::iterator it = m_cleanupIndex.get<byEntity>().find(entry);
  if (it != m_cleanupIndex.get<byEntity>().end())
    m_cleanupIndex.get<byEntity>().erase(it);
}

} // namespace util
} // namespace ndn
