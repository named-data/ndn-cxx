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

#include "sqlite3-statement.hpp"

#include <sqlite3.h>

namespace ndn {
namespace util {

Sqlite3Statement::~Sqlite3Statement()
{
  sqlite3_finalize(m_stmt);
}

Sqlite3Statement::Sqlite3Statement(sqlite3* database, const std::string& statement)
{
  int res = sqlite3_prepare_v2(database, statement.c_str(), -1, &m_stmt, nullptr);
  if (res != SQLITE_OK)
    BOOST_THROW_EXCEPTION(std::domain_error("bad SQL statement: " + statement));
}

int
Sqlite3Statement::bind(int index, const char* value, size_t size, void(*destructor)(void*))
{
  return sqlite3_bind_text(m_stmt, index, value, size, destructor);
}

int
Sqlite3Statement::bind(int index, const std::string& value, void(*destructor)(void*))
{
  return sqlite3_bind_text(m_stmt, index, value.c_str(), value.size(), destructor);
}

int
Sqlite3Statement::bind(int index, const void* buf, size_t size, void(*destructor)(void*))
{
  return sqlite3_bind_blob(m_stmt, index, buf, size, destructor);
}

int
Sqlite3Statement::bind(int index, const Block& block, void(*destructor)(void*))
{
  return sqlite3_bind_blob(m_stmt, index, block.wire(), block.size(), destructor);
}

int
Sqlite3Statement::bind(int index, int number)
{
  return sqlite3_bind_int(m_stmt, index, number);
}

std::string
Sqlite3Statement::getString(int column)
{
  return std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, column)),
                     sqlite3_column_bytes(m_stmt, column));
}

Block
Sqlite3Statement::getBlock(int column)
{
  return Block(sqlite3_column_blob(m_stmt, column), sqlite3_column_bytes(m_stmt, column));
}

int
Sqlite3Statement::getInt(int column)
{
  return sqlite3_column_int(m_stmt, column);
}


const uint8_t*
Sqlite3Statement::getBlob(int column)
{
  return static_cast<const uint8_t*>(sqlite3_column_blob(m_stmt, column));
}

int
Sqlite3Statement::getSize(int column)
{
  return sqlite3_column_bytes(m_stmt, column);
}

int
Sqlite3Statement::step()
{
  return sqlite3_step(m_stmt);
}

Sqlite3Statement::operator sqlite3_stmt*()
{
  return m_stmt;
}

} // namespace util
} // namespace ndn
