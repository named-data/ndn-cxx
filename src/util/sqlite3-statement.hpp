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

#ifndef NDN_UTIL_SQLITE3_STATEMENT_HPP
#define NDN_UTIL_SQLITE3_STATEMENT_HPP

#include "../encoding/block.hpp"
#include <string>

struct sqlite3;
struct sqlite3_stmt;

namespace ndn {
namespace util {

/**
 * @brief wrap an SQLite3 prepared statement
 * @warning This class is implementation detail of ndn-cxx library.
 */
class Sqlite3Statement : noncopyable
{
public:
  /**
   * @brief initialize and prepare Sqlite3 statement
   * @param database handler to open sqlite3 database
   * @param statement SQL statement
   * @throw std::domain_error SQL statement is bad
   */
  Sqlite3Statement(sqlite3* database, const std::string& statement);

  /**
   * @brief finalize the statement
   */
  ~Sqlite3Statement();

  /**
   * @brief bind a string to the statement
   *
   * @param index The binding position
   * @param value The pointer of the binding string
   * @param size The size of the binding string
   * @param destructor SQLite3 destructor, e.g., SQLITE_TRANSIENT
   * @return SQLite result value.
   */
  int
  bind(int index, const char* value, size_t size, void(*destructor)(void*));

  /**
   * @brief bind a string to the statement
   *
   * @param index The binding position
   * @param value The binding string
   * @param destructor SQLite3 destructor, e.g., SQLITE_TRANSIENT
   * @return SQLite result value.
   */
  int
  bind(int index, const std::string& value, void(*destructor)(void*));

  /**
   * @brief bind a byte blob to the statement
   *
   * @param index The binding position
   * @param value The pointer of the blob
   * @param size The size of the blob
   * @param destructor SQLite3 destructor, e.g., SQLITE_TRANSIENT
   * @return SQLite result value.
   */
  int
  bind(int index, const void* value, size_t size, void(*destructor)(void*));

  /**
   * @brief bind a byte blob to the statement
   *
   * @param index The binding position
   * @param block The binding block
   * @param destructor SQLite3 destructor, e.g., SQLITE_TRANSIENT
   * @return SQLite result value
   */
  int
  bind(int index, const Block& block, void(*destructor)(void*));

  /**
   * @brief bind an integer to the statement
   *
   * @param index The binding position
   * @param number The binding integer
   * @return SQLite result value
   */
  int
  bind(int index, int number);

  /**
   * @brief get a string from @p column.
   */
  std::string
  getString(int column);

  /**
   * @brief get a block from @p column.
   */
  Block
  getBlock(int column);

  /**
   * @brief get an integer from @p column.
   */
  int
  getInt(int column);

  /**
   * @brief get a pointer of byte blob from @p column.
   */
  const uint8_t*
  getBlob(int column);

  /**
   * @brief get the size of @p column.
   */
  int
  getSize(int column);

  /**
   * @brief wrapper of sqlite3_step
   */
  int
  step();

  /**
   * @brief implicitly converts to sqlite3_stmt* to be used in SQLite C API
   */
  operator sqlite3_stmt*();

private:
  sqlite3_stmt* m_stmt;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_SQLITE3_STATEMENT_HPP
