/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "interest.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(TestInterest)

const uint8_t Interest1[] = {
  0x05,  0x41, // NDN Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x09,  0x1f, // Selectors
          0x0d,  0x1,  0x1,  // MinSuffix
          0x0e,  0x1,  0x1,  // MaxSuffix
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
      0x0a,  0x1, // Nonce
          0x1,
      0x0b,  0x1, // Scope
          0x1,
      0x0c,       // InterestLifetime
          0x2,  0x3,  0xe8
};

const uint8_t InterestWithLocalControlHeader[] = {
  0x50, 0x22, 0x51, 0x01, 0x0a, 0x05, 0x1d, 0x07, 0x14, 0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61,
  0x6c, 0x08, 0x03, 0x6e, 0x64, 0x6e, 0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, 0x09,
  0x02, 0x12, 0x00, 0x0a, 0x01, 0x01
};

const uint8_t InterestWithoutLocalControlHeader[] = {
  0x05, 0x1d, 0x07, 0x14, 0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x08, 0x03, 0x6e, 0x64,
  0x6e, 0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, 0x09, 0x02, 0x12, 0x00, 0x0a, 0x01,
  0x01
};

BOOST_AUTO_TEST_CASE (Decode)
{
  Block interestBlock(Interest1, sizeof(Interest1));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_REQUIRE_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_REQUIRE_EQUAL(i.getScope(), 1);
  BOOST_REQUIRE_EQUAL(i.getInterestLifetime(), time::milliseconds(1000));
  BOOST_REQUIRE_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_REQUIRE_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_REQUIRE_EQUAL(i.getChildSelector(), 1);
  BOOST_REQUIRE_EQUAL(i.getMustBeFresh(), false);
  BOOST_REQUIRE_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_REQUIRE_EQUAL(i.getNonce(), 1);
}

BOOST_AUTO_TEST_CASE (DecodeFromStream)
{
  boost::iostreams::stream<boost::iostreams::array_source> is (reinterpret_cast<const char *>(Interest1), sizeof(Interest1));
  
  Block interestBlock(is);

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_REQUIRE_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_REQUIRE_EQUAL(i.getScope(), 1);
  BOOST_REQUIRE_EQUAL(i.getInterestLifetime(), time::milliseconds(1000));
  BOOST_REQUIRE_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_REQUIRE_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_REQUIRE_EQUAL(i.getChildSelector(), 1);
  BOOST_REQUIRE_EQUAL(i.getMustBeFresh(), false);
  BOOST_REQUIRE_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_REQUIRE_EQUAL(i.getNonce(), 1);
}

BOOST_AUTO_TEST_CASE (Encode)
{
  ndn::Interest i(ndn::Name("/local/ndn/prefix"));
  i.setScope(1);
  i.setInterestLifetime(time::milliseconds(1000));
  i.setMinSuffixComponents(1);
  i.setMaxSuffixComponents(1);
  i.setChildSelector(1);
  i.setMustBeFresh(false);
  Exclude exclude;
  exclude
    .excludeOne(name::Component("alex"))
    .excludeRange(name::Component("xxxx"), name::Component("yyyy"));
  i.setExclude(exclude);
  i.setNonce(1);

  const Block &wire = i.wireEncode();

  BOOST_CHECK_EQUAL_COLLECTIONS(Interest1, Interest1+sizeof(Interest1),
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
                                InterestWithLocalControlHeader+5,
                                headerBlock.begin(), headerBlock.end());

  interest.setNonce(1000);

  Block updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 5);

  // only length should have changed
  BOOST_CHECK_EQUAL_COLLECTIONS(updatedHeaderBlock.begin()+2, updatedHeaderBlock.end(),
                                headerBlock.begin()+2,        headerBlock.end());

  // updating IncomingFaceId that keeps the length
  interest.setIncomingFaceId(100);
  updatedHeaderBlock = interest.getLocalControlHeader().wireEncode(interest, true, true);
  BOOST_CHECK_EQUAL(updatedHeaderBlock.size(), 5);
  BOOST_CHECK_NE(*(updatedHeaderBlock.begin()+4), *(headerBlock.begin()+4));

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


BOOST_AUTO_TEST_CASE (DecodeWithLocalHeader)
{
  Block wireBlock(InterestWithLocalControlHeader, sizeof(InterestWithLocalControlHeader));
  const Block& payload = nfd::LocalControlHeader::getPayload(wireBlock);
  BOOST_REQUIRE_NE(&payload, &wireBlock);

  BOOST_CHECK_EQUAL(payload.type(), (uint32_t)Tlv::Interest);
  BOOST_CHECK_EQUAL(wireBlock.type(), (uint32_t)tlv::nfd::LocalControlHeader);
  
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

BOOST_AUTO_TEST_CASE (DecodeWithoutLocalHeader)
{
  Block wireBlock(InterestWithoutLocalControlHeader, sizeof(InterestWithoutLocalControlHeader));
  const Block& payload = nfd::LocalControlHeader::getPayload(wireBlock);
  BOOST_CHECK_EQUAL(&payload, &wireBlock);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
