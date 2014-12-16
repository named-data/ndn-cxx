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

#ifndef NDN_UTIL_IN_MEMORY_STORAGE_HPP
#define NDN_UTIL_IN_MEMORY_STORAGE_HPP

#include "../common.hpp"
#include "../interest.hpp"
#include "../data.hpp"

#include "in-memory-storage-entry.hpp"

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <stack>
#include <iterator>

namespace ndn {
namespace util {

/** @brief Represents in-memory storage
 */
class InMemoryStorage : noncopyable
{
public:
  //multi_index_container to implement storage
  class byFullName;

  typedef boost::multi_index_container<
    InMemoryStorageEntry*,
    boost::multi_index::indexed_by<

      // by Full Name
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<byFullName>,
        boost::multi_index::const_mem_fun<InMemoryStorageEntry, const Name&,
                                          &InMemoryStorageEntry::getFullName>,
        std::less<Name>
      >

    >
  > Cache;

  /** @brief Represents a self-defined const_iterator for the in-memory storage
   *
   *  @note Don't try to instantiate this class directly, use InMemoryStorage::begin() instead.
   */
  class const_iterator : public std::iterator<std::input_iterator_tag, const Data>
  {
  public:
    const_iterator(const Data* ptr, const Cache* cache,
                   Cache::index<byFullName>::type::iterator it);

    const_iterator&
    operator++();

    const_iterator
    operator++(int);

    const Data&
    operator*();

    const Data*
    operator->();

    bool
    operator==(const const_iterator& rhs);

    bool
    operator!=(const const_iterator& rhs);

  private:
    const Data* m_ptr;
    const Cache* m_cache;
    Cache::index<byFullName>::type::iterator m_it;
  };

  /** @brief Represents an error might be thrown during reduce the current capacity of the
   *  in-memory storage through function setCapacity(size_t nMaxPackets).
   */
  class Error : public std::runtime_error
  {
  public:
    Error() : std::runtime_error("Cannot reduce the capacity of the in-memory storage!")
    {
    }
  };

  explicit
  InMemoryStorage(size_t limit = std::numeric_limits<size_t>::max());

  /** @note Please make sure to implement it to free m_freeEntries and evict
    * all items in the derived class for anybody who wishes to inherit this class
    */
  virtual
  ~InMemoryStorage();

  /** @brief Inserts a Data packet
   *
   *  @note Packets are considered duplicate if the name with implicit digest matches.
   *  The new Data packet with the identical name, but a different payload
   *  will be placed in the in-memory storage.
   *
   *  @note It will invoke afterInsert(shared_ptr<InMemoryStorageEntry>).
   */
  void
  insert(const Data& data);

  /** @brief Finds the best match Data for an Interest
   *
   *  @note It will invoke afterAccess(shared_ptr<InMemoryStorageEntry>).
   *  As currently it is impossible to determine whether a Name contains implicit digest or not,
   *  therefore this find function is not able to locate a packet according to an interest(
   *  including implicit digest) whose name is not the full name of the data matching the
   *  implicit digest.
   *
   *  @return{ the best match, if any; otherwise a null shared_ptr }
   */
  shared_ptr<const Data>
  find(const Interest& interest);

  /** @brief Finds the best match Data for a Name with or without
   *  the implicit digest.
   *
   *  If packets with the same name but different digests exist
   *  and the Name supplied is the one without implicit digest, a packet
   *  will be arbitrarily chosen to return.
   *
   *  @note It will invoke afterAccess(shared_ptr<InMemoryStorageEntry>).
   *
   *  @return{ the one matched the Name; otherwise a null shared_ptr }
   */
  shared_ptr<const Data>
  find(const Name& name);

  /** @brief Deletes in-memory storage entry by prefix by default.
   *  @param[in] isPrefix If it is clear, the function will only delete the
   *  entry completely matched with the prefix according to canonical ordering.
   *  For this case, user should substitute the prefix with full name.
   *
   *  @note Please do not use this function directly in any derived class to erase
   *  entry in the cache, use eraseHelper instead.
   *  @note It will invoke beforeErase(shared_ptr<InMemoryStorageEntry>).
   */
  void
  erase(const Name& prefix, const bool isPrefix = true);

  /** @return{ maximum number of packets that can be allowed to store in in-memory storage }
   */
  size_t
  getLimit() const
  {
    return m_limit;
  }

  /** @return{ number of packets stored in in-memory storage }
   */
  size_t
  size() const
  {
    return m_nPackets;
  }

  /** @brief Returns begin iterator of the in-memory storage ordering by
   *  name with digest
   *
   *  @return{ const_iterator pointing to the beginning of the m_cache }
   */
  InMemoryStorage::const_iterator
  begin() const;

  /** @brief Returns end iterator of the in-memory storage ordering by
   *  name with digest
   *
   *  @return{ const_iterator pointing to the end of the m_cache }
   */
  InMemoryStorage::const_iterator
  end() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  /** @brief Update the entry when the entry is returned by the find() function
   *  according to derived class implemented replacement policy
   */
  virtual void
  afterAccess(InMemoryStorageEntry* entry);

  /** @brief Update the entry or other data structures
   *  after a entry is successfully inserted
   *  according to derived class implemented replacement policy
   */
  virtual void
  afterInsert(InMemoryStorageEntry* entry);

  /** @brief Update the entry or other data structures
   *  before a entry is successfully erased
   *  according to derived class implemented replacement policy
   */
  virtual void
  beforeErase(InMemoryStorageEntry* entry);

  /** @brief Removes one Data packet from in-memory storage based on
   *  derived class implemented replacement policy
   *
   *  Please do not use this function directly in any derived class to erase
   *  entry in the cache, use eraseHelper instead.
   *  @return{ whether the Data was removed }
   */
  virtual bool
  evictItem() = 0;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  /** @brief sets current capacity of in-memory storage (in packets)
   */
  void
  setCapacity(size_t nMaxPackets);

  /** @brief returns current capacity of in-memory storage (in packets)
   *  @return{ number of packets that can be stored in application cache }
   */
  size_t
  getCapacity() const
  {
    return m_capacity;
  }

  /** @brief returns true if the in-memory storage uses up the current capacity, false otherwise
   */
  bool
  isFull() const
  {
    return size() >= m_capacity;
  }

  /** @brief deletes in-memory storage entries by the Name with implicit digest.
   *
   *  This is the function one should use to erase entry in the cache
   *  in derived class.
   *  It won't invoke beforeErase(shared_ptr<Entry>).
   */
  void
  eraseImpl(const Name& name);

  /** @brief Prints contents of the in-memory storage
   */
  void
  printCache(std::ostream& os) const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /** @brief free in-memory storage entries by an iterator pointing to that entry.
      @return An iterator pointing to the element that followed the last element erased.
   */
  Cache::iterator
  freeEntry(Cache::iterator it);

  /** @brief Implements child selector (leftmost, rightmost, undeclared).
   *  Operates on the first layer of a skip list.
   *
   *  startingPoint must be less than Interest Name.
   *  startingPoint can be equal to Interest Name only
   *  when the item is in the begin() position.
   *
   *  Iterates toward greater Names, terminates when application cache entry falls out of Interest
   *  prefix. When childSelector = leftmost, returns first application cache entry that satisfies
   *  other selectors. When childSelector = rightmost, it goes till the end, and returns application
   *  cache entry that satisfies other selectors. Returned application cache entry is the leftmost
   *  child of the rightmost child.
   *  @return{ the best match, if any; otherwise 0 }
   */
  InMemoryStorageEntry*
  selectChild(const Interest& interest,
              Cache::index<byFullName>::type::iterator startingPoint) const;

private:
  Cache m_cache;
  /// user defined maximum capacity of the in-memory storage in packets
  size_t m_limit;
  /// current capacity of the in-memory storage in packets
  size_t m_capacity;
  /// current number of packets in in-memory storage
  size_t m_nPackets;
  /// memory pool
  std::stack<InMemoryStorageEntry*> m_freeEntries;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_IN_MEMORY_STORAGE_HPP
