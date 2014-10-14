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

#ifndef NDN_UTIL_IN_MEMORY_STORAGE_LFU_HPP
#define NDN_UTIL_IN_MEMORY_STORAGE_LFU_HPP

#include "in-memory-storage.hpp"

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace ndn {
namespace util {

/** @brief Provides an in-memory storage with Least Frequently Used (LFU) replacement policy.
 *  @note The frequency right now is usage count.
 *  @sa https://en.wikipedia.org/w/index.php?title=Least_frequently_used&oldid=604542656
 */
class InMemoryStorageLfu : public InMemoryStorage
{
public:
  explicit
  InMemoryStorageLfu(size_t limit = 10);

  virtual
  ~InMemoryStorageLfu();

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  /** @brief Removes one Data packet from in-memory storage based on LFU, i.e. evict the least
   *  frequently accessed Data packet
   *  @return{ whether the Data was removed }
   */
  virtual bool
  evictItem();

  /** @brief Update the entry when the entry is returned by the find() function,
   *  increment the frequency according to LFU
   */
  virtual void
  afterAccess(InMemoryStorageEntry* entry);

  /** @brief Update the entry after a entry is successfully inserted, add it to the cleanupIndex
   */
  virtual void
  afterInsert(InMemoryStorageEntry* entry);

  /** @brief Update the entry or other data structures before a entry is successfully erased,
   *  erase it from the cleanupIndex
   */
  virtual void
  beforeErase(InMemoryStorageEntry* entry);

private:
  //binds frequency and entry together
  struct CleanupEntry
  {
    InMemoryStorageEntry* entry;
    uint64_t frequency;//could potentially be overflowed
  };

  /** @brief Function to increment frequency of the entry in the CleanupEntry
   */
  static inline void
  incrementFrequency(CleanupEntry& cleanupEntry)
  {
    ++cleanupEntry.frequency;
  }

private:
  //multi_index_container to implement LFU
  class byFrequency;
  class byEntity;

  typedef boost::multi_index_container<
    CleanupEntry,
    boost::multi_index::indexed_by<

      // by Entry itself
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<byEntity>,
        boost::multi_index::member<CleanupEntry, InMemoryStorageEntry*, &CleanupEntry::entry>
      >,

      // by frequency (LFU)
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<byFrequency>,
        boost::multi_index::member<CleanupEntry, uint64_t, &CleanupEntry::frequency>,
        std::less<uint64_t>
      >

    >
  > CleanupIndex;

  CleanupIndex m_cleanupIndex;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_IN_MEMORY_STORAGE_LFU_HPP
