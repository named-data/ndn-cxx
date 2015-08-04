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

#include "security/additional-description.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityAdditionalDescription)

static const uint8_t description[] = {
  0xfd, 0x01, 0x02, 0x28,
    0xfd, 0x02, 0x00, 0x10, // DescriptionEntry
      0xfd, 0x02, 0x01, 0x04, // DescriptionKey
        0x6b, 0x65, 0x79, 0x31, // "key1"
      0xfd, 0x02, 0x02, 0x04, // DescriptionValue
        0x76, 0x61, 0x6c, 0x31, // "val1"
    0xfd, 0x02, 0x00, 0x10, // DescriptionEntry
      0xfd, 0x02, 0x01, 0x04, // DescriptionKey
        0x6b, 0x65, 0x79, 0x32, // "key2"
      0xfd, 0x02, 0x02, 0x04, // DescriptionValue
        0x76, 0x61, 0x6c, 0x32, // "val2"
};

static const std::string text = "((key1:val1), (key2:val2))";

BOOST_AUTO_TEST_CASE(Basic)
{
  AdditionalDescription aDescription;

  aDescription.set("key2", "val2");
  aDescription.set("key1", "val1");

  BOOST_REQUIRE_NO_THROW(aDescription.get("key1"));
  BOOST_REQUIRE_NO_THROW(aDescription.get("key2"));
  BOOST_REQUIRE_THROW(aDescription.get("key3"), AdditionalDescription::Error);

  BOOST_CHECK_EQUAL(aDescription.has("key1"), true);
  BOOST_CHECK_EQUAL(aDescription.has("key2"), true);
  BOOST_CHECK_EQUAL(aDescription.has("key3"), false);

  auto val1 = aDescription.get("key1");
  auto val2 = aDescription.get("key2");

  BOOST_CHECK_EQUAL(val1, "val1");
  BOOST_CHECK_EQUAL(val2, "val2");

  auto it = aDescription.begin();
  BOOST_CHECK_EQUAL(it->second, "val1");
  it++;
  BOOST_CHECK_EQUAL(it->second, "val2");
  it++;
  BOOST_CHECK(it == aDescription.end());

  BOOST_CHECK_EQUAL_COLLECTIONS(aDescription.wireEncode().wire(),
                                aDescription.wireEncode().wire() + aDescription.wireEncode().size(),
                                description,
                                description + sizeof(description));

  BOOST_REQUIRE_NO_THROW(AdditionalDescription(Block(description, sizeof(description))));
  AdditionalDescription aDescription2(Block(description, sizeof(description)));

  BOOST_CHECK(aDescription2 == aDescription);

  AdditionalDescription aDescription3;
  aDescription3.set("key3", "val3");
  aDescription3.set("key2", "val2");

  BOOST_CHECK(aDescription2 != aDescription3);

  std::ostringstream os;
  os << aDescription;
  BOOST_CHECK_EQUAL(os.str(), text);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
