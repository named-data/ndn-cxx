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

#include "selectors.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestSelectors)

BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
  Selectors s;
  BOOST_CHECK(s.empty());
  BOOST_CHECK_EQUAL(s.getMinSuffixComponents(), -1);
  BOOST_CHECK_EQUAL(s.getMaxSuffixComponents(), -1);
  BOOST_CHECK(s.getPublisherPublicKeyLocator().empty());
  BOOST_CHECK(s.getExclude().empty());
  BOOST_CHECK_EQUAL(s.getChildSelector(), 0);
  BOOST_CHECK_EQUAL(s.getMustBeFresh(), false);
}

BOOST_AUTO_TEST_CASE(EncodeDecodeEmpty)
{
  const uint8_t WIRE[] = {
    0x09, 0x00 // Selectors
  };

  Selectors s1;
  Block wire1 = s1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Selectors s2(wire1);
  BOOST_CHECK(s2.empty());
  BOOST_CHECK_EQUAL(s2.getMinSuffixComponents(), -1);
  BOOST_CHECK_EQUAL(s2.getMaxSuffixComponents(), -1);
  BOOST_CHECK(s2.getPublisherPublicKeyLocator().empty());
  BOOST_CHECK(s2.getExclude().empty());
  BOOST_CHECK_EQUAL(s2.getChildSelector(), 0);
  BOOST_CHECK_EQUAL(s2.getMustBeFresh(), false);

  BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(EncodeDecodeFull)
{
  const uint8_t WIRE[] = {
    0x09, 0x39, // Selectors
          0x0d, 0x01, 0x02, // MinSuffixComponents
          0x0e, 0x01, 0x06,  // MaxSuffixComponent
          0x1c, 0x16, // KeyLocator
                0x07, 0x14, // Name
                      0x08, 0x04, 0x74, 0x65, 0x73, 0x74,
                      0x08, 0x03, 0x6b, 0x65, 0x79,
                      0x08, 0x07, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
          0x10, 0x14, // Exclude
                0x08, 0x04, 0x61, 0x6c, 0x65, 0x78, // GenericNameComponent
                0x08, 0x04, 0x78, 0x78, 0x78, 0x78, // GenericNameComponent
                0x13, 0x00, // Any
                0x08, 0x04, 0x79, 0x79, 0x79, 0x79, // GenericNameComponent
          0x11, 0x01, 0x01, // ChildSelector
          0x12, 0x00 // MustBeFresh
  };

  Selectors s1;
  s1.setMinSuffixComponents(2);
  s1.setMaxSuffixComponents(6);
  s1.setPublisherPublicKeyLocator(KeyLocator("/test/key/locator"));
  s1.setExclude(Exclude().excludeOne(name::Component("alex"))
                .excludeRange(name::Component("xxxx"), name::Component("yyyy")));
  s1.setChildSelector(1);
  s1.setMustBeFresh(true);

  Block wire1 = s1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Selectors s2(wire1);
  BOOST_CHECK(!s2.empty());
  BOOST_CHECK_EQUAL(s2.getMinSuffixComponents(), 2);
  BOOST_CHECK_EQUAL(s2.getMaxSuffixComponents(), 6);
  BOOST_CHECK_EQUAL(s2.getPublisherPublicKeyLocator().getType(), KeyLocator::KeyLocator_Name);
  BOOST_CHECK_EQUAL(s2.getPublisherPublicKeyLocator().getName(), "ndn:/test/key/locator");
  BOOST_CHECK_EQUAL(s2.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_CHECK_EQUAL(s2.getChildSelector(), 1);
  BOOST_CHECK_EQUAL(s2.getMustBeFresh(), true);

  BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(SetChildSelector)
{
  Selectors s;
  BOOST_CHECK_EQUAL(s.getChildSelector(), 0);
  BOOST_CHECK_THROW(s.setChildSelector(-1), std::invalid_argument);
  BOOST_CHECK_THROW(s.setChildSelector(2), std::invalid_argument);
  s.setChildSelector(1);
  BOOST_CHECK_EQUAL(s.getChildSelector(), 1);
  s.setChildSelector(0);
  BOOST_CHECK_EQUAL(s.getChildSelector(), 0);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  // Selectors ::= SELECTORS-TYPE TLV-LENGTH
  //                 MinSuffixComponents?
  //                 MaxSuffixComponents?
  //                 PublisherPublicKeyLocator?
  //                 Exclude?
  //                 ChildSelector?
  //                 MustBeFresh?

  Selectors a;
  Selectors b;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // MinSuffixComponents
  a.setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMinSuffixComponents(2);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // MaxSuffixComponents
  a.setMaxSuffixComponents(10);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMaxSuffixComponents(10);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // PublisherPublicKeyLocator
  a.setPublisherPublicKeyLocator(KeyLocator("/key/Locator/name"));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setPublisherPublicKeyLocator(KeyLocator("/key/Locator/name"));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // Exclude
  a.setExclude(Exclude().excludeOne(name::Component("exclude")));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setExclude(Exclude().excludeOne(name::Component("exclude")));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // ChildSelector
  a.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // MustBeFresh
  a.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_SUITE_END() // TestSelectors

} // namespace tests
} // namespace ndn
