/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
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
