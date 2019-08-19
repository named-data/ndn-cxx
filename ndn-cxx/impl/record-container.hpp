/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_IMPL_RECORD_CONTAINER_HPP
#define NDN_IMPL_RECORD_CONTAINER_HPP

#include "ndn-cxx/detail/common.hpp"
#include "ndn-cxx/util/signal.hpp"

#include <atomic>

namespace ndn {

using RecordId = uintptr_t;

template<typename T>
class RecordContainer;

/** \brief Template of PendingInterest, RegisteredPrefix, and InterestFilterRecord.
 *  \tparam T concrete type
 */
template<typename T>
class RecordBase : noncopyable
{
public:
  RecordId
  getId() const
  {
    BOOST_ASSERT(m_id != 0);
    return m_id;
  }

protected:
  ~RecordBase() = default;

  /** \brief Delete this record from the container.
   */
  void
  deleteSelf()
  {
    BOOST_ASSERT(m_container != nullptr);
    m_container->erase(m_id);
  }

private:
  RecordContainer<T>* m_container = nullptr;
  RecordId m_id = 0;
  friend RecordContainer<T>;
};

/** \brief Container of PendingInterest, RegisteredPrefix, or InterestFilterRecord.
 *  \tparam T record type
 */
template<typename T>
class RecordContainer
{
public:
  using Record = T;
  using Container = std::map<RecordId, Record>;

  /** \brief Retrieve record by ID.
   */
  Record*
  get(RecordId id)
  {
    auto i = m_container.find(id);
    if (i == m_container.end()) {
      return nullptr;
    }
    return &i->second;
  }

  /** \brief Insert a record with given ID.
   */
  template<typename ...TArgs>
  Record&
  put(RecordId id, TArgs&&... args)
  {
    BOOST_ASSERT(id != 0);
    auto it = m_container.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                                  std::forward_as_tuple(std::forward<decltype(args)>(args)...));
    BOOST_ASSERT(it.second);

    Record& record = it.first->second;
    record.m_container = this;
    record.m_id = id;
    return record;
  }

  RecordId
  allocateId()
  {
    return ++m_lastId;
  }

  /** \brief Insert a record with newly assigned ID.
   */
  template<typename ...TArgs>
  Record&
  insert(TArgs&&... args)
  {
    return put(allocateId(), std::forward<decltype(args)>(args)...);
  }

  void
  erase(RecordId id)
  {
    m_container.erase(id);
    if (empty()) {
      this->onEmpty();
    }
  }

  void
  clear()
  {
    m_container.clear();
    this->onEmpty();
  }

  /** \brief Visit all records with the option to erase.
   *  \tparam Visitor function of type 'bool f(Record& record)'
   *  \param f visitor function, return true to erase record
   */
  template<typename Visitor>
  void
  removeIf(const Visitor& f)
  {
    for (auto i = m_container.begin(); i != m_container.end(); ) {
      bool wantErase = f(i->second);
      if (wantErase) {
        i = m_container.erase(i);
      }
      else {
        ++i;
      }
    }
    if (empty()) {
      this->onEmpty();
    }
  }

  /** \brief Visit all records.
   *  \tparam Visitor function of type 'void f(Record& record)'
   *  \param f visitor function
   */
  template<typename Visitor>
  void
  forEach(const Visitor& f)
  {
    removeIf([&f] (Record& record) {
      f(record);
      return false;
    });
  }

  NDN_CXX_NODISCARD bool
  empty() const noexcept
  {
    return m_container.empty();
  }

  size_t
  size() const noexcept
  {
    return m_container.size();
  }

public:
  /** \brief Signals when container becomes empty
   */
  util::Signal<RecordContainer<T>> onEmpty;

private:
  Container m_container;
  std::atomic<RecordId> m_lastId{0};
};

} // namespace ndn

#endif // NDN_IMPL_RECORD_CONTAINER_HPP
