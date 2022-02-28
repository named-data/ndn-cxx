/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/validation-policy-simple-hierarchy.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/validator-fixture.hpp"

#include <boost/mpl/vector.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestValidationPolicySimpleHierarchy,
                         HierarchicalValidatorFixture<ValidationPolicySimpleHierarchy>)

using Packets = boost::mpl::vector<InterestV03Pkt, DataPkt>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Validate, Packet, Packets)
{
  const Name name = "/Security/ValidatorFixture/Sub1/Sub2/Packet";

  auto packet = Packet::makePacket(name);
  VALIDATE_FAILURE(packet, "Unsigned");

  packet = Packet::makePacket(name);
  m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_FAILURE(packet, "Should not be accepted, name not prefix of /localhost/identity/digest-sha256");

  packet = Packet::makePacket("/localhost/identity/digest-sha256/foobar");
  m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_SUCCESS(packet, "Should be accepted, as name is prefix of /localhost/identity/digest-sha256");

  packet = Packet::makePacket(name);
  m_keyChain.sign(packet, signingByIdentity(identity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the anchor");

  packet = Packet::makePacket(name);
  m_keyChain.sign(packet, signingByIdentity(subIdentity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the policy-compliant cert");

  packet = Packet::makePacket(name);
  m_keyChain.sign(packet, signingByIdentity(otherIdentity));
  VALIDATE_FAILURE(packet, "Should fail, as signed by the policy-violating cert");

  packet = Packet::makePacket(name);
  m_keyChain.sign(packet, signingByIdentity(subSelfSignedIdentity));
  VALIDATE_FAILURE(packet, "Should fail, because subSelfSignedIdentity is not a trust anchor");

  // TODO add checks with malformed packets
}

BOOST_AUTO_TEST_CASE(NonKeyNameInsideLocator)
{
//  auto cert = identity.getDefaultKey().getDefaultCertificate().wireEncode();
//  std::cerr << "Certificate idCert{\"" << toHex(cert) << "\"_block};" << std::endl;

//  cert = subIdentity.getDefaultKey().getDefaultCertificate().wireEncode();
//  std::cerr << "Certificate subIdCert{\"" << toHex(cert) << "\"_block};" << std::endl;

//  Data pkt("/Security/ValidatorFixture/Sub1/Sub2/Packet");
//  m_keyChain.sign(pkt, signingByIdentity(subIdentity));
//  std::cerr << "Data packet{\"" << toHex(pkt.wireEncode()) << "\"_block};" << std::endl;

  // These are hard-coded with a key locator that is the exact name of the certificate
  Certificate idCert{"06FD014C073C08085365637572697479081056616C696461746F724669787475726508034B455"
      "9080816971C408D162A0A080473656C660809FD000001499D598CA0140918010219040036EE80155B30593013060"
      "72A8648CE3D020106082A8648CE3D03010703420004F795BB9A7D51ECC4491605D49ADFCB46598D8260F872F0F15"
      "15FA4EA7259F2A27C25B5ADAFA8D548C5FA0FBB88AB7769E986057FCAB52C7AF17A7964C47DF57B165C1B01031C2"
      "D072B08085365637572697479081056616C696461746F724669787475726508034B4559080816971C408D162A0AF"
      "D00FD26FD00FE0F313937303031303154303030303030FD00FF0F323033343131303654303533353332174630440"
      "2200E9C538FE7285B006476C86F1CCCEC4398C939D1F6CE2E4F3537EB145252EE1502201A1C50E647CAF90CAA76A"
      "065452D1D560E5BB7122A7553002E7DFBCA80FCB422"_block};
  Certificate subIdCert{"06FD0188074408085365637572697479081056616C696461746F7246697874757265080453"
      "75623108034B45590808D76A31B01E14092B0806706172656E740809FD000001499D598CA0140918010219040036"
      "EE80155B3059301306072A8648CE3D020106082A8648CE3D0301070342000441783BEDE80DEF6F2910D78E1BD0AD"
      "7EF033A62F510A7578D1FC8F7601AC2B7709B27BBF225423A865964FA523B9746169FDB03C68B2FAC8D75DA0C755"
      "8122AF16901B01031C3E073C08085365637572697479081056616C696461746F724669787475726508034B455908"
      "0816971C408D162A0A080473656C660809FD000001499D598CA0FD00FD26FD00FE0F323031343131313154303533"
      "353332FD00FF0F323033343131303654303533353332FD01021FFD02001BFD02010474797065FD02020F7375622D"
      "636572746966696361746517463044022013CEFBF0E15EA6C85B26C56B550316A8A0F6F45510CCBDA4188E7732EF"
      "1E81B6022026BDC1B6F32BE163F20E379B86EF686F5B8B2B57B6494DE89D5D503C8E3C6125"_block};
  Data packet{"06CD073008085365637572697479081056616C696461746F724669787475726508045375623108045375"
      "623208065061636B657414001500164B1B01031C46074408085365637572697479081056616C696461746F724669"
      "787475726508045375623108034B45590808D76A31B01E14092B0806706172656E740809FD000001499D598CA017"
      "483046022100BDD3E0EF2385658825EB73E87A02D1A16AA8ACE50840C1B91782836164AACA3B0221008007B3EBA9"
      "B7638BD204766B08AF6E4221CDB88156CC7DA13CD916610D6D3AED"_block};

  this->cache.insert(idCert);
  this->cache.insert(subIdCert);
  this->validator.loadAnchor("", std::move(idCert));

  BOOST_REQUIRE(packet.getKeyLocator());
  BOOST_CHECK_EQUAL(packet.getKeyLocator()->getName(),
                    "/Security/ValidatorFixture/Sub1/KEY/%D7j1%B0%1E%14%09%2B/parent/%FD%00%00%01I%9DY%8C%A0");

  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the policy-compliant cert");
}

BOOST_AUTO_TEST_SUITE_END() // TestValidationPolicySimpleHierarchy
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn
