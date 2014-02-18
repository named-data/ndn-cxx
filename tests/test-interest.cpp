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
  BOOST_REQUIRE_EQUAL(i.getInterestLifetime(), 1000);
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
  BOOST_REQUIRE_EQUAL(i.getInterestLifetime(), 1000);
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
  i.setInterestLifetime(1000);
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
  {
    ndn::Interest i1(ndn::Name("/local/ndn/prefix"));
    i1.setMustBeFresh(true);
    i1.setIncomingFaceId(10);
    i1.setNonce(1);
    
    BOOST_CHECK(!i1.hasWire());
    const Block* block1 = &i1.getLocalControlHeader().wireEncode(i1);
    
    BOOST_CHECK_EQUAL(block1, &i1.getLocalControlHeader().wireEncode(i1));
    BOOST_CHECK(i1.hasWire());
    BOOST_CHECK_NE(block1, &i1.wireEncode());
    BOOST_CHECK_NE(block1->wire(), i1.wireEncode().wire());
    BOOST_CHECK_NE(block1->size(), i1.wireEncode().size());

    BOOST_CHECK_EQUAL_COLLECTIONS(InterestWithLocalControlHeader,
                                  InterestWithLocalControlHeader+sizeof(InterestWithLocalControlHeader),
                                  block1->begin(), block1->end());

    Block savedBlock1 = *block1;

    i1.setNonce(1000);
    BOOST_CHECK(!i1.hasWire());
  
    const Block* block4 = &i1.getLocalControlHeader().wireEncode(i1);
    BOOST_CHECK_EQUAL(block4, &i1.getLocalControlHeader().wireEncode(i1));
    BOOST_CHECK_NE((void*)block4->wire(), (void*)savedBlock1.wire());
    BOOST_CHECK_NE(block4->size(), savedBlock1.size()); 
  }

  {
    ndn::Interest i1(ndn::Name("/local/ndn/prefix"));
    i1.setMustBeFresh(true);
    i1.setIncomingFaceId(10);
    i1.setNonce(1);
    i1.wireEncode(); // Encode with reserve for LocalControlHeader

    const void* savedWire = i1.wireEncode().wire();
    const Block* block1 = &i1.getLocalControlHeader().wireEncode(i1);

    BOOST_CHECK_EQUAL((const void*)i1.wireEncode().wire(), savedWire);
    BOOST_CHECK_EQUAL(i1.wireEncode().wire() - block1->wire(), 5);
   }

  {
    ndn::Interest i1(ndn::Name("/local/ndn/prefix"));
    i1.setMustBeFresh(true);
    i1.setIncomingFaceId(10);
    i1.setNonce(1);

    EncodingBuffer buffer(31,0); // compared to previous version, there is not reserve for LocalControlHeader
    i1.wireEncode(buffer);
    i1.wireDecode(buffer.block());

    const void* savedWire = i1.wireEncode().wire();
    const Block* block1 = &i1.getLocalControlHeader().wireEncode(i1);

    BOOST_CHECK_EQUAL((const void*)i1.wireEncode().wire(), savedWire);
    BOOST_CHECK_NE(i1.wireEncode().wire() - block1->wire(), 5);
   }
  
  {
    ndn::Interest i2(ndn::Name("/local/ndn/prefix"));
    i2.setMustBeFresh(true);
    i2.setNonce(1);

    BOOST_CHECK(!i2.hasWire());
    const Block* block2 = &i2.getLocalControlHeader().wireEncode(i2);
    BOOST_CHECK_EQUAL(block2, &i2.getLocalControlHeader().wireEncode(i2));
    BOOST_CHECK(i2.hasWire());
    BOOST_CHECK_NE(block2, &i2.wireEncode());
    BOOST_CHECK_EQUAL(block2->wire(), i2.wireEncode().wire());
    BOOST_CHECK_EQUAL(block2->size(), i2.wireEncode().size());

    Block savedBlock2 = *block2;

    BOOST_CHECK_EQUAL_COLLECTIONS(InterestWithoutLocalControlHeader,
                                  InterestWithoutLocalControlHeader+sizeof(InterestWithoutLocalControlHeader),
                                  block2->begin(), block2->end());

    i2.setNonce(1);
    BOOST_CHECK(!i2.hasWire());
  
    const Block* block4 = &i2.getLocalControlHeader().wireEncode(i2);
    BOOST_CHECK_EQUAL(block4, &i2.getLocalControlHeader().wireEncode(i2));
    BOOST_CHECK_NE((void*)block4->wire(), (void*)savedBlock2.wire());
    BOOST_CHECK_EQUAL(block4->size(), savedBlock2.size());
  }
}

BOOST_AUTO_TEST_CASE (DecodeWithLocalHeader)
{
  Block b1(InterestWithLocalControlHeader, sizeof(InterestWithLocalControlHeader));
  const Block& block1 = nfd::LocalControlHeader::getPayload(b1);
  BOOST_REQUIRE_NE(&block1, &b1);

  BOOST_CHECK_EQUAL(block1.type(), (uint32_t)Tlv::Interest);
  BOOST_CHECK_EQUAL(b1.type(), (uint32_t)tlv::nfd::LocalControlHeader);

  Interest i(block1);
  BOOST_CHECK(!i.getLocalControlHeader().hasIncomingFaceId());
  BOOST_CHECK(!i.getLocalControlHeader().hasNextHopFaceId());

  BOOST_REQUIRE_NO_THROW(i.getLocalControlHeader().wireDecode(b1));

  BOOST_CHECK_EQUAL(i.getIncomingFaceId(), 10);
  BOOST_CHECK(!i.getLocalControlHeader().hasNextHopFaceId());

  BOOST_CHECK_EQUAL((void*)i.getLocalControlHeader().wireEncode(i).wire(), (void*)b1.wire());

  //
  
  Block b2(InterestWithoutLocalControlHeader, sizeof(InterestWithoutLocalControlHeader));
  const Block& block2 = nfd::LocalControlHeader::getPayload(b2);
  BOOST_CHECK_EQUAL(&block2, &b2);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
