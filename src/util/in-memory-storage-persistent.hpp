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

#ifndef NDN_UTIL_IN_MEMORY_STORAGE_PERSISTENT_HPP
#define NDN_UTIL_IN_MEMORY_STORAGE_PERSISTENT_HPP

#include "in-memory-storage.hpp"

namespace ndn {
namespace util {

/** @brief Provides application cache with persistent storage, of which no replacement policy will
 *  be employed. Entries will only be deleted by explicitly application control.
 */
class InMemoryStoragePersistent : public InMemoryStorage
{
public:
  explicit
  InMemoryStoragePersistent();

  virtual
  ~InMemoryStoragePersistent();

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  /** @brief Do nothing.
   *
   *  This storage is persistent, and does not support eviction.
   *
   *  @return false
   */
  virtual bool
  evictItem();
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_IN_MEMORY_STORAGE_PERSISTENT_HPP
