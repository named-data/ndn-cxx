/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "in-memory-storage-entry.hpp"

namespace ndn {

InMemoryStorageEntry::InMemoryStorageEntry()
  : m_isFresh(true)
{
}

void
InMemoryStorageEntry::release()
{
  m_dataPacket.reset();
  m_markStaleEventId.reset();
}

void
InMemoryStorageEntry::setData(const Data& data)
{
  m_dataPacket = data.shared_from_this();
  m_isFresh = true;
}

void
InMemoryStorageEntry::setMarkStaleEventId(unique_ptr<util::scheduler::ScopedEventId> markStaleEventId)
{
  m_markStaleEventId = std::move(markStaleEventId);
}

void
InMemoryStorageEntry::markStale()
{
  m_isFresh = false;
}

} // namespace ndn
