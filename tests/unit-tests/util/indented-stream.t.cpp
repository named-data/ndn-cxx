/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "util/indented-stream.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace util {
namespace tests {

using boost::test_tools::output_test_stream;

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestIndentedStream)

BOOST_AUTO_TEST_CASE(Basic)
{
  output_test_stream os;

  os << "Hello" << std::endl;
  {
    IndentedStream os1(os, " [prefix] ");
    os1 << "," << "\n";
    {
      IndentedStream os2(os1, " [another prefix] ");
      os2 << "World!" << "\n";
    }
  }

  BOOST_CHECK(os.is_equal("Hello\n"
                          " [prefix] ,\n"
                          " [prefix]  [another prefix] World!\n"
                          ));
}

BOOST_AUTO_TEST_CASE(BasicWithFlushes) // Bug #2723
{
  output_test_stream os;

  os << "Hello" << std::endl;
  {
    IndentedStream os1(os, " [prefix] ");
    os1 << "," << std::endl;
    {
      IndentedStream os2(os1, " [another prefix] ");
      os2 << "World!" << std::endl;
    }
  }

  BOOST_CHECK(os.is_equal("Hello\n"
                          " [prefix] ,\n"
                          " [prefix]  [another prefix] World!\n"
                          ));
}

BOOST_AUTO_TEST_SUITE_END() // TestIndentedStream
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
