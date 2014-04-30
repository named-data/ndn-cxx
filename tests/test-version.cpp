/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
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
