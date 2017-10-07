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

#include "security/transform/strip-space.hpp"
#include "security/transform/step-source.hpp"
#include "security/transform/stream-sink.hpp"
#include "encoding/buffer-stream.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestStripSpace)

BOOST_AUTO_TEST_CASE(Basic)
{
  const char* input = R"STR(
    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor
    incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud
    exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute
    irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla
    pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia
    deserunt mollit anim id est laborum.
  )STR";
  size_t inputLen = strlen(input);

  OBufferStream os;
  StepSource source;
  source >> stripSpace() >> streamSink(os);

  for (size_t offset = 0; offset < inputLen; offset += 40) {
    source.write(reinterpret_cast<const uint8_t*>(input + offset),
                 std::min<size_t>(40, inputLen - offset));
  }
  source.end();

  std::string expected(
    "Loremipsumdolorsitamet,consecteturadipiscingelit,seddoeiusmodtemporincididuntutl"
    "aboreetdoloremagnaaliqua.Utenimadminimveniam,quisnostrudexercitationullamcolabor"
    "isnisiutaliquipexeacommodoconsequat.Duisauteiruredolorinreprehenderitinvoluptate"
    "velitessecillumdoloreeufugiatnullapariatur.Excepteursintoccaecatcupidatatnonproi"
    "dent,suntinculpaquiofficiadeseruntmollitanimidestlaborum.");
  ConstBufferPtr buf = os.buf();
  BOOST_CHECK_EQUAL(std::string(buf->get<char>(), buf->size()), expected);
}

BOOST_AUTO_TEST_SUITE_END() // TestStripSpace
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
