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
 *
 * @author Eric Newberry <enewberry@email.arizona.edu>
 */

#include "lp/nack-header.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace lp {
namespace tests {

BOOST_AUTO_TEST_SUITE(LpNackHeader)

BOOST_AUTO_TEST_CASE(Encode)
{
  NackHeader header;
  header.setReason(NackReason::DUPLICATE);

  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = header.wireEncode());

  // Sample encoded value obtained with:
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }

  // Contains NackReason::DUPLICATE
  static const uint8_t expectedBlock[] = {
    0xfd, 0x03, 0x20, 0x05, 0xfd, 0x03, 0x21, 0x01, 0x64,
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(header.wireDecode(wire));
}

BOOST_AUTO_TEST_CASE(DecodeUnknownReasonCode)
{
  static const uint8_t expectedBlock[] = {
    0xfd, 0x03, 0x20, 0x08, 0xfd, 0x03, 0x21, 0x04, 0xff, 0xff, 0xff, 0xff,
  };

  NackHeader header;
  Block wire(expectedBlock, sizeof(expectedBlock));
  BOOST_REQUIRE_NO_THROW(header.wireDecode(wire));
  Block wireEncoded;
  BOOST_REQUIRE_NO_THROW(wireEncoded = header.wireEncode());
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                wireEncoded.begin(), wireEncoded.end());
  BOOST_CHECK_EQUAL(header.getReason(), NackReason::NONE);
}

BOOST_AUTO_TEST_CASE(DecodeOmitReason)
{
  static const uint8_t expectedBlock[] = {
    0xfd, 0x03, 0x20, 0x00,
  };

  NackHeader header;
  Block wire(expectedBlock, sizeof(expectedBlock));
  BOOST_REQUIRE_NO_THROW(header.wireDecode(wire));
  Block wireEncoded;
  BOOST_REQUIRE_NO_THROW(wireEncoded = header.wireEncode());
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedBlock, expectedBlock + sizeof(expectedBlock),
                                wireEncoded.begin(), wireEncoded.end());
  BOOST_CHECK_EQUAL(header.getReason(), NackReason::NONE);
}

BOOST_AUTO_TEST_CASE(Reason)
{
  NackHeader header;
  BOOST_CHECK_EQUAL(header.getReason(), NackReason::NONE);

  header.setReason(NackReason::DUPLICATE);
  BOOST_CHECK_EQUAL(header.getReason(), NackReason::DUPLICATE);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace lp
} // namespace ndn