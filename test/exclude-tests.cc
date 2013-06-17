/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ndn-cpp/fields/exclude.h"
#include "ndn-cpp/error.h"

#include <boost/test/unit_test.hpp>

#include <boost/lexical_cast.hpp>

using namespace ndn;
using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(ExcludeTests)

BOOST_AUTO_TEST_CASE (Basic)
{
  Exclude e;
  e.excludeOne (string ("b"));
  BOOST_CHECK_EQUAL (e.size (), 1);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "b ");

  e.excludeOne (string ("d"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "b d ");

  e.excludeOne (string ("a"));
  BOOST_CHECK_EQUAL (e.size (), 3);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "a b d ");

  e.excludeOne (string ("aa"));
  BOOST_CHECK_EQUAL (e.size (), 4);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "a b d aa ");

  e.excludeOne (string ("cc"));
  BOOST_CHECK_EQUAL (e.size (), 5);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "a b d aa cc ");

  e.excludeOne (string ("c"));
  BOOST_CHECK_EQUAL (e.size (), 6);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "a b c d aa cc ");
}

BOOST_AUTO_TEST_CASE (Ranges)
{
// example: ANY /b /d ANY /f

  Exclude e;
  e.excludeOne (string ("b0"));
  BOOST_CHECK_EQUAL (e.size (), 1);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), "b0 ");

  e.excludeRange (name::Component (), string ("b1"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> b1 ");

  e.excludeRange (name::Component (), string ("c0"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> c0 ");

  e.excludeRange (string ("a0"), string ("c0"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> c0 ");

  e.excludeRange (string ("d0"), string ("e0"));
  BOOST_CHECK_EQUAL (e.size (), 4);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> c0 d0 ----> e0 ");

  e.excludeRange (string ("c1"), string ("d1"));
  BOOST_CHECK_EQUAL (e.size (), 4);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> c0 c1 ----> e0 ");

  e.excludeRange (string ("a1"), string ("d1"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> e0 ");

  e.excludeBefore (string ("e2"));
  BOOST_CHECK_EQUAL (e.size (), 2);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> e2 ");

  e.excludeAfter (string ("f0"));
  BOOST_CHECK_EQUAL (e.size (), 3);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> e2 f0 ----> ");

  e.excludeAfter (string ("e5"));
  BOOST_CHECK_EQUAL (e.size (), 3);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> e2 e5 ----> ");

  e.excludeAfter (string ("b2"));
  BOOST_CHECK_EQUAL (e.size (), 1);
  BOOST_CHECK_EQUAL (lexical_cast<string> (e), " ----> ");

  BOOST_REQUIRE_THROW (e.excludeRange (string ("d0"), string ("a0")), error::Exclude);
}

BOOST_AUTO_TEST_SUITE_END()
