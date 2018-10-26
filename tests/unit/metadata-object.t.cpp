/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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
 * @author Chavoosh Ghasemi <chghasemi@cs.arizona.edu>
 */

#include "ndn-cxx/metadata-object.hpp"

#include "tests/boost-test.hpp"
#include "tests/identity-management-fixture.hpp"

namespace ndn {
namespace tests {

class MetadataObjectFixture : public IdentityManagementFixture
{
public:
  MetadataObjectFixture()
    : metadataComponent(32, reinterpret_cast<const uint8_t*>("metadata"), std::strlen("metadata"))
    , versionedContentName(Name(baseContentName)
                           .appendVersion(342092199154ULL))
    , metadataFullName(Name(baseContentName)
                       .append(metadataComponent)
                       .appendVersion(metadataVerNo)
                       .appendSegment(0))
  {
  }

protected:
  const name::Component metadataComponent;

  // content prefix
  const Name baseContentName = "/ndn/unit/tests";
  const Name versionedContentName;

  // metadata prefix
  const uint64_t metadataVerNo = 89400192181ULL;
  const Name metadataFullName;
};

BOOST_FIXTURE_TEST_SUITE(TestMetadataObject, MetadataObjectFixture)

BOOST_AUTO_TEST_CASE(EncodeDecode)
{
  MetadataObject metadata1;
  metadata1.setVersionedName(versionedContentName);

  // pass metadata version number
  const Data data1 = metadata1.makeData(metadataFullName.getPrefix(-2), m_keyChain,
                                        KeyChain::getDefaultSigningInfo(), metadataVerNo);

  BOOST_CHECK_EQUAL(metadata1.getVersionedName(), versionedContentName);
  BOOST_CHECK_EQUAL(data1.getName(), metadataFullName);
  BOOST_CHECK_EQUAL(data1.getFreshnessPeriod(), 10_ms);

  // do not pass metadata version number
  metadata1.setVersionedName(versionedContentName);
  const Data data2 = metadata1.makeData(metadataFullName.getPrefix(-2), m_keyChain);
  BOOST_CHECK_NE(data2.getName()[-2].toVersion(), metadataVerNo);

  // construct a metadata object based on a valid metadata packet
  MetadataObject metadata2(data1);

  BOOST_CHECK_EQUAL(metadata2.getVersionedName(), versionedContentName);
  BOOST_CHECK(baseContentName.isPrefixOf(metadata2.makeData(metadataFullName.getPrefix(-2),
                                                            m_keyChain).getName()));
}

BOOST_AUTO_TEST_CASE(InvalidFormat)
{
  Data data;

  // invalid content type
  data.setName(Name("/ndn/unit/test").append(metadataComponent));
  data.setContentType(tlv::ContentType_Key);
  BOOST_CHECK_THROW(MetadataObject metadata(data), tlv::Error);

  // invalid metadata name
  data.setName("/ndn/unit/test");
  data.setContentType(tlv::ContentType_Blob);
  BOOST_CHECK_THROW(MetadataObject metadata(data), tlv::Error);

  // empty content
  data.setName(Name("ndn/unit/test").append(metadataComponent));
  BOOST_CHECK_THROW(MetadataObject metadata(data), tlv::Error);

  // non-empty content with no name element
  data.setContent("F000"_block);
  BOOST_CHECK_THROW(MetadataObject metadata(data), tlv::Error);
}

BOOST_AUTO_TEST_CASE(IsValidName)
{
  // valid name
  Name name = Name("/ndn/unit/test")
              .append(metadataComponent)
              .appendVersion()
              .appendSegment(0);
  BOOST_CHECK(MetadataObject::isValidName(name));

  // invalid names
  // segment component is missing
  BOOST_CHECK_EQUAL(MetadataObject::isValidName(name.getPrefix(-1)), false);

  // version component is missing
  BOOST_CHECK_EQUAL(MetadataObject::isValidName(name.getPrefix(-2)), false);

  // keyword name component `32=keyword` is missing
  BOOST_CHECK_EQUAL(MetadataObject::isValidName(name.getPrefix(-3)), false);

  // too short name
  BOOST_CHECK_EQUAL(MetadataObject::isValidName(name.getPrefix(-4)), false);

  // out-of-order segment and version components
  name = name.getPrefix(-2).appendSegment(0).appendVersion();
  BOOST_CHECK_EQUAL(MetadataObject::isValidName(name), false);

  // invalid name component keyword
  name = name.getPrefix(-3)
         .append(32, reinterpret_cast<const uint8_t*>("foo"), std::strlen("foo"))
         .appendVersion()
         .appendSegment(0);
  BOOST_CHECK_EQUAL(MetadataObject::isValidName(name), false);
}

BOOST_AUTO_TEST_CASE(MakeDiscoveryInterest)
{
  Interest interest = MetadataObject::makeDiscoveryInterest(baseContentName);
  BOOST_CHECK_EQUAL(interest.getName(), Name(baseContentName).append(metadataComponent));
  BOOST_CHECK(interest.getCanBePrefix());
  BOOST_CHECK(interest.getMustBeFresh());
}

BOOST_AUTO_TEST_SUITE_END() // TestMetadataObject

} // namespace tests
} // namespace ndn
