/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nrd-prefix-reg-options.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

using namespace std;

namespace ndn {
namespace nrd {

BOOST_AUTO_TEST_SUITE(TestNrd)

const uint8_t RealPrefixRegOptions[] = {
  0x65, 0x1b, 0x07, 0x16, 0x08, 0x09, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74,
  0x08, 0x03, 0x72, 0x65, 0x67, 0x08, 0x04, 0x74, 0x65, 0x73, 0x74, 0x66, 0x01, 0x00,
};

BOOST_AUTO_TEST_CASE (PrefixRegOptionsEncode)
{
  Name n ("/localhost/reg/test");
  PrefixRegOptions opt;

  opt.setName (n);
  opt.setFaceId (0);
  opt.setCost (0);

  const Block& blk = opt.wireEncode ();

  BOOST_CHECK_EQUAL_COLLECTIONS(RealPrefixRegOptions,
                                RealPrefixRegOptions + sizeof (RealPrefixRegOptions),
                                blk.begin (), blk.end ());

  std::ostringstream os;
  os << opt;
  BOOST_CHECK_EQUAL(os.str(), "PrefixRegOptions(Prefix: /localhost/reg/test, "
                    "FaceID: 0, Flags: 1, Cost: 0, ExpirationPeriod: -1, Protocol: )");
}

BOOST_AUTO_TEST_CASE (PrefixRegOptionsDecoding)
{
  Block blk (RealPrefixRegOptions, sizeof (RealPrefixRegOptions));
  Name n ("/localhost/reg/test");
  PrefixRegOptions opt;

  BOOST_REQUIRE_NO_THROW (opt.wireDecode (blk));
  
  BOOST_CHECK_EQUAL (opt.getName (), n);
  BOOST_CHECK_EQUAL (opt.getFaceId (), 0);
  BOOST_CHECK_EQUAL (opt.getCost (), 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nrd
} // namespace ndn
