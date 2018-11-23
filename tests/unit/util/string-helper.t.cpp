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

#include "util/string-helper.hpp"
#include "encoding/buffer.hpp"

#include "boost-test.hpp"

#include <cctype>
#include <cstring>

namespace ndn {
namespace util {
namespace test {

using boost::test_tools::output_test_stream;

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestStringHelper)

BOOST_AUTO_TEST_CASE(PrintHex)
{
  output_test_stream os;

  printHex(os, 0);
  BOOST_CHECK(os.is_equal("0x0"));

  printHex(os, 42);
  BOOST_CHECK(os.is_equal("0x2a"));

  printHex(os, 2748, true);
  BOOST_CHECK(os.is_equal("0xABC"));

  printHex(os, static_cast<uint64_t>(-1));
  BOOST_CHECK(os.is_equal("0xffffffffffffffff"));

  printHex(os, ~0U, true);
  BOOST_CHECK(os.is_equal("0xFFFFFFFF"));

  printHex(os, ~0ULL, true);
  BOOST_CHECK(os.is_equal("0xFFFFFFFFFFFFFFFF"));
}

BOOST_AUTO_TEST_CASE(AsHex)
{
  using ndn::AsHex;
  output_test_stream os;

  os << AsHex{0};
  BOOST_CHECK(os.is_equal("0x0"));

  os << AsHex{42};
  BOOST_CHECK(os.is_equal("0x2a"));

  os << std::uppercase << AsHex{~0U};
  BOOST_CHECK(os.is_equal("0xFFFFFFFF"));

  os << std::nouppercase << AsHex{~0U};
  BOOST_CHECK(os.is_equal("0xffffffff"));
}

BOOST_AUTO_TEST_CASE(ToHex)
{
  std::string test = "Hello, world!";
  BOOST_CHECK_EQUAL(toHex(reinterpret_cast<const uint8_t*>(test.data()), test.size()),
                    "48656C6C6F2C20776F726C6421");
  BOOST_CHECK_EQUAL(toHex(reinterpret_cast<const uint8_t*>(test.data()), test.size(), false),
                    "48656c6c6f2c20776f726c6421");
  BOOST_CHECK_EQUAL(toHex(nullptr, 0), "");

  Buffer buffer(test.data(), test.size());
  BOOST_CHECK_EQUAL(toHex(buffer, false),  "48656c6c6f2c20776f726c6421");
  BOOST_CHECK_EQUAL(toHex(Buffer{}), "");
}

BOOST_AUTO_TEST_CASE(FromHex)
{
  BOOST_CHECK(*fromHex("") == Buffer{});
  BOOST_CHECK(*fromHex("48656c6c6f2c20776f726c6421") ==
              (std::vector<uint8_t>{0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20,
                                    0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21}));
  BOOST_CHECK(*fromHex("012a3Bc4defAB5CdEF") ==
              (std::vector<uint8_t>{0x01, 0x2a, 0x3b, 0xc4, 0xde,
                                    0xfa, 0xb5, 0xcd, 0xef}));

  BOOST_CHECK_THROW(fromHex("1"), StringHelperError);
  BOOST_CHECK_THROW(fromHex("zz"), StringHelperError);
  BOOST_CHECK_THROW(fromHex("00az"), StringHelperError);
  BOOST_CHECK_THROW(fromHex("1234z"), StringHelperError);
}

BOOST_AUTO_TEST_CASE(ToHexChar)
{
  static const std::vector<std::pair<unsigned int, char>> hexMap{
    {0, '0'}, {1, '1'},  {2, '2'},  {3, '3'},  {4, '4'},  {5, '5'},  {6, '6'},  {7, '7'},
    {8, '8'}, {9, '9'}, {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'}
  };

  for (const auto& i : hexMap) {
    BOOST_CHECK_EQUAL(toHexChar(i.first), i.second);
    BOOST_CHECK_EQUAL(toHexChar(i.first + 16), i.second);
    BOOST_CHECK_EQUAL(toHexChar(i.first + 32), i.second);
    BOOST_CHECK_EQUAL(toHexChar(i.first + 240), i.second);
    BOOST_CHECK_EQUAL(toHexChar(i.first, false), std::tolower(static_cast<unsigned char>(i.second)));
  }
}

BOOST_AUTO_TEST_CASE(FromHexChar)
{
  // for (int ch = 0; ch <= std::numeric_limits<uint8_t>::max(); ++ch) {
  //   std::cout << "{0x" << std::hex << ch << ", "
  //             << std::dec << fromHexChar(static_cast<char>(ch)) << "}, ";
  //   if (ch % 8 == 7)
  //     std::cout << std::endl;
  // }
  static const std::vector<std::pair<char, int>> hexMap{
    {0x0, -1}, {0x1, -1}, {0x2, -1}, {0x3, -1}, {0x4, -1}, {0x5, -1}, {0x6, -1}, {0x7, -1},
    {0x8, -1}, {0x9, -1}, {0xa, -1}, {0xb, -1}, {0xc, -1}, {0xd, -1}, {0xe, -1}, {0xf, -1},
    {0x10, -1}, {0x11, -1}, {0x12, -1}, {0x13, -1}, {0x14, -1}, {0x15, -1}, {0x16, -1}, {0x17, -1},
    {0x18, -1}, {0x19, -1}, {0x1a, -1}, {0x1b, -1}, {0x1c, -1}, {0x1d, -1}, {0x1e, -1}, {0x1f, -1},
    {0x20, -1}, {0x21, -1}, {0x22, -1}, {0x23, -1}, {0x24, -1}, {0x25, -1}, {0x26, -1}, {0x27, -1},
    {0x28, -1}, {0x29, -1}, {0x2a, -1}, {0x2b, -1}, {0x2c, -1}, {0x2d, -1}, {0x2e, -1}, {0x2f, -1},
    {0x30, 0}, {0x31, 1}, {0x32, 2}, {0x33, 3}, {0x34, 4}, {0x35, 5}, {0x36, 6}, {0x37, 7},
    {0x38, 8}, {0x39, 9}, {0x3a, -1}, {0x3b, -1}, {0x3c, -1}, {0x3d, -1}, {0x3e, -1}, {0x3f, -1},
    {0x40, -1}, {0x41, 10}, {0x42, 11}, {0x43, 12}, {0x44, 13}, {0x45, 14}, {0x46, 15}, {0x47, -1},
    {0x48, -1}, {0x49, -1}, {0x4a, -1}, {0x4b, -1}, {0x4c, -1}, {0x4d, -1}, {0x4e, -1}, {0x4f, -1},
    {0x50, -1}, {0x51, -1}, {0x52, -1}, {0x53, -1}, {0x54, -1}, {0x55, -1}, {0x56, -1}, {0x57, -1},
    {0x58, -1}, {0x59, -1}, {0x5a, -1}, {0x5b, -1}, {0x5c, -1}, {0x5d, -1}, {0x5e, -1}, {0x5f, -1},
    {0x60, -1}, {0x61, 10}, {0x62, 11}, {0x63, 12}, {0x64, 13}, {0x65, 14}, {0x66, 15}, {0x67, -1},
    {0x68, -1}, {0x69, -1}, {0x6a, -1}, {0x6b, -1}, {0x6c, -1}, {0x6d, -1}, {0x6e, -1}, {0x6f, -1},
    {0x70, -1}, {0x71, -1}, {0x72, -1}, {0x73, -1}, {0x74, -1}, {0x75, -1}, {0x76, -1}, {0x77, -1},
    {0x78, -1}, {0x79, -1}, {0x7a, -1}, {0x7b, -1}, {0x7c, -1}, {0x7d, -1}, {0x7e, -1}, {0x7f, -1},
    {0x80, -1}, {0x81, -1}, {0x82, -1}, {0x83, -1}, {0x84, -1}, {0x85, -1}, {0x86, -1}, {0x87, -1},
    {0x88, -1}, {0x89, -1}, {0x8a, -1}, {0x8b, -1}, {0x8c, -1}, {0x8d, -1}, {0x8e, -1}, {0x8f, -1},
    {0x90, -1}, {0x91, -1}, {0x92, -1}, {0x93, -1}, {0x94, -1}, {0x95, -1}, {0x96, -1}, {0x97, -1},
    {0x98, -1}, {0x99, -1}, {0x9a, -1}, {0x9b, -1}, {0x9c, -1}, {0x9d, -1}, {0x9e, -1}, {0x9f, -1},
    {0xa0, -1}, {0xa1, -1}, {0xa2, -1}, {0xa3, -1}, {0xa4, -1}, {0xa5, -1}, {0xa6, -1}, {0xa7, -1},
    {0xa8, -1}, {0xa9, -1}, {0xaa, -1}, {0xab, -1}, {0xac, -1}, {0xad, -1}, {0xae, -1}, {0xaf, -1},
    {0xb0, -1}, {0xb1, -1}, {0xb2, -1}, {0xb3, -1}, {0xb4, -1}, {0xb5, -1}, {0xb6, -1}, {0xb7, -1},
    {0xb8, -1}, {0xb9, -1}, {0xba, -1}, {0xbb, -1}, {0xbc, -1}, {0xbd, -1}, {0xbe, -1}, {0xbf, -1},
    {0xc0, -1}, {0xc1, -1}, {0xc2, -1}, {0xc3, -1}, {0xc4, -1}, {0xc5, -1}, {0xc6, -1}, {0xc7, -1},
    {0xc8, -1}, {0xc9, -1}, {0xca, -1}, {0xcb, -1}, {0xcc, -1}, {0xcd, -1}, {0xce, -1}, {0xcf, -1},
    {0xd0, -1}, {0xd1, -1}, {0xd2, -1}, {0xd3, -1}, {0xd4, -1}, {0xd5, -1}, {0xd6, -1}, {0xd7, -1},
    {0xd8, -1}, {0xd9, -1}, {0xda, -1}, {0xdb, -1}, {0xdc, -1}, {0xdd, -1}, {0xde, -1}, {0xdf, -1},
    {0xe0, -1}, {0xe1, -1}, {0xe2, -1}, {0xe3, -1}, {0xe4, -1}, {0xe5, -1}, {0xe6, -1}, {0xe7, -1},
    {0xe8, -1}, {0xe9, -1}, {0xea, -1}, {0xeb, -1}, {0xec, -1}, {0xed, -1}, {0xee, -1}, {0xef, -1},
    {0xf0, -1}, {0xf1, -1}, {0xf2, -1}, {0xf3, -1}, {0xf4, -1}, {0xf5, -1}, {0xf6, -1}, {0xf7, -1},
    {0xf8, -1}, {0xf9, -1}, {0xfa, -1}, {0xfb, -1}, {0xfc, -1}, {0xfd, -1}, {0xfe, -1}, {0xff, -1}
  };

  for (const auto& item : hexMap) {
    BOOST_CHECK_EQUAL(fromHexChar(item.first), item.second);
  }
}

BOOST_AUTO_TEST_CASE(Escape)
{
  BOOST_CHECK_EQUAL(escape(""), "");
  BOOST_CHECK_EQUAL(escape("foo42"), "foo42");
  BOOST_CHECK_EQUAL(escape("foo%bar"), "foo%25bar");
  BOOST_CHECK_EQUAL(escape("lower UPPER"), "lower%20UPPER");
  BOOST_CHECK_EQUAL(escape("-._~"), "-._~");
  BOOST_CHECK_EQUAL(escape(":/?#[]@"), "%3A%2F%3F%23%5B%5D%40");

  output_test_stream os;
  const char str[] = "\x01\x2a\x3b\xc4\xde\xfa\xb5\xcd\xef";
  escape(os, str, std::strlen(str));
  BOOST_CHECK(os.is_equal("%01%2A%3B%C4%DE%FA%B5%CD%EF"));
}

BOOST_AUTO_TEST_CASE(Unescape)
{
  BOOST_CHECK_EQUAL(unescape(""), "");
  BOOST_CHECK_EQUAL(unescape("Hello%01, world!%AA  "), "Hello\x01, world!\xAA  ");
  BOOST_CHECK_EQUAL(unescape("Bad %ZZ (not a hex value)"), "Bad %ZZ (not a hex value)");
  BOOST_CHECK_EQUAL(unescape("Bad %a (should be two hex chars)"), "Bad %a (should be two hex chars)");
  BOOST_CHECK_EQUAL(unescape("Bad %a"), "Bad %a");

  output_test_stream os;
  const char str[] = "%01%2a%3B%c4%de%fA%B5%Cd%EF";
  unescape(os, str, std::strlen(str));
  BOOST_CHECK(os.is_equal("\x01\x2a\x3b\xc4\xde\xfa\xb5\xcd\xef"));
}

BOOST_AUTO_TEST_SUITE_END() // TestStringHelper
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace test
} // namespace util
} // namespace ndn
