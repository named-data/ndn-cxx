/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "name.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestName)

static const uint8_t TestName[] = {
        0x3,  0x14, // Name
          0x4,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x4,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x4,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78
};

BOOST_AUTO_TEST_CASE (Encode)
{
  Name name("/local/ndn/prefix");

  const Block &wire = name.wireEncode();

  // for (Buffer::const_iterator i = wire.begin();
  //      i != wire.end();
  //      ++i)
  //   {
  //     std::ios::fmtflags saveFlags = std::cout.flags(std::ios::hex);

  //     if (i != wire.begin())
  //       std::cout << ", ";
  //     std::cout << "0x" << static_cast<uint32_t>(*i);
      
  //     std::cout.flags(saveFlags);
  //   }
  // std::cout << std::endl;
  
  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestName, TestName+sizeof(TestName),
                                  wire.begin(), wire.end());
}


BOOST_AUTO_TEST_CASE (Decode)
{
  Block block(TestName, sizeof(TestName));

  Name name(block);

  BOOST_CHECK_EQUAL(name.toUri(), "/local/ndn/prefix");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
