/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "interest.hpp"

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestBlock)

const uint8_t Interest1[] = {
  0x1,  0x41, // NDN Interest
      0x3,  0x14, // Name
          0x4,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x4,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x4,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x5,  0x1f, // Selectors
          0x09,  0x1,  0x1,  // MinSuffix
          0x0a,  0x1,  0x1,  // MaxSuffix
          0x0c,  0x14, // Exclude
              0x4,  0x4, // NameComponent
                  0x61,  0x6c,  0x65,  0x78,
              0x4,  0x4, // NameComponent
                  0x78,  0x78,  0x78,  0x78,
              0xf,  0x0, // Any
              0x4,  0x4, // NameComponent
                  0x79,  0x79,  0x79,  0x79,
          0x0d,  0x1, // ChildSelector
              0x1,
      0x6,  0x1, // Nonce
          0x1,
      0x7,  0x1, // Scope
          0x1,
      0x8,       // InterestLifetime
          0x2,  0x3,  0xe8
};

BOOST_AUTO_TEST_CASE (Decode)
{
  boost::iostreams::stream<boost::iostreams::array_source> is (reinterpret_cast<const char *>(Interest1), sizeof(Interest1));
  
  Block interestBlock(is);

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
  BOOST_REQUIRE_EQUAL(i.getNonce(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
