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

#include "ndn-cxx/util/backports.hpp"
#include "ndn-cxx/util/ostream-joiner.hpp"

#include "tests/boost-test.hpp"

#include <numeric>

#if BOOST_VERSION >= 105900
#include <boost/test/tools/output_test_stream.hpp>
#else
#include <boost/test/output_test_stream.hpp>
#endif

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestBackports)

BOOST_AUTO_TEST_CASE(Clamp)
{
  int x = clamp(5, 1, 10);
  BOOST_CHECK_EQUAL(x, 5);

  x = clamp(-5, 1, 10);
  BOOST_CHECK_EQUAL(x, 1);

  x = clamp(15, 1, 10);
  BOOST_CHECK_EQUAL(x, 10);

  x = clamp(5, 10, 1, std::greater<int>());
  BOOST_CHECK_EQUAL(x, 5);

  x = clamp(-5, 10, 1, std::greater<int>());
  BOOST_CHECK_EQUAL(x, 1);

  x = clamp(15, 10, 1, std::greater<int>());
  BOOST_CHECK_EQUAL(x, 10);
}

BOOST_AUTO_TEST_CASE(OstreamJoiner)
{
  boost::test_tools::output_test_stream os;

  auto joiner1 = ostream_joiner<char>(os, ' ');
  auto joiner2 = make_ostream_joiner(os, ' ');
  static_assert(std::is_same<decltype(joiner1), decltype(joiner2)>::value, "");

  std::vector<int> v(5);
  std::iota(v.begin(), v.end(), 1);
  std::copy(v.begin(), v.end(), joiner2);
  BOOST_CHECK(os.is_equal("1 2 3 4 5"));

  auto joiner3 = make_ostream_joiner(os, "...");
  std::copy(v.begin(), v.end(), joiner3);
  BOOST_CHECK(os.is_equal("1...2...3...4...5"));

  joiner3 = "one";
  BOOST_CHECK(os.is_equal("one"));
  joiner3 = "two";
  BOOST_CHECK(os.is_equal("...two"));
  ++joiner3 = "three";
  BOOST_CHECK(os.is_equal("...three"));
  joiner3++ = "four";
  BOOST_CHECK(os.is_equal("...four"));

  std::copy(v.begin(), v.end(), make_ostream_joiner(os, ""));
  BOOST_CHECK(os.is_equal("12345"));

  std::string delimiter("_");
  std::copy(v.begin(), v.end(), make_ostream_joiner(os, delimiter));
  BOOST_CHECK(os.is_equal("1_2_3_4_5"));
}

BOOST_AUTO_TEST_SUITE_END() // TestBackports
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace ndn
