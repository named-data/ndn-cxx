/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "interest.hpp"
#include "data.hpp"
#include "security/signature-sha256-with-rsa.hpp"
#include "security/signature-sha256.hpp"
#include "encoding/buffer-stream.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestInterest)

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Interest>));
BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Selectors>));

const uint8_t Interest1[] = {
  0x05,  0x5c, // NDN Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x09,  0x37, // Selectors
          0x0d,  0x1,  0x1,  // MinSuffix
          0x0e,  0x1,  0x1,  // MaxSuffix
          0x1c, 0x16, // KeyLocator
              0x07, 0x14, // Name
                  0x08, 0x04,
                      0x74, 0x65, 0x73, 0x74,
                  0x08, 0x03,
                      0x6b, 0x65, 0x79,
                  0x08, 0x07,
                      0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
          0x10,  0x14, // Exclude
              0x08,  0x4, // NameComponent
                  0x61,  0x6c,  0x65,  0x78,
              0x08,  0x4, // NameComponent
                  0x78,  0x78,  0x78,  0x78,
              0x13,  0x0, // Any
              0x08,  0x4, // NameComponent
                  0x79,  0x79,  0x79,  0x79,
          0x11,  0x1, // ChildSelector
              0x1,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x0b,  0x1, // Scope
          0x1,
      0x0c,       // InterestLifetime
          0x2,  0x3,  0xe8
};

const uint8_t Interest2[] = {
  0x05,  0x5c, // NDN Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x09,  0x37, // Selectors
          0x0d,  0x1,  0x1,  // MinSuffix
          0x0e,  0x1,  0x1,  // MaxSuffix
          0x1c, 0x16, // KeyLocator
              0x07, 0x14, // Name
                  0x08, 0x04,
                      0x74, 0x65, 0x73, 0x74,
                  0x08, 0x03,
                      0x6b, 0x65, 0x79,
                  0x08, 0x07,
                      0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
          0x10,  0x14, // Exclude
              0x08,  0x4, // NameComponent
                  0x61,  0x6c,  0x65,  0x78,
              0x08,  0x4, // NameComponent
                  0x78,  0x78,  0x78,  0x78,
              0x13,  0x0, // Any
              0x08,  0x4, // NameComponent
                  0x79,  0x79,  0x79,  0x79,
          0x11,  0x1, // ChildSelector
              0x1,
      0x0a,  0x4, // Nonce
          0x2, 0x0, 0x0, 0x00,
      0x0b,  0x1, // Scope
          0x1,
      0x0c,       // InterestLifetime
          0x2,  0x3,  0xe8
};

const uint8_t InterestWithLocalControlHeader[] = {
  0x50, 0x25, 0x51, 0x01, 0x0a,
  0x05, 0x20, 0x07, 0x14, 0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x08, 0x03, 0x6e, 0x64,
  0x6e, 0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, 0x09, 0x02, 0x12, 0x00, 0x0a, 0x04,
  0x01, 0x00, 0x00, 0x00
};

const uint8_t InterestWithoutLocalControlHeader[] = {
  0x05, 0x20, 0x07, 0x14, 0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x08, 0x03, 0x6e, 0x64,
  0x6e, 0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, 0x09, 0x02, 0x12, 0x00, 0x0a, 0x04,
  0x01, 0x00, 0x00, 0x00
};

BOOST_AUTO_TEST_CASE(InterestEqualityChecks)
{
  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                Scope?
  //                InterestLifetime?

  Interest a;
  Interest b;

  // if nonce is not set, it will be set to a random value
  a.setNonce(1);
  b.setNonce(1);

  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Name
  a.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/B");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Selectors
  a.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Nonce
  a.setNonce(100);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setNonce(100);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Nonce
  a.setScope(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setScope(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on InterestLifetime
  a.setInterestLifetime(time::seconds(10));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setInterestLifetime(time::seconds(10));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_CASE(SelectorsEqualityChecks)
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

BOOST_AUTO_TEST_CASE(Decode)
{
  Block interestBlock(Interest1, sizeof(Interest1));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_CHECK_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i.getScope(), 1);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), time::milliseconds(1000));
  BOOST_CHECK_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getPublisherPublicKeyLocator().getType(),
                    static_cast<uint32_t>(KeyLocator::KeyLocator_Name));
  BOOST_CHECK_EQUAL(i.getPublisherPublicKeyLocator().getName(), "ndn:/test/key/locator");
  BOOST_CHECK_EQUAL(i.getChildSelector(), 1);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_CHECK_EQUAL(i.getNonce(), 1U);
}

BOOST_AUTO_TEST_CASE(DecodeFromStream)
{
  boost::iostreams::stream<boost::iostreams::array_source> is(
    reinterpret_cast<const char *>(Interest1), sizeof(Interest1));

  Block interestBlock = Block::fromStream(is);

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_CHECK_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i.getScope(), 1);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), time::milliseconds(1000));
  BOOST_CHECK_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getChildSelector(), 1);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_CHECK_EQUAL(i.getNonce(), 1U);
}

BOOST_AUTO_TEST_CASE(Encode)
{
  ndn::Interest i(ndn::Name("/local/ndn/prefix"));
  i.setScope(1);
  i.setInterestLifetime(time::milliseconds(1000));
  i.setMinSuffixComponents(1);
  i.setMaxSuffixComponents(1);
  i.setPublisherPublicKeyLocator(KeyLocator("ndn:/test/key/locator"));
  i.setChildSelector(1);
  i.setMustBeFresh(false);
  Exclude exclude;
  exclude
    .excludeOne(name::Component("alex"))
    .excludeRange(name::Component("xxxx"), name::Component("yyyy"));
  i.setExclude(exclude);
  i.setNonce(1);

  BOOST_CHECK_EQUAL(i.hasWire(), false);
  const Block &wire = i.wireEncode();
  BOOST_CHECK_EQUAL(i.hasWire(), true);

  BOOST_CHECK_EQUAL_COLLECTIONS(Interest1, Interest1 + sizeof(Interest1),
                                wire.begin(), wire.end());

  const uint8_t* originalWire = wire.wire();
  i.setNonce(2);
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK_EQUAL(originalWire, i.wireEncode().wire());
  BOOST_CHECK_EQUAL(i.hasWire(), true);

  BOOST_CHECK_EQUAL_COLLECTIONS(Interest2, Interest2 + sizeof(Interest2),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(EncodeWithLocalHeader)
{
  ndn::Interest interest(ndn::Name("/local/ndn/prefix"));
  interest.setMustBeFresh(true);
  interest.setIncomingFaceId(10);
  interest.setNonce(1);

  BOOST_CHECK(!interest.hasWire());

  Block headerBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);

  BOOST_CHECK(interest.hasWire());
  BOOST_CHECK(headerBlock.hasWire());

  BOOST_CHECK_NE(headerBlock.wire(), interest.wireEncode().wire());
  BOOST_CHECK_NE(headerBlock.size(), interest.wireEncode().size());
  BOOST_CHECK_EQUAL(headerBlock.size(), 5);

  BOOST_CHECK_EQUAL_COLLECTIONS(InterestWithLocalControlHeader,
                                InterestWithLocalControlHeader + 5,
                                headerBlock.begin(), headerBlock.end());

  interest.setNonce(1000);

  Block updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 5);

  // only length should have changed
  BOOST_CHECK_EQUAL_COLLECTIONS(updatedHeaderBlock.begin() + 2, updatedHeaderBlock.end(),
                                headerBlock.begin() + 2,        headerBlock.end());

  // updating IncomingFaceId that keeps the length
  interest.setIncomingFaceId(100);
  updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 5);
  BOOST_CHECK_NE(*(updatedHeaderBlock.begin() + 4), *(headerBlock.begin() + 4));

  // updating IncomingFaceId that increases the length by 2
  interest.setIncomingFaceId(1000);
  updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 6);

  // adding NextHopId
  interest.setNextHopFaceId(1);
  updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 9);

  // masking IncomingFaceId
  updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, false, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 5);

  // masking NextHopId
  updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, false);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 6);

  // masking everything
  BOOST_CHECK_THROW(interest.getLocalControlHeader().wireEncode(interest, false, false),
                    nfd::LocalControlHeader::Error);
}


BOOST_AUTO_TEST_CASE(DecodeWithLocalHeader)
{
  Block wireBlock(InterestWithLocalControlHeader, sizeof(InterestWithLocalControlHeader));
  const Block& payload = nfd::LocalControlHeader::getPayload(wireBlock);
  BOOST_REQUIRE_NE(&payload, &wireBlock);

  BOOST_CHECK_EQUAL(payload.type(), static_cast<uint32_t>(Tlv::Interest));
  BOOST_CHECK_EQUAL(wireBlock.type(), static_cast<uint32_t>(tlv::nfd::LocalControlHeader));

  Interest interest(payload);
  BOOST_CHECK(!interest.getLocalControlHeader().hasIncomingFaceId());
  BOOST_CHECK(!interest.getLocalControlHeader().hasNextHopFaceId());

  BOOST_REQUIRE_NO_THROW(interest.getLocalControlHeader().wireDecode(wireBlock));

  BOOST_CHECK_EQUAL(interest.getLocalControlHeader().wireEncode(interest, true, true).size(), 5);

  BOOST_CHECK_EQUAL(interest.getIncomingFaceId(), 10);
  BOOST_CHECK(!interest.getLocalControlHeader().hasNextHopFaceId());

  BOOST_CHECK_THROW(interest.getLocalControlHeader().wireEncode(interest, false, false),
                    nfd::LocalControlHeader::Error);

  BOOST_CHECK_THROW(interest.getLocalControlHeader().wireEncode(interest, false, true),
                    nfd::LocalControlHeader::Error);

  BOOST_CHECK_NO_THROW(interest.getLocalControlHeader().wireEncode(interest, true, false));
  BOOST_CHECK_NO_THROW(interest.getLocalControlHeader().wireEncode(interest, true, true));

  BOOST_CHECK_NE((void*)interest.getLocalControlHeader().wireEncode(interest, true, true).wire(),
                 (void*)wireBlock.wire());

  BOOST_CHECK_EQUAL(interest.getLocalControlHeader().wireEncode(interest, true, true).size(), 5);
}

BOOST_AUTO_TEST_CASE(DecodeWithoutLocalHeader)
{
  Block wireBlock(InterestWithoutLocalControlHeader, sizeof(InterestWithoutLocalControlHeader));
  const Block& payload = nfd::LocalControlHeader::getPayload(wireBlock);
  BOOST_CHECK_EQUAL(&payload, &wireBlock);
}

BOOST_AUTO_TEST_CASE(MatchesData)
{
  Interest interest;
  interest.setName("ndn:/A")
          .setMinSuffixComponents(2)
          .setMaxSuffixComponents(2)
          .setPublisherPublicKeyLocator(KeyLocator("ndn:/B"))
          .setExclude(Exclude().excludeBefore(name::Component("C")));

  Data data("ndn:/A/D");
  SignatureSha256WithRsa signature;
  signature.setKeyLocator(KeyLocator("ndn:/B"));
  data.setSignature(signature);
  BOOST_CHECK_EQUAL(interest.matchesData(data), true);

  Data data1 = data;
  data1.setName("ndn:/A");// violates MinSuffixComponents
  BOOST_CHECK_EQUAL(interest.matchesData(data1), false);

  Data data2 = data;
  data2.setName("ndn:/A/E/F");// violates MaxSuffixComponents
  BOOST_CHECK_EQUAL(interest.matchesData(data2), false);

  Data data3 = data;
  SignatureSha256WithRsa signature3;
  signature3.setKeyLocator(KeyLocator("ndn:/G"));// violates PublisherPublicKeyLocator
  data3.setSignature(signature3);
  BOOST_CHECK_EQUAL(interest.matchesData(data3), false);

  Data data4 = data;
  SignatureSha256 signature4;// violates PublisherPublicKeyLocator
  data4.setSignature(signature4);
  BOOST_CHECK_EQUAL(interest.matchesData(data4), false);

  Data data5 = data;
  data5.setName("ndn:/A/C");// violates Exclude
  BOOST_CHECK_EQUAL(interest.matchesData(data5), false);

  Data data6 = data;
  data6.setName("ndn:/H/I");// violates Name
  BOOST_CHECK_EQUAL(interest.matchesData(data6), false);
}

BOOST_AUTO_TEST_CASE(InterestFilterMatching)
{
  BOOST_CHECK_EQUAL(InterestFilter("/a").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b/c").doesMatch("/a/b"), false);

  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b>").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<b>").doesMatch("/a/b"), false);

  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<b>").doesMatch("/a/b/c/b"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<>*<b>").doesMatch("/a/b/c/b"), true);

  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b>").doesMatch("/a/b/c/d"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>*").doesMatch("/a/b/c/d"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>*").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>+").doesMatch("/a/b"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>+").doesMatch("/a/b/c"), true);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
