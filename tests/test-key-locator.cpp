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

#include "key-locator.hpp"

#include "boost-test.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestKeyLocator)

BOOST_AUTO_TEST_CASE(Equality)
{
  BOOST_CONCEPT_ASSERT((boost::EqualityComparable<KeyLocator>));

  KeyLocator a;
  KeyLocator b;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/B");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
