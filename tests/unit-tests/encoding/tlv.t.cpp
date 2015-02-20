/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "encoding/tlv.hpp"

#include "boost-test.hpp"
#include <boost/iostreams/stream.hpp>

namespace ndn {
namespace tlv {
namespace tests {

using std::ostringstream;

BOOST_AUTO_TEST_SUITE(EncodingTlv)

BOOST_AUTO_TEST_CASE(Exception)
{
  BOOST_CHECK_THROW(throw Error("Test"), Error);
}

BOOST_AUTO_TEST_SUITE(VarNumber)

static const uint8_t BUFFER[] = {
  0x01, // == 1
  0xfc, // == 252
  0xfd, 0x00, 0xfd, // == 253
  0xfe, 0x00, 0x01, 0x00, 0x00, // == 65536
  0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 // == 4294967296LL
};

BOOST_AUTO_TEST_CASE(SizeOf)
{
  BOOST_CHECK_EQUAL(sizeOfVarNumber(1), 1);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(252), 1);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(253), 3);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(65536), 5);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(4294967296LL), 9);
}

BOOST_AUTO_TEST_CASE(Write)
{
  ostringstream os;

  writeVarNumber(os, 1);
  writeVarNumber(os, 252);
  writeVarNumber(os, 253);
  writeVarNumber(os, 65536);
  writeVarNumber(os, 4294967296LL);

  std::string buffer = os.str();
  const uint8_t* actual = reinterpret_cast<const uint8_t*>(buffer.c_str());

  BOOST_CHECK_EQUAL(buffer.size(), sizeof(BUFFER));
  BOOST_CHECK_EQUAL_COLLECTIONS(BUFFER, BUFFER + sizeof(BUFFER),
                                actual, actual + sizeof(BUFFER));
}

BOOST_AUTO_TEST_CASE(ReadFromBuffer)
{
  const uint8_t* begin;
  uint64_t value;

  begin = BUFFER;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 1, value), true);
  begin = BUFFER;
  BOOST_CHECK_NO_THROW(readVarNumber(begin, begin + 1));
  BOOST_CHECK_EQUAL(value, 1);

  begin = BUFFER + 1;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 1, value), true);
  begin = BUFFER + 1;
  BOOST_CHECK_NO_THROW(readVarNumber(begin, begin + 1));
  BOOST_CHECK_EQUAL(value, 252);

  begin = BUFFER + 2;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 1, value), false);
  begin = BUFFER + 2;
  BOOST_CHECK_THROW(readVarNumber(begin, begin + 1), Error);

  begin = BUFFER + 2;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 2, value), false);
  begin = BUFFER + 2;
  BOOST_CHECK_THROW(readVarNumber(begin, begin + 2), Error);

  begin = BUFFER + 2;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 3, value), true);
  begin = BUFFER + 2;
  BOOST_CHECK_NO_THROW(readVarNumber(begin, begin + 3));
  BOOST_CHECK_EQUAL(value, 253);


  begin = BUFFER + 5;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 1, value), false);
  begin = BUFFER + 5;
  BOOST_CHECK_THROW(readVarNumber(begin, begin + 1), Error);

  begin = BUFFER + 5;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 4, value), false);
  begin = BUFFER + 5;
  BOOST_CHECK_THROW(readVarNumber(begin, begin + 4), Error);

  begin = BUFFER + 5;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 5, value), true);
  begin = BUFFER + 5;
  BOOST_CHECK_NO_THROW(readVarNumber(begin, begin + 5));
  BOOST_CHECK_EQUAL(value, 65536);

  begin = BUFFER + 10;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 1, value), false);
  begin = BUFFER + 10;
  BOOST_CHECK_THROW(readVarNumber(begin, begin + 1), Error);

  begin = BUFFER + 10;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 8, value), false);
  begin = BUFFER + 10;
  BOOST_CHECK_THROW(readVarNumber(begin, begin + 8), Error);

  begin = BUFFER + 10;
  BOOST_CHECK_EQUAL(readVarNumber(begin, begin + 9, value), true);
  begin = BUFFER + 10;
  BOOST_CHECK_NO_THROW(readVarNumber(begin, begin + 9));
  BOOST_CHECK_EQUAL(value, 4294967296LL);
}

BOOST_AUTO_TEST_CASE(ReadFromStream)
{
  typedef boost::iostreams::stream<boost::iostreams::array_source> ArrayStream;
  typedef std::istream_iterator<uint8_t> Iterator;

  Iterator end; // end of stream
  uint64_t value;
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 1);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 1);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 1);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 1, 1);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 1, 1);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 252);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 1);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 1);
    Iterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 2);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 2);
    Iterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 3);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 3);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 253);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 1);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 1);
    Iterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 4);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 4);
    Iterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 5);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 5);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 65536);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 1);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 1);
    Iterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 8);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 8);
    Iterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 9);
    Iterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 9);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 4294967296LL);
  }
}

BOOST_AUTO_TEST_SUITE_END() // VarNumber

BOOST_AUTO_TEST_SUITE(NonNegativeInteger)

static const uint8_t BUFFER[] = {
  0x01, // 1
  0x01, 0x01, // 257
  0x01, 0x01, 0x01, 0x01, // 16843009LL
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 // 72340172838076673LL
};

BOOST_AUTO_TEST_CASE(SizeOf)
{
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(1), 1);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(252), 1);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(253), 2);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(257), 2);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(65536), 4);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(16843009LL), 4);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(4294967296LL), 8);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(72340172838076673LL), 8);
}

BOOST_AUTO_TEST_CASE(Write)
{
  ostringstream os;

  writeNonNegativeInteger(os, 1);
  writeNonNegativeInteger(os, 257);
  writeNonNegativeInteger(os, 16843009LL);
  writeNonNegativeInteger(os, 72340172838076673LL);

  std::string buffer = os.str();
  const uint8_t* actual = reinterpret_cast<const uint8_t*>(buffer.c_str());

  BOOST_CHECK_EQUAL(buffer.size(), sizeof(BUFFER));
  BOOST_CHECK_EQUAL_COLLECTIONS(BUFFER, BUFFER + sizeof(BUFFER),
                                actual, actual + sizeof(BUFFER));
}


BOOST_AUTO_TEST_CASE(ReadFromBuffer)
{
  const uint8_t* begin;
  uint64_t value;

  begin = BUFFER;
  BOOST_CHECK_THROW(value = readNonNegativeInteger(1, begin, begin + 0), Error);
  BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(1, begin, begin + 1));
  BOOST_CHECK_EQUAL(value, 1);

  begin = BUFFER + 1;
  BOOST_CHECK_THROW(value = readNonNegativeInteger(2, begin, begin + 1), Error);
  BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(2, begin, begin + 2));
  BOOST_CHECK_EQUAL(value, 257);

  begin = BUFFER + 3;
  BOOST_CHECK_THROW(value = readNonNegativeInteger(4, begin, begin + 3), Error);
  BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(4, begin, begin + 4));
  BOOST_CHECK_EQUAL(value, 16843009LL);

  begin = BUFFER + 7;
  BOOST_CHECK_THROW(value = readNonNegativeInteger(8, begin, begin + 7), Error);
  BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(8, begin, begin + 8));
  BOOST_CHECK_EQUAL(value, 72340172838076673LL);

  begin = BUFFER;
  BOOST_CHECK_THROW(value = readNonNegativeInteger(3, begin, begin + 3), Error);
}

BOOST_AUTO_TEST_CASE(ReadFromStream)
{
  typedef boost::iostreams::stream<boost::iostreams::array_source> ArrayStream;
  typedef std::istream_iterator<uint8_t> Iterator;

  Iterator end; // end of stream
  uint64_t value;
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 0);
    Iterator begin(stream);
    BOOST_CHECK_THROW(value = readNonNegativeInteger(1, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 1);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(1, begin, end));
    BOOST_CHECK_EQUAL(value, 1);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER + 1), 1);
    Iterator begin(stream);
    BOOST_CHECK_THROW(value = readNonNegativeInteger(2, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER + 1), 2);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(2, begin, end));
    BOOST_CHECK_EQUAL(value, 257);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER + 3), 3);
    Iterator begin(stream);
    BOOST_CHECK_THROW(value = readNonNegativeInteger(4, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER + 3), 4);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(4, begin, end));
    BOOST_CHECK_EQUAL(value, 16843009LL);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER + 7), 7);
    Iterator begin(stream);
    BOOST_CHECK_THROW(value = readNonNegativeInteger(8, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER + 7), 8);
    Iterator begin(stream);
    BOOST_CHECK_NO_THROW(value = readNonNegativeInteger(8, begin, end));
    BOOST_CHECK_EQUAL(value, 72340172838076673LL);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 3);
    Iterator begin(stream);
    BOOST_CHECK_THROW(value = readNonNegativeInteger(3, begin, end), Error);
  }
}

BOOST_AUTO_TEST_SUITE_END() // NonNegativeInteger

BOOST_AUTO_TEST_SUITE_END() // EncodingTlv

} // namespace tests
} // namespace tlv
} // namespace ndn
