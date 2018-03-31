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

#include "interest.hpp"
#include "data.hpp"
#include "security/digest-sha256.hpp"
#include "security/signature-sha256-with-rsa.hpp"

#include "boost-test.hpp"
#include "identity-management-fixture.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestInterest)

// ---- constructor, encode, decode ----

BOOST_AUTO_TEST_CASE(DefaultConstructor)
{
  Interest i;
  BOOST_CHECK_EQUAL(i.getName(), "/");
  BOOST_CHECK(i.getSelectors().empty());
  BOOST_CHECK_EQUAL(i.hasNonce(), false);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
}

BOOST_AUTO_TEST_CASE(EncodeDecodeBasic)
{
  const uint8_t WIRE[] = {
    0x05, 0x1c, // Interest
          0x07, 0x14, // Name
                0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // GenericNameComponent
                0x08, 0x03, 0x6e, 0x64, 0x6e, // GenericNameComponent
                0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, // GenericNameComponent
          0x0a, 0x04, // Nonce
                0x01, 0x00, 0x00, 0x00
  };

  Interest i1("/local/ndn/prefix");
  i1.setNonce(1);
  Block wire1 = i1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(), "/local/ndn/prefix");
  BOOST_CHECK(i2.getSelectors().empty());
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);

  BOOST_CHECK_EQUAL(i1, i2);
}

BOOST_AUTO_TEST_CASE(EncodeDecodeFull)
{
  const uint8_t WIRE[] = {
    0x05, 0x31, // Interest
          0x07, 0x14, // Name
                0x08, 0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // GenericNameComponent
                0x08, 0x03, 0x6e, 0x64, 0x6e, // GenericNameComponent
                0x08, 0x06, 0x70, 0x72, 0x65, 0x66, 0x69, 0x78, // GenericNameComponent
          0x09, 0x03, // Selectors
                0x0d, 0x01, 0x01,  // MinSuffixComponents
          0x0a, 0x04, // Nonce
                0x01, 0x00, 0x00, 0x00,
          0x0c, 0x02, // InterestLifetime
                0x03, 0xe8,
          0x1e, 0x0a, // ForwardingHint
                0x1f, 0x08, // Delegation
                      0x1e, 0x01, 0x01, // Preference=1
                      0x07, 0x03, 0x08, 0x01, 0x41 // Name=/A
  };

  Interest i1;
  i1.setName("/local/ndn/prefix");
  i1.setMinSuffixComponents(1);
  i1.setNonce(1);
  i1.setInterestLifetime(1000_ms);
  i1.setForwardingHint({{1, "/A"}});
  Block wire1 = i1.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(wire1.begin(), wire1.end(), WIRE, WIRE + sizeof(WIRE));

  Interest i2(wire1);
  BOOST_CHECK_EQUAL(i2.getName(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i2.getMinSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), 1000_ms);
  BOOST_CHECK_EQUAL(i2.getForwardingHint(), DelegationList({{1, "/A"}}));

  BOOST_CHECK_EQUAL(i1, i2);
}

BOOST_AUTO_TEST_CASE(WireDecodeReset) // checks wireDecode resets all fields
{
  Interest i1;
  i1.setName("/test");
  i1.setMinSuffixComponents(100);
  i1.setNonce(10);
  i1.setInterestLifetime(10_s);

  Interest i2(i1.wireEncode());
  BOOST_CHECK_EQUAL(i2.getName().toUri(), "/test");
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), 10_s);
  BOOST_CHECK_EQUAL(i2.getMinSuffixComponents(), 100);
  BOOST_CHECK_EQUAL(i2.getNonce(), 10);

  i2.wireDecode(Interest().wireEncode());
  BOOST_CHECK_EQUAL(i2.getName().toUri(), "/");
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK_EQUAL(i2.getMinSuffixComponents(), -1);
  BOOST_WARN_NE(i2.getNonce(), 10);
}

BOOST_AUTO_TEST_CASE(DecodeNoName)
{
  Block b(tlv::Interest);
  b.push_back(makeBinaryBlock(tlv::Nonce, "FISH", 4));
  b.encode();

  Interest i;
  BOOST_CHECK_THROW(i.wireDecode(b), tlv::Error);
}

BOOST_AUTO_TEST_CASE(DecodeNoNonce)
{
  Block b(tlv::Interest);
  b.push_back(Name("/YvzNKtPWh").wireEncode());
  b.encode();

  Interest i;
  BOOST_CHECK_THROW(i.wireDecode(b), tlv::Error);
}

BOOST_AUTO_TEST_CASE(DecodeBadNonce)
{
  Block b(tlv::Interest);
  b.push_back(Name("/BJzEHVxDJ").wireEncode());
  b.push_back(makeBinaryBlock(tlv::Nonce, "SKY", 3));
  b.encode();

  Interest i;
  BOOST_CHECK_THROW(i.wireDecode(b), tlv::Error);
}

// ---- matching ----

BOOST_AUTO_TEST_CASE(MatchesData)
{
  Interest interest;
  interest.setName("ndn:/A")
          .setMinSuffixComponents(2)
          .setMaxSuffixComponents(2)
          .setPublisherPublicKeyLocator(KeyLocator("ndn:/B"))
          .setExclude(Exclude().excludeAfter(name::Component("J")));

  Data data("ndn:/A/D");
  SignatureSha256WithRsa signature(KeyLocator("ndn:/B"));
  signature.setValue(encoding::makeEmptyBlock(tlv::SignatureValue));
  data.setSignature(signature);
  data.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data), true);

  Data data1 = data;
  data1.setName("ndn:/A"); // violates MinSuffixComponents
  data1.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data1), false);

  Interest interest1 = interest;
  interest1.setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(interest1.matchesData(data1), true);

  Data data2 = data;
  data2.setName("ndn:/A/E/F"); // violates MaxSuffixComponents
  data2.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data2), false);

  Interest interest2 = interest;
  interest2.setMaxSuffixComponents(3);
  BOOST_CHECK_EQUAL(interest2.matchesData(data2), true);

  Data data3 = data;
  SignatureSha256WithRsa signature3(KeyLocator("ndn:/G")); // violates PublisherPublicKeyLocator
  signature3.setValue(encoding::makeEmptyBlock(tlv::SignatureValue));
  data3.setSignature(signature3);
  data3.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data3), false);

  Interest interest3 = interest;
  interest3.setPublisherPublicKeyLocator(KeyLocator("ndn:/G"));
  BOOST_CHECK_EQUAL(interest3.matchesData(data3), true);

  Data data4 = data;
  DigestSha256 signature4; // violates PublisherPublicKeyLocator
  signature4.setValue(encoding::makeEmptyBlock(tlv::SignatureValue));
  data4.setSignature(signature4);
  data4.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data4), false);

  Interest interest4 = interest;
  interest4.setPublisherPublicKeyLocator(KeyLocator());
  BOOST_CHECK_EQUAL(interest4.matchesData(data4), true);

  Data data5 = data;
  data5.setName("ndn:/A/J"); // violates Exclude
  data5.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data5), false);

  Interest interest5 = interest;
  interest5.setExclude(Exclude().excludeAfter(name::Component("K")));
  BOOST_CHECK_EQUAL(interest5.matchesData(data5), true);

  Data data6 = data;
  data6.setName("ndn:/H/I"); // violates Name
  data6.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data6), false);

  Data data7 = data;
  data7.setName("ndn:/A/B");
  data7.wireEncode();

  Interest interest7("/A/B/sha256digest=54008e240a7eea2714a161dfddf0dd6ced223b3856e9da96792151e180f3b128");
  BOOST_CHECK_EQUAL(interest7.matchesData(data7), true);

  Interest interest7b("/A/B/sha256digest=0000000000000000000000000000000000000000000000000000000000000000");
  BOOST_CHECK_EQUAL(interest7b.matchesData(data7), false); // violates implicit digest
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MatchesInterest, 1)
BOOST_AUTO_TEST_CASE(MatchesInterest)
{
  Interest interest;
  interest
    .setName("/A")
    .setMinSuffixComponents(2)
    .setMaxSuffixComponents(2)
    .setPublisherPublicKeyLocator(KeyLocator("/B"))
    .setExclude(Exclude().excludeAfter(name::Component("J")))
    .setNonce(10)
    .setInterestLifetime(5_s)
    .setForwardingHint({{1, "/H"}});

  Interest other;
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false);

  other.setName(interest.getName());
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false);

  other.setSelectors(interest.getSelectors());
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), false); // will match until #3162 implemented

  other.setForwardingHint({{1, "/H"}});
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);

  other.setNonce(200);
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);

  other.setInterestLifetime(5_h);
  BOOST_CHECK_EQUAL(interest.matchesInterest(other), true);
}

// ---- field accessors ----

BOOST_AUTO_TEST_CASE(GetNonce)
{
  unique_ptr<Interest> i1, i2;

  // getNonce automatically assigns a random Nonce.
  // It's possible to assign the same Nonce to two Interest, but it's unlikely to get 100 pairs of
  // same Nonces in a row.
  int nIterations = 0;
  uint32_t nonce1 = 0, nonce2 = 0;
  do {
    i1 = make_unique<Interest>();
    nonce1 = i1->getNonce();
    i2 = make_unique<Interest>();
    nonce2 = i2->getNonce();
  }
  while (nonce1 == nonce2 && ++nIterations < 100);
  BOOST_CHECK_NE(nonce1, nonce2);
  BOOST_CHECK(i1->hasNonce());
  BOOST_CHECK(i2->hasNonce());

  // Once a Nonce is assigned, it should not change.
  BOOST_CHECK_EQUAL(i1->getNonce(), nonce1);
}

BOOST_AUTO_TEST_CASE(SetNonce)
{
  Interest i1("/A");
  i1.setNonce(1);
  i1.wireEncode();
  BOOST_CHECK_EQUAL(i1.getNonce(), 1);

  Interest i2(i1);
  BOOST_CHECK_EQUAL(i2.getNonce(), 1);

  i2.setNonce(2);
  BOOST_CHECK_EQUAL(i2.getNonce(), 2);
  BOOST_CHECK_EQUAL(i1.getNonce(), 1); // should not affect i1 Nonce (Bug #4168)
}

BOOST_AUTO_TEST_CASE(RefreshNonce)
{
  Interest i;
  BOOST_CHECK(!i.hasNonce());
  i.refreshNonce();
  BOOST_CHECK(!i.hasNonce());

  i.setNonce(1);
  BOOST_CHECK(i.hasNonce());
  i.refreshNonce();
  BOOST_CHECK(i.hasNonce());
  BOOST_CHECK_NE(i.getNonce(), 1);
}

BOOST_AUTO_TEST_CASE(SetInterestLifetime)
{
  BOOST_CHECK_THROW(Interest("/A", time::milliseconds(-1)), std::invalid_argument);
  BOOST_CHECK_NO_THROW(Interest("/A", 0_ms));

  Interest i("/local/ndn/prefix");
  i.setNonce(1);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK_THROW(i.setInterestLifetime(time::milliseconds(-1)), std::invalid_argument);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  i.setInterestLifetime(0_ms);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 0_ms);
  i.setInterestLifetime(1_ms);
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), 1_ms);
}

BOOST_AUTO_TEST_CASE(ModifyForwardingHint)
{
  Interest i;
  i.setForwardingHint({{1, "/A"}});
  i.wireEncode();
  BOOST_CHECK(i.hasWire());

  i.modifyForwardingHint([] (DelegationList& fh) { fh.insert(2, "/B"); });
  BOOST_CHECK(!i.hasWire());
  BOOST_CHECK_EQUAL(i.getForwardingHint(), DelegationList({{1, "/A"}, {2, "/B"}}));
}

// ---- operators ----

BOOST_AUTO_TEST_CASE(Equality)
{
  Interest a;
  Interest b;

  // if nonce is not set, it would be set to a random value
  a.setNonce(1);
  b.setNonce(1);

  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // compare Name
  a.setName("/A");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("/B");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("/A");
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // compare Selectors
  a.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // compare Nonce
  a.setNonce(100);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setNonce(100);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // compare InterestLifetime
  a.setInterestLifetime(10_s);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setInterestLifetime(10_s);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // compare ForwardingHint
  a.setForwardingHint({{1, "/H"}});
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setForwardingHint({{1, "/H"}});
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_SUITE_END() // TestInterest

} // namespace tests
} // namespace ndn
