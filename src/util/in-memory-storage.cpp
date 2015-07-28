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

#include "in-memory-storage.hpp"
#include "in-memory-storage-entry.hpp"

#include "crypto.hpp"

#include "../security/signature-sha256-with-rsa.hpp"

namespace ndn {
namespace util {

InMemoryStorage::const_iterator::const_iterator(const Data* ptr, const Cache* cache,
                                                Cache::index<byFullName>::type::iterator it)
  : m_ptr(ptr)
  , m_cache(cache)
  , m_it(it)
{
}

InMemoryStorage::const_iterator&
InMemoryStorage::const_iterator::operator++()
{
  m_it++;
  if (m_it != m_cache->get<byFullName>().end()) {
    m_ptr = &((*m_it)->getData());
  }
  else {
    m_ptr = 0;
  }

  return *this;
}

InMemoryStorage::const_iterator
InMemoryStorage::const_iterator::operator++(int)
{
  InMemoryStorage::const_iterator i(*this);
  this->operator++();
  return i;
}

const Data&
InMemoryStorage::const_iterator::operator*()
{
  return *m_ptr;
}

const Data*
InMemoryStorage::const_iterator::operator->()
{
  return m_ptr;
}

bool
InMemoryStorage::const_iterator::operator==(const const_iterator& rhs)
{
  return m_it == rhs.m_it;
}

bool
InMemoryStorage::const_iterator::operator!=(const const_iterator& rhs)
{
  return m_it != rhs.m_it;
}

InMemoryStorage::InMemoryStorage(size_t limit)
  : m_limit(limit)
  , m_nPackets(0)
{
  // TODO consider a more suitable initial value
  m_capacity = 10;

  if (limit != std::numeric_limits<size_t>::max() && m_capacity > m_limit) {
    m_capacity = m_limit;
  }

  for (size_t i = 0; i < m_capacity; i++) {
    m_freeEntries.push(new InMemoryStorageEntry());
  }
}

InMemoryStorage::~InMemoryStorage()
{
  // evict all items from cache
  Cache::iterator it = m_cache.begin();
  while (it != m_cache.end()) {
    it = freeEntry(it);
  }

  BOOST_ASSERT(m_freeEntries.size() == m_capacity);

  while (!m_freeEntries.empty()) {
    delete m_freeEntries.top();
    m_freeEntries.pop();
  }
}

void
InMemoryStorage::setCapacity(size_t capacity)
{
  size_t oldCapacity = m_capacity;
  m_capacity = capacity;

  if (size() > m_capacity) {
    ssize_t nAllowedFailures = size() - m_capacity;
    while (size() > m_capacity) {
      if (!evictItem() && --nAllowedFailures < 0) {
        BOOST_THROW_EXCEPTION(Error());
      }
    }
  }

  if (m_capacity >= oldCapacity) {
    for (size_t i = oldCapacity; i < m_capacity; i++) {
      m_freeEntries.push(new InMemoryStorageEntry());
    }
  }
  else {
    for (size_t i = oldCapacity; i > m_capacity; i--) {
      delete m_freeEntries.top();
      m_freeEntries.pop();
    }
  }

  BOOST_ASSERT(size() + m_freeEntries.size() == m_capacity);
}

void
InMemoryStorage::insert(const Data& data)
{
  //check if identical Data/Name already exists
  Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().find(data.getFullName());
  if (it != m_cache.get<byFullName>().end())
    return;

  //if full, double the capacity
  bool doesReachLimit = (getLimit() == getCapacity());
  if (isFull() && !doesReachLimit) {
    // note: This is incorrect if 2*capacity overflows, but memory should run out before that
    size_t newCapacity = std::min(2 * getCapacity(), getLimit());
    setCapacity(newCapacity);
  }

  //if full and reach limitation of the capacity, employ replacement policy
  if (isFull() && doesReachLimit) {
    evictItem();
  }

  //insert to cache
  BOOST_ASSERT(m_freeEntries.size() > 0);
  // take entry for the memory pool
  InMemoryStorageEntry* entry = m_freeEntries.top();
  m_freeEntries.pop();
  m_nPackets++;
  entry->setData(data);
  m_cache.insert(entry);

  //let derived class do something with the entry
  afterInsert(entry);
}

shared_ptr<const Data>
InMemoryStorage::find(const Name& name)
{
  Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().lower_bound(name);

  //if not found, return null
  if (it == m_cache.get<byFullName>().end()) {
    return shared_ptr<const Data>();
  }

  //if the given name is not the prefix of the lower_bound, return null
  if (!name.isPrefixOf((*it)->getFullName())) {
    return shared_ptr<const Data>();
  }

  afterAccess(*it);
  return ((*it)->getData()).shared_from_this();
}

shared_ptr<const Data>
InMemoryStorage::find(const Interest& interest)
{
  //if the interest contains implicit digest, it is possible to directly locate a packet.
  Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>()
                                                    .find(interest.getName());

  //if a packet is located by its full name, it must be the packet to return.
  if (it != m_cache.get<byFullName>().end()) {
    return ((*it)->getData()).shared_from_this();
  }

  //if the packet is not discovered by last step, either the packet is not in the storage or
  //the interest doesn't contains implicit digest.
  it = m_cache.get<byFullName>().lower_bound(interest.getName());

  if (it == m_cache.get<byFullName>().end()) {
    return shared_ptr<const Data>();
  }


  //to locate the element that has a just smaller name than the interest's
  if (it != m_cache.get<byFullName>().begin())
    it--;

  InMemoryStorageEntry* ret = selectChild(interest, it);
  if (ret != 0) {
    //let derived class do something with the entry
    afterAccess(ret);
    return ret->getData().shared_from_this();
  }
  else {
    return shared_ptr<const Data>();
  }
}

InMemoryStorageEntry*
InMemoryStorage::selectChild(const Interest& interest,
                             Cache::index<byFullName>::type::iterator startingPoint) const
{
  BOOST_ASSERT(startingPoint != m_cache.get<byFullName>().end());

  if (startingPoint != m_cache.get<byFullName>().begin())
    {
      BOOST_ASSERT((*startingPoint)->getFullName() < interest.getName());
    }

  bool hasLeftmostSelector = (interest.getChildSelector() <= 0);
  bool hasRightmostSelector = !hasLeftmostSelector;

  if (hasLeftmostSelector)
    {
      if (interest.matchesData((*startingPoint)->getData()))
        {
          return *startingPoint;
        }
    }

  //iterate to the right
  Cache::index<byFullName>::type::iterator rightmost = startingPoint;
  if (startingPoint != m_cache.get<byFullName>().end())
    {
      Cache::index<byFullName>::type::iterator rightmostCandidate = startingPoint;
      Name currentChildPrefix("");

      while (true)
        {
          ++rightmostCandidate;

          bool isInBoundaries = (rightmostCandidate != m_cache.get<byFullName>().end());
          bool isInPrefix = false;
          if (isInBoundaries)
            {
              isInPrefix = interest.getName().isPrefixOf((*rightmostCandidate)->getFullName());
            }

          if (isInPrefix)
            {
              if (interest.matchesData((*rightmostCandidate)->getData()))
                {
                  if (hasLeftmostSelector)
                    {
                      return *rightmostCandidate;
                    }

                  if (hasRightmostSelector)
                    {
                      // get prefix which is one component longer than Interest name
                      const Name& childPrefix = (*rightmostCandidate)->getFullName()
                                                  .getPrefix(interest.getName().size() + 1);

                      if (currentChildPrefix.empty() || (childPrefix != currentChildPrefix))
                        {
                          currentChildPrefix = childPrefix;
                          rightmost = rightmostCandidate;
                        }
                    }
                }
            }
          else
            break;
        }
    }

  if (rightmost != startingPoint)
    {
      return *rightmost;
    }

  if (hasRightmostSelector) // if rightmost was not found, try starting point
    {
      if (interest.matchesData((*startingPoint)->getData()))
        {
          return *startingPoint;
        }
    }

  return 0;
}

InMemoryStorage::Cache::iterator
InMemoryStorage::freeEntry(Cache::iterator it)
{
  //push the *empty* entry into mem pool
  (*it)->release();
  m_freeEntries.push(*it);
  m_nPackets--;
  return m_cache.erase(it);
}

void
InMemoryStorage::erase(const Name& prefix, const bool isPrefix)
{
  if (isPrefix) {
    Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().lower_bound(prefix);

    while (it != m_cache.get<byFullName>().end() && prefix.isPrefixOf((*it)->getName())) {
      //let derived class do something with the entry
      beforeErase(*it);
      it = freeEntry(it);
    }
  }
  else {
    Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().find(prefix);

    if (it == m_cache.get<byFullName>().end())
      return;

    //let derived class do something with the entry
    beforeErase(*it);
    freeEntry(it);
  }

  if (m_freeEntries.size() > (2 * size()))
    setCapacity(getCapacity() / 2);
}

void
InMemoryStorage::eraseImpl(const Name& name)
{
  Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().find(name);

  if (it == m_cache.get<byFullName>().end())
    return;

  freeEntry(it);
}

InMemoryStorage::const_iterator
InMemoryStorage::begin() const
{
  Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().begin();

  return const_iterator(&((*it)->getData()), &m_cache, it);
}

InMemoryStorage::const_iterator
InMemoryStorage::end() const
{
  Cache::index<byFullName>::type::iterator it = m_cache.get<byFullName>().end();

  const Data* ptr = NULL;

  return const_iterator(ptr, &m_cache, it);
}

void
InMemoryStorage::afterInsert(InMemoryStorageEntry* entry)
{
}

void
InMemoryStorage::beforeErase(InMemoryStorageEntry* entry)
{
}

void
InMemoryStorage::afterAccess(InMemoryStorageEntry* entry)
{
}

void
InMemoryStorage::printCache(std::ostream& os) const
{
  //start from the upper layer towards bottom
  const Cache::index<byFullName>::type& cacheIndex = m_cache.get<byFullName>();
  for (Cache::index<byFullName>::type::iterator it = cacheIndex.begin();
       it != cacheIndex.end(); it++)
    os << (*it)->getFullName() << std::endl;
}

} // namespace util
} // namespace ndn
