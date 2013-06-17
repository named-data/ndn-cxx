/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ndn-cpp/wire/ccnb.h"
#include "ndn-cpp/interest.h"

#include <unistd.h>
#include <fstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/make_shared.hpp>

#include "logging.h"

using namespace ndn;
using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(InterestTests)

static const string Interest1 ("\x01\xD2\xF2\x00\x05\x9A\x8E\x32\x00\x05\xA2\x8E\x32\x00\x05\xAA\x8E\x31\x00\x02"
                               "\xFA\x8E\x34\x00\x02\xD2\x8E\x30\x00\x03\x82\x95\xA0\x00\x00\x00",
                               36);

static const string Interest2 ("\x01\xD2\xF2\x00\x05\x9A\x8E\x32\x00\x05\xA2\x8E\x32\x00\x03\x82\x95\xA0\x00\x00"
                               "\x00",
                               21);

static const string Interest3 ("\x01\xD2\xF2\x00\x05\x9A\x8E\x32\x00\x05\xA2\x8E\x32\x00\x02\xDA\xFA\xA5\x61\x6C" \
                               "\x65\x78\x00\xFA\xC5\x7A\x68\x65\x6E\x6B\x61\x69\x30\x00\xEA\x00\xFA\xC5\x7A\x68" \
                               "\x65\x6E\x6B\x61\x69\x31\x00\xFA\x01\x8D\x6C\x6F\x6F\x6F\x6F\x6F\x6F\x6F\x6F\x6F" \
                               "\x6F\x6F\x6F\x6F\x6F\x6E\x67\x00\xEA\x00\x00\x05\xAA\x8E\x31\x00\x02\xFA\x8E\x34" \
                               "\x00\x02\xD2\x8E\x30\x00\x03\x82\x95\xA0\x00\x00\x00", 93);

BOOST_AUTO_TEST_CASE (Basic)
{
  INIT_LOGGERS ();
  
  Interest i;
  i.setName (Name ("/test"));
  i.setMinSuffixComponents (2);
  i.setMaxSuffixComponents (2);
  i.setInterestLifetime (posix_time::seconds (10));
  i.setScope (Interest::SCOPE_LOCAL_CCND);
  i.setAnswerOriginKind (Interest::AOK_STALE);
  i.setChildSelector (Interest::CHILD_RIGHT);
  // i.setPublisherPublicKeyDigest (?);

  ostringstream os;
  wire::Ccnb::appendInterest (os, i);
  string Interest0 = os.str ();
  BOOST_CHECK_EQUAL_COLLECTIONS (Interest0.begin (), Interest0.end (),
                                 Interest1.begin (), Interest1.end ());

  i.getExclude ().excludeOne (name::Component ("alex"));
  i.getExclude ().excludeRange (name::Component ("zhenkai0"), name::Component("zhenkai1"));
  i.getExclude ().excludeAfter (name::Component ("loooooooooooooong"));

  BOOST_CHECK_EQUAL (boost::lexical_cast<string> (i.getExclude ()), "alex zhenkai0 ----> zhenkai1 loooooooooooooong ----> ");

  os.str (""); os.clear ();
  wire::Ccnb::appendInterest (os, i);
  Interest0 = os.str ();
  BOOST_CHECK_EQUAL_COLLECTIONS (Interest0.begin (), Interest0.end (),
                                 Interest3.begin (), Interest3.end ());
}

// BOOST_AUTO_TEST_CASE (Charbuf)
// {
//   INIT_LOGGERS ();

//   Interest i;
//   i.setName (Name ("/test"));
//   i.setMinSuffixComponents (2);
//   i.setMaxSuffixComponents (2);
//   i.setInterestLifetime (posix_time::seconds (10));

//   charbuf_stream stream;
//   wire::Ccnb::appendInterest (stream, i);

//   BOOST_CHECK_EQUAL_COLLECTIONS (reinterpret_cast<char*> (stream.buf ().getBuf ()->buf),
//                                  reinterpret_cast<char*> (stream.buf ().getBuf ()->buf+stream.buf ().getBuf ()->length),
//                                  Interest2.begin (), Interest2.end ());
  
// }

BOOST_AUTO_TEST_SUITE_END()
