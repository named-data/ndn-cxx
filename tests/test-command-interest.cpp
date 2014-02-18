/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "helper/command-interest-generator.hpp"
#include "helper/command-interest-validator.hpp"
#include "util/random.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestCommandInterest)

class TestCore
{
public:
  TestCore()
    : m_validity(false)
  {}
  
  void
  validated(const shared_ptr<const Interest>& interest)
  { m_validity = true; }

  void
  validationFailed(const shared_ptr<const Interest>& interest)
  { m_validity = false; }

  void
  reset()
  { m_validity = false; }

  bool m_validity;
}; 

BOOST_AUTO_TEST_CASE (Validation)
{
  KeyChain keyChain;
  Name identity("/TestCommandInterest/Validation");
  Name certName;
  BOOST_REQUIRE_NO_THROW(certName = keyChain.createIdentity(identity));

  TestCore core;
  CommandInterestGenerator generator;
  CommandInterestValidator validator;

  validator.addInterestRule("^<TestCommandInterest><Validation>", *keyChain.getCertificate(certName));

  //Test a legitimate command
  shared_ptr<Interest> commandInterest1 = make_shared<Interest>("/TestCommandInterest/Validation/Command1");
  generator.generateWithIdentity(*commandInterest1, identity);
  validator.validate(*commandInterest1,
  		     bind(&TestCore::validated, &core, _1),
  		     bind(&TestCore::validationFailed, &core, _1));
  
  BOOST_CHECK_EQUAL(core.m_validity, true);
  
  //Test an outdated command
  core.reset();
  shared_ptr<Interest> commandInterest2 = make_shared<Interest>("/TestCommandInterest/Validation/Command2");
  int64_t timestamp = time::now() / 1000000;
  timestamp -= 5000;
  Name commandName = commandInterest2->getName();
  commandName
    .append(name::Component::fromNumber(timestamp))
    .append(name::Component::fromNumber(random::generateWord64()));
  commandInterest2->setName(commandName);
  
  keyChain.signByIdentity(*commandInterest2, identity);
  validator.validate(*commandInterest2,
  		     bind(&TestCore::validated, &core, _1),
  		     bind(&TestCore::validationFailed, &core, _1));
  
  BOOST_CHECK_EQUAL(core.m_validity, false);
  
  //Test an unauthorized command
  Name identity2("/TestCommandInterest/Validation2");
  Name certName2;
  BOOST_REQUIRE_NO_THROW(certName2 = keyChain.createIdentity(identity2));
  
  shared_ptr<Interest> commandInterest3 = make_shared<Interest>("/TestCommandInterest/Validation/Command3");
  generator.generateWithIdentity(*commandInterest3, identity2);
  validator.validate(*commandInterest3,
  		     bind(&TestCore::validated, &core, _1),
  		     bind(&TestCore::validationFailed, &core, _1));
  
  BOOST_CHECK_EQUAL(core.m_validity, false);

  //Test another unauthorized command
  shared_ptr<Interest> commandInterest4 = make_shared<Interest>("/TestCommandInterest/Validation2/Command");
  generator.generateWithIdentity(*commandInterest4, identity);
  validator.validate(*commandInterest4,
  		     bind(&TestCore::validated, &core, _1),
  		     bind(&TestCore::validationFailed, &core, _1));
  
  BOOST_CHECK_EQUAL(core.m_validity, false);

  BOOST_CHECK_NO_THROW(keyChain.deleteIdentity(identity));
  BOOST_CHECK_NO_THROW(keyChain.deleteIdentity(identity2));
}

BOOST_AUTO_TEST_SUITE_END()
}
