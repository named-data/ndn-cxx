/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "exclude.hpp"

#include <boost/test/unit_test.hpp>

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestExclude)

BOOST_AUTO_TEST_CASE (Basic)
{
  Exclude e;
  e.excludeOne (name::Component("b"));
  BOOST_CHECK_EQUAL (e.size (), 1);
  BOOST_CHECK_EQUAL (e.toUri(), "b");

  e.excludeOne (name::Component("d"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (e.toUri(), "b,d");

  e.excludeOne (name::Component("a"));
  BOOST_CHECK_EQUAL (e.size (), 3);
  BOOST_CHECK_EQUAL (e.toUri(), "a,b,d");

  e.excludeOne (name::Component("aa"));
  BOOST_CHECK_EQUAL (e.size (), 4);
  BOOST_CHECK_EQUAL (e.toUri(), "a,b,d,aa");

  e.excludeOne (name::Component("cc"));
  BOOST_CHECK_EQUAL (e.size (), 5);
  BOOST_CHECK_EQUAL (e.toUri(), "a,b,d,aa,cc");

  e.excludeOne (name::Component("c"));
  BOOST_CHECK_EQUAL (e.size (), 6);
  BOOST_CHECK_EQUAL (e.toUri(), "a,b,c,d,aa,cc");
}

BOOST_AUTO_TEST_CASE (Before)
{
  // based on http://redmine.named-data.net/issues/1158
  ndn::Exclude e;
  BOOST_REQUIRE_NO_THROW(e.excludeBefore(name::Component("PuQxMaf91")));

  BOOST_CHECK_EQUAL(e.toUri(), "*,PuQxMaf91");
}

BOOST_AUTO_TEST_CASE (Ranges)
{
// example: ANY /b /d ANY /f

  Exclude e;
  e.excludeOne (name::Component ("b0"));
  BOOST_CHECK_EQUAL (e.size (), 1);
  BOOST_CHECK_EQUAL (e.toUri(), "b0");

  e.excludeRange (name::Component (), name::Component ("b1"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (e.toUri(), "*,b1");

  e.excludeRange (name::Component (), name::Component("c0"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (e.toUri(), "*,c0");

  e.excludeRange (name::Component("a0"), name::Component("c0"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (e.toUri(), "*,c0");

  e.excludeRange (name::Component("d0"), name::Component("e0"));
  BOOST_CHECK_EQUAL (e.size (), 4);
  BOOST_CHECK_EQUAL (e.toUri(), "*,c0,d0,*,e0");

  e.excludeRange (name::Component("c1"), name::Component("d1"));
  BOOST_CHECK_EQUAL (e.size (), 4);
  BOOST_CHECK_EQUAL (e.toUri(), "*,c0,c1,*,e0");

  e.excludeRange (name::Component("a1"), name::Component("d1"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (e.toUri(), "*,e0");

  e.excludeBefore (name::Component("e2"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (e.toUri(), "*,e2");

  e.excludeAfter (name::Component("f0"));
  BOOST_CHECK_EQUAL (e.size (), 3);
  BOOST_CHECK_EQUAL (e.toUri(), "*,e2,f0,*");

  e.excludeAfter (name::Component("e5"));
  BOOST_CHECK_EQUAL (e.size (), 3);
  BOOST_CHECK_EQUAL (e.toUri(), "*,e2,e5,*");

  e.excludeAfter (name::Component("b2"));
  BOOST_CHECK_EQUAL (e.size (), 1);
  BOOST_CHECK_EQUAL (e.toUri(), "*");

  BOOST_REQUIRE_THROW (e.excludeRange (name::Component("d0"), name::Component("a0")), Exclude::Error);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
