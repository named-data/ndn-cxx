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

#include "ndn-cxx/detail/common.hpp"

#include "tests/boost-test.hpp"

namespace ndn::tests {

BOOST_AUTO_TEST_SUITE(TestNdebug)

BOOST_AUTO_TEST_CASE(Assert)
{
  BOOST_TEST(BOOST_IS_DEFINED(BOOST_ASSERT_IS_VOID) == BOOST_IS_DEFINED(NDEBUG));

#ifdef NDEBUG
  // in release builds, assertion shouldn't execute
  BOOST_ASSERT(false);
  BOOST_VERIFY(false);
#endif
}

BOOST_AUTO_TEST_CASE(SideEffect)
{
  int a = 1;
  BOOST_ASSERT((a = 2) > 0);
#ifdef NDEBUG
  BOOST_TEST(a == 1);
#else
  BOOST_TEST(a == 2);
#endif
}

BOOST_AUTO_TEST_SUITE_END() // TestNdebug

} // namespace ndn::tests
