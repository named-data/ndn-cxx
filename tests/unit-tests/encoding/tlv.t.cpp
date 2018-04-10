/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include <array>
#include <deque>
#include <list>
#include <sstream>
#include <boost/concept_archetype.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tlv {
namespace tests {

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestTlv)

BOOST_AUTO_TEST_CASE(CriticalType)
{
  BOOST_CHECK_EQUAL(isCriticalType(0), true);
  BOOST_CHECK_EQUAL(isCriticalType(1), true);
  BOOST_CHECK_EQUAL(isCriticalType(2), true);
  BOOST_CHECK_EQUAL(isCriticalType(30), true);
  BOOST_CHECK_EQUAL(isCriticalType(31), true);
  BOOST_CHECK_EQUAL(isCriticalType(32), false);
  BOOST_CHECK_EQUAL(isCriticalType(33), true);
  BOOST_CHECK_EQUAL(isCriticalType(34), false);
  BOOST_CHECK_EQUAL(isCriticalType(10000), false);
  BOOST_CHECK_EQUAL(isCriticalType(10001), true);
}

using ArrayStream = boost::iostreams::stream<boost::iostreams::array_source>;
using StreamIterator = std::istream_iterator<uint8_t>;

#define ASSERT_READ_NUMBER_IS_FAST(T) \
  static_assert(std::is_base_of<detail::ReadNumberFast<T>, detail::ReadNumber<T>>::value, \
                # T " should use ReadNumberFast")
#define ASSERT_READ_NUMBER_IS_SLOW(T) \
  static_assert(std::is_base_of<detail::ReadNumberSlow<T>, detail::ReadNumber<T>>::value, \
                # T " should use ReadNumberSlow")

ASSERT_READ_NUMBER_IS_FAST(const uint8_t*);
ASSERT_READ_NUMBER_IS_FAST(uint8_t*);
ASSERT_READ_NUMBER_IS_FAST(int8_t*);
ASSERT_READ_NUMBER_IS_FAST(char*);
ASSERT_READ_NUMBER_IS_FAST(unsigned char*);
ASSERT_READ_NUMBER_IS_FAST(signed char*);
ASSERT_READ_NUMBER_IS_FAST(const uint8_t[]);
ASSERT_READ_NUMBER_IS_FAST(uint8_t[]);
ASSERT_READ_NUMBER_IS_FAST(const uint8_t[12]);
ASSERT_READ_NUMBER_IS_FAST(uint8_t[12]);
using Uint8Array = std::array<uint8_t, 87>;
ASSERT_READ_NUMBER_IS_FAST(Uint8Array::const_iterator);
ASSERT_READ_NUMBER_IS_FAST(Uint8Array::iterator);
using CharArray = std::array<char, 87>;
ASSERT_READ_NUMBER_IS_FAST(CharArray::iterator);
ASSERT_READ_NUMBER_IS_FAST(std::string::const_iterator);
ASSERT_READ_NUMBER_IS_FAST(std::string::iterator);
ASSERT_READ_NUMBER_IS_FAST(Buffer::const_iterator);
ASSERT_READ_NUMBER_IS_FAST(Buffer::iterator);
ASSERT_READ_NUMBER_IS_FAST(std::vector<uint8_t>::const_iterator);
ASSERT_READ_NUMBER_IS_FAST(std::vector<uint8_t>::iterator);
ASSERT_READ_NUMBER_IS_FAST(std::vector<int8_t>::iterator);
ASSERT_READ_NUMBER_IS_FAST(std::vector<char>::iterator);
ASSERT_READ_NUMBER_IS_FAST(std::vector<unsigned char>::iterator);
ASSERT_READ_NUMBER_IS_FAST(std::vector<signed char>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(std::vector<bool>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(std::vector<uint16_t>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(std::vector<uint32_t>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(std::vector<uint64_t>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(std::deque<uint8_t>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(std::list<uint8_t>::iterator);
ASSERT_READ_NUMBER_IS_SLOW(StreamIterator);

BOOST_AUTO_TEST_SUITE(VarNumber)

// This check ensures readVarNumber and readType only require InputIterator concept and nothing
// more. This function should compile, but should never be executed.
void
checkArchetype()
{
  boost::input_iterator_archetype<uint8_t> begin, end;
  uint64_t number = readVarNumber(begin, end);
  uint32_t type = readType(begin, end);;
  readVarNumber(begin, end, number);
  readType(begin, end, type);
}

static const uint8_t BUFFER[] = {
  0x01, // == 1
  0xfc, // == 252
  0xfd, 0x00, 0xfd, // == 253
  0xfe, 0x00, 0x01, 0x00, 0x00, // == 65536
  0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 // == 4294967296
};

BOOST_AUTO_TEST_CASE(SizeOf)
{
  BOOST_CHECK_EQUAL(sizeOfVarNumber(1), 1);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(252), 1);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(253), 3);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(65536), 5);
  BOOST_CHECK_EQUAL(sizeOfVarNumber(4294967296), 9);
}

BOOST_AUTO_TEST_CASE(Write)
{
  std::ostringstream os;

  writeVarNumber(os, 1);
  writeVarNumber(os, 252);
  writeVarNumber(os, 253);
  writeVarNumber(os, 65536);
  writeVarNumber(os, 4294967296);

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
  BOOST_CHECK_EQUAL(value, 4294967296);
}

BOOST_AUTO_TEST_CASE(ReadFromStream)
{
  StreamIterator end; // end of stream
  uint64_t value;
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 1);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 1);
    StreamIterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 1);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 1, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 1, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 252);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 2);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 2);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 3);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 2, 3);
    StreamIterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 253);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 4);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 4);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 5);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 5, 5);
    StreamIterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 65536);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 1);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 8);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), false);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 8);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readVarNumber(begin, end), Error);
  }

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 9);
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readVarNumber(begin, end, value), true);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER) + 10, 9);
    StreamIterator begin(stream);
    BOOST_CHECK_NO_THROW(readVarNumber(begin, end));
    BOOST_CHECK_EQUAL(value, 4294967296);
  }
}

BOOST_AUTO_TEST_SUITE_END() // VarNumber

BOOST_AUTO_TEST_SUITE(NonNegativeInteger)

// This check ensures readNonNegativeInteger only requires InputIterator concept and nothing more.
// This function should compile, but should never be executed.
void
checkArchetype()
{
  boost::input_iterator_archetype<uint8_t> begin, end;
  readNonNegativeInteger(0, begin, end);
}

static const uint8_t BUFFER[] = {
  0x01, // 1
  0xff, // 255
  0x01, 0x02, // 258
  0x01, 0x01, 0x01, 0x02, // 16843010
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02 // 72340172838076674
};

BOOST_AUTO_TEST_CASE(SizeOf)
{
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(1), 1);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(253), 1);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(255), 1);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(256), 2);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(65536), 4);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(16843009), 4);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(4294967296), 8);
  BOOST_CHECK_EQUAL(sizeOfNonNegativeInteger(72340172838076673), 8);
}

BOOST_AUTO_TEST_CASE(Write)
{
  std::ostringstream os;

  writeNonNegativeInteger(os, 1);
  writeNonNegativeInteger(os, 255);
  writeNonNegativeInteger(os, 258);
  writeNonNegativeInteger(os, 16843010);
  writeNonNegativeInteger(os, 72340172838076674);

  std::string buffer = os.str();
  const uint8_t* actual = reinterpret_cast<const uint8_t*>(buffer.c_str());

  BOOST_CHECK_EQUAL_COLLECTIONS(BUFFER, BUFFER + sizeof(BUFFER),
                                actual, actual + sizeof(BUFFER));
}

BOOST_AUTO_TEST_CASE(ReadFromBuffer)
{
  const uint8_t* begin = nullptr;

  begin = BUFFER;
  BOOST_CHECK_EQUAL(readNonNegativeInteger(1, begin, begin + 1), 1);
  BOOST_CHECK_EQUAL(begin, BUFFER + 1);

  begin = BUFFER + 1;
  BOOST_CHECK_EQUAL(readNonNegativeInteger(1, begin, begin + 1), 255);
  BOOST_CHECK_EQUAL(begin, BUFFER + 2);

  begin = BUFFER + 2;
  BOOST_CHECK_EQUAL(readNonNegativeInteger(2, begin, begin + 2), 258);
  BOOST_CHECK_EQUAL(begin, BUFFER + 4);

  begin = BUFFER + 4;
  BOOST_CHECK_EQUAL(readNonNegativeInteger(4, begin, begin + 4), 16843010);
  BOOST_CHECK_EQUAL(begin, BUFFER + 8);

  begin = BUFFER + 8;
  BOOST_CHECK_EQUAL(readNonNegativeInteger(8, begin, begin + 8), 72340172838076674);
  BOOST_CHECK_EQUAL(begin, BUFFER + 16);

  // invalid size
  begin = BUFFER;
  BOOST_CHECK_THROW(readNonNegativeInteger(3, begin, begin + 3), Error);

  // available buffer smaller than size
  begin = BUFFER;
  BOOST_CHECK_THROW(readNonNegativeInteger(1, begin, begin + 0), Error);
  begin = BUFFER;
  BOOST_CHECK_THROW(readNonNegativeInteger(2, begin, begin + 1), Error);
  begin = BUFFER;
  BOOST_CHECK_THROW(readNonNegativeInteger(4, begin, begin + 3), Error);
  begin = BUFFER;
  BOOST_CHECK_THROW(readNonNegativeInteger(8, begin, begin + 7), Error);
}

BOOST_AUTO_TEST_CASE(ReadFromStream)
{
  StreamIterator end; // end of stream

  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), sizeof(BUFFER));
    StreamIterator begin(stream);
    BOOST_CHECK_EQUAL(readNonNegativeInteger(1, begin, end), 1);
    BOOST_CHECK_EQUAL(readNonNegativeInteger(1, begin, end), 255);
    BOOST_CHECK_EQUAL(readNonNegativeInteger(2, begin, end), 258);
    BOOST_CHECK_EQUAL(readNonNegativeInteger(4, begin, end), 16843010);
    BOOST_CHECK_EQUAL(readNonNegativeInteger(8, begin, end), 72340172838076674);
    BOOST_CHECK(begin == end);
  }

  // invalid size
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 3);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readNonNegativeInteger(3, begin, end), Error);
  }

  // available buffer smaller than size
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 0);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readNonNegativeInteger(1, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 1);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readNonNegativeInteger(2, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 3);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readNonNegativeInteger(4, begin, end), Error);
  }
  {
    ArrayStream stream(reinterpret_cast<const char*>(BUFFER), 7);
    StreamIterator begin(stream);
    BOOST_CHECK_THROW(readNonNegativeInteger(8, begin, end), Error);
  }
}

BOOST_AUTO_TEST_SUITE_END() // NonNegativeInteger

BOOST_AUTO_TEST_SUITE(PrintHelpers)

BOOST_AUTO_TEST_CASE(PrintSignatureTypeValue)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DigestSha256), "DigestSha256");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(SignatureSha256WithRsa), "SignatureSha256WithRsa");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(SignatureSha256WithEcdsa), "SignatureSha256WithEcdsa");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<SignatureTypeValue>(200)), "Unknown Signature Type");
}

BOOST_AUTO_TEST_SUITE_END() // PrintHelpers

BOOST_AUTO_TEST_SUITE_END() // TestTlv
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace tlv
} // namespace ndn
