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

#include "ndn-cxx/util/sqlite3-statement.hpp"

#include "tests/boost-test.hpp"

#include <cstring>
#include <filesystem>
#include <sqlite3.h>

namespace ndn::tests {

using ndn::util::Sqlite3Statement;

class Sqlite3DbFixture
{
public:
  Sqlite3DbFixture()
  {
    std::filesystem::create_directories(m_path);

    int result = sqlite3_open_v2((m_path / "sqlite3-statement.db").c_str(), &db,
                                 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
#ifdef NDN_CXX_DISABLE_SQLITE3_FS_LOCKING
                                 "unix-dotfile"
#else
                                 nullptr
#endif
                                 );

    if (result != SQLITE_OK) {
      BOOST_FAIL("Sqlite3 database cannot be opened/created: " + m_path.native());
    }
  }

  ~Sqlite3DbFixture()
  {
    sqlite3_close(db);
    std::filesystem::remove_all(m_path);
  }

protected:
  sqlite3* db = nullptr;

private:
  const std::filesystem::path m_path{UNIT_TESTS_TMPDIR};
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestSqlite3Statement, Sqlite3DbFixture)

BOOST_AUTO_TEST_CASE(Basic)
{
  // create table
  BOOST_CHECK_NO_THROW(Sqlite3Statement(db, "CREATE TABLE test (t1 int, t2 text)").step());

  // insert data into table
  BOOST_CHECK_NO_THROW(Sqlite3Statement(db, "INSERT INTO test VALUES (1, 'test1')").step());

  {
    Sqlite3Statement stmt(db, "INSERT INTO test VALUES (2, ?)");
    stmt.bind(1, "test2", std::strlen("test2"), SQLITE_STATIC);
    stmt.step();
  }

  {
    Sqlite3Statement stmt(db, "INSERT INTO test VALUES (3, ?)");
    stmt.bind(1, "test3", SQLITE_TRANSIENT);
    stmt.step();
  }

  Block block(100);
  block.encode();
  {
    Sqlite3Statement stmt(db, "INSERT INTO test VALUES (4, ?)");
    stmt.bind(1, block, SQLITE_STATIC);
    stmt.step();
  }

  {
    Sqlite3Statement stmt(db, "INSERT INTO test VALUES (5, ?)");
    stmt.bind(1, reinterpret_cast<const void*>(block.data()), block.size(), SQLITE_STATIC);
    stmt.step();
  }

  {
    Sqlite3Statement stmt(db, "INSERT INTO test VALUES (?, ?)");
    stmt.bind(1, 6);
    stmt.bind(2, "test", SQLITE_TRANSIENT);
    stmt.step();
  }

  // check content of the table

  {
    Sqlite3Statement stmt(db, "SELECT count(*) FROM test");
    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.getInt(0), 6);
    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_DONE);
  }

  {
    Sqlite3Statement stmt(db, "SELECT t1, t2 FROM test ORDER BY t1");
    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.getInt(0), 1);
    BOOST_CHECK_EQUAL(stmt.getString(1), "test1");

    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.getInt(0), 2);
    BOOST_CHECK_EQUAL(stmt.getString(1), "test2");

    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.getInt(0), 3);
    BOOST_CHECK_EQUAL(stmt.getString(1), "test3");

    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.getInt(0), 4);

    Block newBlock = stmt.getBlock(1);
    BOOST_CHECK_EQUAL(newBlock.type(), 100);
    BOOST_CHECK_EQUAL(newBlock, block);

    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.getInt(0), 5);
    BOOST_CHECK_EQUAL(stmt.getSize(1), block.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(block.begin(), block.end(),
                                  stmt.getBlob(1), stmt.getBlob(1) + stmt.getSize(1));

    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_ROW);
    BOOST_CHECK_EQUAL(stmt.step(), SQLITE_DONE);
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestSqlite3Statement
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace ndn::tests
