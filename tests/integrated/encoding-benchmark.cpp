/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#define BOOST_TEST_MAIN 1
#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE ndn-cxx Encoding Benchmark

#include "encoding/tlv.hpp"

#include "boost-test.hpp"
#include "timed-execute.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>

#include <iostream>

namespace ndn {
namespace tlv {
namespace tests {

using namespace ndn::tests;

template<size_t WIRE_SIZE>
struct ReadVarNumberTest;

template<>
struct ReadVarNumberTest<1>
{
  static const uint8_t WIRE[];
  static const uint64_t VALUE = 252;
};
const uint8_t ReadVarNumberTest<1>::WIRE[] = {0xfc};

template<>
struct ReadVarNumberTest<3>
{
  static const uint8_t WIRE[];
  static const uint64_t VALUE = 253;
};
const uint8_t ReadVarNumberTest<3>::WIRE[] = {0xfd, 0x00, 0xfd};

template<>
struct ReadVarNumberTest<5>
{
  static const uint8_t WIRE[];
  static const uint64_t VALUE = 65536;
};
const uint8_t ReadVarNumberTest<5>::WIRE[] = {0xfe, 0x00, 0x01, 0x00, 0x00};

template<>
struct ReadVarNumberTest<9>
{
  static const uint8_t WIRE[];
  static const uint64_t VALUE = 4294967296;
};
const uint8_t ReadVarNumberTest<9>::WIRE[] = {0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

template<size_t WIRE_SIZE, size_t ALIGNMENT_OFFSET>
struct ReadVarNumberAlignTest : public ReadVarNumberTest<WIRE_SIZE>
{
  using AlignmentOffset = std::integral_constant<size_t, ALIGNMENT_OFFSET>;

  static_assert(sizeof(ReadVarNumberTest<WIRE_SIZE>::WIRE) == WIRE_SIZE, "");
};

using ReadVarNumberTests = boost::mpl::vector<
  ReadVarNumberAlignTest<1, 0>,
  ReadVarNumberAlignTest<3, 0>,
  ReadVarNumberAlignTest<3, 1>,
  ReadVarNumberAlignTest<5, 0>,
  ReadVarNumberAlignTest<5, 1>,
  ReadVarNumberAlignTest<5, 2>,
  ReadVarNumberAlignTest<5, 3>,
  ReadVarNumberAlignTest<9, 0>,
  ReadVarNumberAlignTest<9, 1>,
  ReadVarNumberAlignTest<9, 2>,
  ReadVarNumberAlignTest<9, 3>,
  ReadVarNumberAlignTest<9, 4>,
  ReadVarNumberAlignTest<9, 5>,
  ReadVarNumberAlignTest<9, 6>,
  ReadVarNumberAlignTest<9, 7>
>;

// Benchmark of ndn::tlv::readVarNumber with different number lengths and alignments.
// Run this benchmark with:
//    ./encoding-benchmark -t 'ReadVarNumber*'
// For accurate results, it is required to compile ndn-cxx in release mode.
// It is recommended to run the benchmark multiple times and take the average.
BOOST_AUTO_TEST_CASE_TEMPLATE(ReadVarNumber, Test, ReadVarNumberTests)
{
  const int N_ITERATIONS = 100000000;

  alignas(8) uint8_t buffer[16];
  static_assert(Test::AlignmentOffset::value + sizeof(Test::WIRE) <= sizeof(buffer), "");
  uint8_t* const begin = buffer + Test::AlignmentOffset::value;
  std::memcpy(begin, Test::WIRE, sizeof(Test::WIRE));
  const uint8_t* const end = begin + sizeof(Test::WIRE);

  int nOks = 0;
  int nCorrects = 0;
  auto d = timedExecute([&] {
    uint64_t number = 0;
    for (int i = 0; i < N_ITERATIONS; ++i) {
      const uint8_t* begin2 = begin; // make a copy because readVarNumber increments the pointer
      nOks += readVarNumber(begin2, end, number);
      nCorrects += number == Test::VALUE;
      // use the number and the return value, so compiler won't optimize out their computation
    }
  });
  BOOST_CHECK_EQUAL(nOks, N_ITERATIONS);
  BOOST_CHECK_EQUAL(nCorrects, N_ITERATIONS);
  std::cout << "size=" << sizeof(Test::WIRE)
            << " offset=" << Test::AlignmentOffset::value
            << " " << d << std::endl;
}

} // namespace tests
} // namespace tlv
} // namespace ndn
