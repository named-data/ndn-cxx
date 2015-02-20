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

#include "version.hpp"

#include "boost-test.hpp"
#include <stdio.h>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestVersion)

BOOST_AUTO_TEST_CASE(Version)
{
  BOOST_CHECK_EQUAL(NDN_CXX_VERSION, NDN_CXX_VERSION_MAJOR * 1000000 +
                                     NDN_CXX_VERSION_MINOR * 1000 +
                                     NDN_CXX_VERSION_PATCH);
}

BOOST_AUTO_TEST_CASE(VersionString)
{
  BOOST_STATIC_ASSERT(NDN_CXX_VERSION_MAJOR < 1000);
  char buf[20];
  snprintf(buf, sizeof(buf), "%d.%d.%d",
           NDN_CXX_VERSION_MAJOR, NDN_CXX_VERSION_MINOR, NDN_CXX_VERSION_PATCH);

  BOOST_CHECK_EQUAL(std::string(NDN_CXX_VERSION_STRING), std::string(buf));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
