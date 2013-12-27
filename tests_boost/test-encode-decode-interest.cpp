/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include <ndn-cpp/interest.hpp>

using namespace std;
using namespace ndn;

BOOST_AUTO_TEST_SUITE(TestInterest)

const uint8_t Interest1[] = {
  0x0,  0x41,
  0x2,  0x14,
  0x3,  0x5,  0x6c,  0x6f,  0x63,  0x61,  0x6c,
  0x3,  0x3,  0x6e,  0x64,  0x6e,
  0x3,  0x6,  0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
  0x4,  0x1f,
  0x8,  0x1,  0x1,
  0x9,  0x1,  0x1,
  0xb,  0x14,
  0x3,  0x4,  0x61,  0x6c,  0x65,  0x78,
  0x3,  0x4,  0x78,  0x78,  0x78,  0x78,
  0xe,  0x0,
  0x3,  0x4,  0x79,  0x79,  0x79,  0x79,
  0xc,  0x1,  0x1,
  0x5,  0x1,  0x0,
  0x6,  0x1,  0x1,
  0x7,  0x2,  0x3,  0xe8
};

BOOST_AUTO_TEST_CASE (Decode)
{
  Block interestBlock(Interest1, sizeof(Interest1));
  
  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_REQUIRE_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_REQUIRE_EQUAL(i.getScope(), 1);
  BOOST_REQUIRE_EQUAL(i.getInterestLifetime(), 1000);
  BOOST_REQUIRE_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_REQUIRE_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_REQUIRE_EQUAL(i.getChildSelector(), 1);
  BOOST_REQUIRE_EQUAL(i.getMustBeFresh(), false);
  BOOST_REQUIRE_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
}

BOOST_AUTO_TEST_CASE (Encode)
{
  ndn::Interest i(ndn::Name("/local/ndn/prefix"));
  i.setScope(1);
  i.setInterestLifetime(1000);
  i.setMinSuffixComponents(1);
  i.setMaxSuffixComponents(1);
  i.setChildSelector(1);
  i.setMustBeFresh(false);
  i.getExclude().excludeOne("alex").excludeRange("xxxx", "yyyy");

  const Block &wire = i.wireEncode();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(Interest1, Interest1+sizeof(Interest1),
                               wire.begin(), wire.end());
}

BOOST_AUTO_TEST_SUITE_END()
