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

#include "security/key-chain.hpp"
#include "security/validator.hpp"

#include "util/command-interest-generator.hpp"
#include "util/command-interest-validator.hpp"

#include "boost-test.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestSignedInterest)

BOOST_AUTO_TEST_CASE(SignedInterest)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identityName("/TestSignedInterest/SignVerify");
  identityName.appendVersion();

  Name certificateName;
  BOOST_REQUIRE_NO_THROW(certificateName = keyChain.createIdentity(identityName));

  Interest interest("/TestSignedInterest/SignVerify/Interest1");
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(interest, identityName));

  Block interestBlock(interest.wireEncode().wire(), interest.wireEncode().size());

  Interest interest2;
  interest2.wireDecode(interestBlock);

  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = keyChain.getPublicKeyFromTpm(
    keyChain.getDefaultKeyNameForIdentity(identityName)));
  bool result = Validator::verifySignature(interest2, *publicKey);

  BOOST_CHECK_EQUAL(result, true);

  keyChain.deleteIdentity(identityName);
}

class CommandInterestFixture
{
public:
  CommandInterestFixture()
    : m_validity(false)
  {
  }

  void
  validated(const shared_ptr<const Interest>& interest)
  {
    m_validity = true;
  }

  void
  validationFailed(const shared_ptr<const Interest>& interest, const string& failureInfo)
  {
    m_validity = false;
  }

  void
  reset()
  {
    m_validity = false;
  }

  bool m_validity;
};

BOOST_FIXTURE_TEST_CASE(CommandInterest, CommandInterestFixture)
{
  KeyChain keyChain;
  Name identity("/TestCommandInterest/Validation");
  identity.appendVersion();

  Name certName;
  BOOST_REQUIRE_NO_THROW(certName = keyChain.createIdentity(identity));

  CommandInterestGenerator generator;
  CommandInterestValidator validator;

  validator.addInterestRule("^<TestCommandInterest><Validation>",
                            *keyChain.getCertificate(certName));

  //Test a legitimate command
  shared_ptr<Interest> commandInterest1 =
    make_shared<Interest>("/TestCommandInterest/Validation/Command1");
  generator.generateWithIdentity(*commandInterest1, identity);
  validator.validate(*commandInterest1,
                     bind(&CommandInterestFixture::validated, this, _1),
                     bind(&CommandInterestFixture::validationFailed, this, _1, _2));

  BOOST_CHECK_EQUAL(m_validity, true);

  //Test an outdated command
  reset();
  shared_ptr<Interest> commandInterest2 =
    make_shared<Interest>("/TestCommandInterest/Validation/Command2");
  time::milliseconds timestamp = time::toUnixTimestamp(time::system_clock::now());
  timestamp -= time::seconds(5);

  Name commandName = commandInterest2->getName();
  commandName
    .appendNumber(timestamp.count())
    .appendNumber(random::generateWord64());
  commandInterest2->setName(commandName);

  keyChain.signByIdentity(*commandInterest2, identity);
  validator.validate(*commandInterest2,
                     bind(&CommandInterestFixture::validated, this, _1),
                     bind(&CommandInterestFixture::validationFailed, this, _1, _2));

  BOOST_CHECK_EQUAL(m_validity, false);

  //Test an unauthorized command
  Name identity2("/TestCommandInterest/Validation2");
  Name certName2;
  BOOST_REQUIRE_NO_THROW(certName2 = keyChain.createIdentity(identity2));

  shared_ptr<Interest> commandInterest3 =
    make_shared<Interest>("/TestCommandInterest/Validation/Command3");
  generator.generateWithIdentity(*commandInterest3, identity2);
  validator.validate(*commandInterest3,
                     bind(&CommandInterestFixture::validated, this, _1),
                     bind(&CommandInterestFixture::validationFailed, this, _1, _2));

  BOOST_CHECK_EQUAL(m_validity, false);

  //Test another unauthorized command
  shared_ptr<Interest> commandInterest4 =
    make_shared<Interest>("/TestCommandInterest/Validation2/Command");
  generator.generateWithIdentity(*commandInterest4, identity);
  validator.validate(*commandInterest4,
                     bind(&CommandInterestFixture::validated, this, _1),
                     bind(&CommandInterestFixture::validationFailed, this, _1, _2));

  BOOST_CHECK_EQUAL(m_validity, false);

  BOOST_CHECK_NO_THROW(keyChain.deleteIdentity(identity));
  BOOST_CHECK_NO_THROW(keyChain.deleteIdentity(identity2));
}

BOOST_FIXTURE_TEST_CASE(Exemption, CommandInterestFixture)
{
  KeyChain keyChain;
  Name identity("/TestCommandInterest/AnyKey");

  Name certName;
  BOOST_REQUIRE_NO_THROW(certName = keyChain.createIdentity(identity));

  CommandInterestGenerator generator;
  CommandInterestValidator validator;

  validator.addInterestBypassRule("^<TestCommandInterest><Exemption>");

  //Test a legitimate command
  shared_ptr<Interest> commandInterest1 =
    make_shared<Interest>("/TestCommandInterest/Exemption/Command1");
  generator.generateWithIdentity(*commandInterest1, identity);
  validator.validate(*commandInterest1,
                     bind(&CommandInterestFixture::validated, this, _1),
                     bind(&CommandInterestFixture::validationFailed, this, _1, _2));

  BOOST_CHECK_EQUAL(m_validity, true);

  BOOST_CHECK_NO_THROW(keyChain.deleteIdentity(identity));
}



BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
