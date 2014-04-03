/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "management/ndnd-forwarding-entry.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace ndnd {

const uint8_t FORWARDING_ENTRY[] = {0x81, 0x19, 0x83, 0x07, 0x73, 0x65, 0x6c, 0x66, 0x72, 0x65, 0x67,
                                      0x07, 0x0b,
                                        0x08, 0x01, 0x61,
                                        0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
                                      0x8a, 0x01, 0x03};

BOOST_AUTO_TEST_SUITE(TestForwardingEntry)

BOOST_AUTO_TEST_CASE (Encode)
{
  ForwardingEntry forwardingEntry("selfreg", "/a/prefix", -1, ForwardingFlags(), time::milliseconds::min());
  const Block &wire = forwardingEntry.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(FORWARDING_ENTRY, FORWARDING_ENTRY+sizeof(FORWARDING_ENTRY),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE (Decode)
{
  ForwardingEntry forwardingEntry;

  BOOST_REQUIRE_NO_THROW(forwardingEntry.wireDecode(Block(FORWARDING_ENTRY, sizeof(FORWARDING_ENTRY))));

  BOOST_REQUIRE_EQUAL(forwardingEntry.getAction(), "selfreg");
  BOOST_REQUIRE_EQUAL(forwardingEntry.getPrefix(), Name("/a/prefix"));
  BOOST_REQUIRE_EQUAL(forwardingEntry.getFaceId(), -1);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getActive(), true);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getChildInherit(), true);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getAdvertise(), false);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getLast(), false);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getCapture(), false);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getLocal(), false);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getTap(), false);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getForwardingFlags().getCaptureOk(), false);
  BOOST_REQUIRE_EQUAL(forwardingEntry.getFreshnessPeriod(), time::milliseconds::min());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndnd
} // namespace ndn
