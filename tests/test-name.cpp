/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "name.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestName)

static const uint8_t TestName[] = {
        0x7,  0x14, // Name
          0x8,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x8,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x8,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78
};

const uint8_t Name1[] = {0x7,  0x7, // Name
                           0x8,  0x5, // NameComponent
                             0x6c,  0x6f,  0x63,  0x61,  0x6c};
    
const uint8_t Name2[] = {0x7,  0xc, // Name
                           0x8,  0x5, // NameComponent
                             0x6c,  0x6f,  0x63,  0x61,  0x6c,
                           0x8,  0x3, // NameComponent
                             0x6e,  0x64,  0x6e};


BOOST_AUTO_TEST_CASE(Basic)
{
  Name name("/hello/world");

  BOOST_CHECK_NO_THROW(name.at(0));
  BOOST_CHECK_NO_THROW(name.at(1));
  BOOST_CHECK_NO_THROW(name.at(-1));
  BOOST_CHECK_NO_THROW(name.at(-2));

  BOOST_CHECK_THROW(name.at(2), Name::Error);
  BOOST_CHECK_THROW(name.at(-3), Name::Error);
}

BOOST_AUTO_TEST_CASE(Encode)
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
  
  BOOST_CHECK_EQUAL_COLLECTIONS(TestName, TestName+sizeof(TestName),
                                wire.begin(), wire.end());
}


BOOST_AUTO_TEST_CASE(Decode)
{
  Block block(TestName, sizeof(TestName));

  Name name(block);

  BOOST_CHECK_EQUAL(name.toUri(), "/local/ndn/prefix");
}

BOOST_AUTO_TEST_CASE(AppendsAndMultiEncode)
{
  Name name("/local");
  
  BOOST_CHECK_EQUAL_COLLECTIONS(name.wireEncode().begin(), name.wireEncode().end(),
                                Name1, Name1 + sizeof(Name1));

  name.append("ndn");
  
  BOOST_CHECK_EQUAL_COLLECTIONS(name.wireEncode().begin(), name.wireEncode().end(),
                                Name2, Name2 + sizeof(Name2));

  name.append("prefix");
  BOOST_CHECK_EQUAL_COLLECTIONS(name.wireEncode().begin(), name.wireEncode().end(),
                                TestName, TestName+sizeof(TestName));
}

BOOST_AUTO_TEST_CASE(AppendNumber)
{
  Name name;
  for (int i = 0; i < 10; i++)
    {
      name.appendNumber(i);
    }

  BOOST_CHECK_EQUAL(name.size(), 10);

  for (int i = 0; i < 10; i++)
    {
      BOOST_CHECK_EQUAL(name[i].toNumber(), i);
    }
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
