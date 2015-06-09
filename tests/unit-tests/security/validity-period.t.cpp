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

#include "security/validity-period.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace test {

BOOST_AUTO_TEST_SUITE(SecurityValidityPeriod)

BOOST_AUTO_TEST_CASE(ConstructorSetter)
{
  time::system_clock::TimePoint notBefore = time::system_clock::now() - time::days(1);
  time::system_clock::TimePoint notAfter = notBefore + time::days(2);

  ValidityPeriod validity1 = ValidityPeriod(notBefore, notAfter);

  auto period = validity1.getPeriod();
  BOOST_CHECK_GE(period.first, notBefore); // fractional seconds will be removed
  BOOST_CHECK_LT(period.first, notBefore + time::seconds(1));

  BOOST_CHECK_LE(period.second, notAfter); // fractional seconds will be removed
  BOOST_CHECK_GT(period.second, notAfter - time::seconds(1));
  BOOST_CHECK_EQUAL(validity1.isValid(), true);

  BOOST_CHECK_EQUAL(ValidityPeriod(time::system_clock::now() - time::days(2),
                                   time::system_clock::now() - time::days(1)).isValid(),
                    false);

  BOOST_CHECK_NO_THROW((ValidityPeriod()));
  ValidityPeriod validity2;
  BOOST_CHECK(validity2.getPeriod() == std::make_pair(time::getUnixEpoch(), time::getUnixEpoch()));

  validity2.setPeriod(notBefore, notAfter);
  BOOST_CHECK(validity2.getPeriod() != std::make_pair(time::getUnixEpoch(), time::getUnixEpoch()));
  BOOST_CHECK_EQUAL(validity2, validity1);

  validity1.setPeriod(time::getUnixEpoch(), time::getUnixEpoch() + time::days(10 * 365));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(validity1),
                    "(19700101T000000, 19791230T000000)");

  validity1.setPeriod(time::getUnixEpoch() + time::nanoseconds(1),
                      time::getUnixEpoch() + time::days(10 * 365) + time::nanoseconds(1));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(validity1),
                    "(19700101T000001, 19791230T000000)");
}

const uint8_t VP1[] = {
  0xfd, 0x00, 0xfd, 0x26, // ValidityPeriod
    0xfd, 0x00, 0xfe, 0x0f, // NotBefore
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0xfd, 0x00, 0xff, 0x0f, // NotAfter
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};

BOOST_AUTO_TEST_CASE(EncodingDecoding)
{
  time::system_clock::TimePoint notBefore = time::getUnixEpoch();
  time::system_clock::TimePoint notAfter = notBefore + time::days(1);

  ValidityPeriod v1(notBefore, notAfter);

  BOOST_CHECK_EQUAL_COLLECTIONS(v1.wireEncode().begin(), v1.wireEncode().end(),
                                VP1, VP1 + sizeof(VP1));

  BOOST_REQUIRE_NO_THROW(ValidityPeriod(Block(VP1, sizeof(VP1))));
  Block block(VP1, sizeof(VP1));
  ValidityPeriod v2(block);
  BOOST_CHECK(v1.getPeriod() == v2.getPeriod());
}

const uint8_t VP_E1[] = {
  0xfd, 0x00, 0xff, 0x26, // ValidityPeriod (error)
    0xfd, 0x00, 0xfe, 0x0f, // NotBefore
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0xfd, 0x00, 0xff, 0x0f, // NotAfter
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};

const uint8_t VP_E2[] = {
  0xfd, 0x00, 0xfd, 0x26, // ValidityPeriod
    0xfd, 0x00, 0xff, 0x0f, // NotBefore (error)
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0xfd, 0x00, 0xff, 0x0f, // NotAfter
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};

const uint8_t VP_E3[] = {
  0xfd, 0x00, 0xfd, 0x26, // ValidityPeriod
    0xfd, 0x00, 0xfe, 0x0f, // NotBefore
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0xfd, 0x00, 0xfe, 0x0f, // NotAfter (error)
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};

const uint8_t VP_E4[] = {
  0xfd, 0x00, 0xfd, 0x39, // ValidityPeriod
    0xfd, 0x00, 0xfe, 0x0f, // NotBefore
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0xfd, 0x00, 0xff, 0x0f, // NotAfter
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0xfd, 0x00, 0xff, 0x0f, // NotAfter (error)
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};

const uint8_t VP_E5[] = {
  0xfd, 0x00, 0xfd, 0x13, // ValidityPeriod
    0xfd, 0x00, 0xfe, 0x0f, // NotBefore
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};

const uint8_t VP_E6[] = {
  0xfd, 0x00, 0xfd, 0x26, // ValidityPeriod
    0xfd, 0x00, 0xfe, 0x0f, // NotBefore
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, // 19700101T00000\xFF
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0xFF,
    0xfd, 0x00, 0xff, 0x0f, // NotAfter
      0x31, 0x39, 0x37, 0x30, 0x30, 0x31, 0x30, 0x32, // 19700102T000000
      0x54, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};


BOOST_AUTO_TEST_CASE(DecodingError)
{
  BOOST_CHECK_THROW(ValidityPeriod(Block(VP_E1, sizeof(VP_E1))), ValidityPeriod::Error);

  BOOST_CHECK_THROW(ValidityPeriod(Block(VP_E2, sizeof(VP_E2))), ValidityPeriod::Error);
  BOOST_CHECK_THROW(ValidityPeriod(Block(VP_E3, sizeof(VP_E3))), ValidityPeriod::Error);

  BOOST_CHECK_THROW(ValidityPeriod(Block(VP_E4, sizeof(VP_E4))), ValidityPeriod::Error);
  BOOST_CHECK_THROW(ValidityPeriod(Block(VP_E5, sizeof(VP_E5))), ValidityPeriod::Error);

  Block emptyBlock;
  BOOST_CHECK_THROW((ValidityPeriod(emptyBlock)), ValidityPeriod::Error);

  BOOST_CHECK_THROW(ValidityPeriod(Block(VP_E6, sizeof(VP_E6))), ValidityPeriod::Error);
}

BOOST_AUTO_TEST_CASE(Comparison)
{
  time::system_clock::TimePoint notBefore = time::getUnixEpoch();
  time::system_clock::TimePoint notAfter = notBefore + time::days(1);
  time::system_clock::TimePoint notAfter2 = notBefore + time::days(2);

  ValidityPeriod validity1(notBefore, notAfter);
  ValidityPeriod validity2(notBefore, notAfter);
  BOOST_CHECK(validity1 == validity2);

  ValidityPeriod validity3(notBefore, notAfter2);
  BOOST_CHECK(validity1 != validity3);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace security
} // namespace ndn
