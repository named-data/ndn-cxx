/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/security/validator-config/rule.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/validator-fixture.hpp"
#include "tests/unit/security/validator-config/common.hpp"

#include <boost/mpl/vector_c.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace validator_config {
namespace tests {

using namespace ndn::tests;
using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(ValidatorConfig)

template<class Packet>
class RuleFixture : public KeyChainFixture
{
public:
  RuleFixture()
    : rule(ruleId, Packet::getType())
    , pktName(Packet::makeName("/foo/bar", m_keyChain))
    , state(Packet::makeState())
  {
  }

public:
  const std::string ruleId = "rule-id";
  Rule rule;
  Name pktName;
  shared_ptr<ValidationState> state;
};

using PktTypes = boost::mpl::vector<DataPkt, InterestV02Pkt, InterestV03Pkt>;

BOOST_AUTO_TEST_SUITE(TestRule)

BOOST_FIXTURE_TEST_CASE(Errors, RuleFixture<DataPkt>)
{
  BOOST_CHECK_THROW(rule.match(tlv::Interest, this->pktName, state), Error);
  BOOST_CHECK_THROW(rule.check(tlv::Interest, tlv::SignatureSha256WithRsa,
                               this->pktName, "/foo/bar", state), Error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Constructor, PktType, PktTypes, RuleFixture<PktType>)
{
  BOOST_CHECK_EQUAL(this->rule.getId(), this->ruleId);
  BOOST_CHECK_EQUAL(this->rule.getPktType(), PktType::getType());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(EmptyRule, PktType, PktTypes, RuleFixture<PktType>)
{
  BOOST_CHECK_EQUAL(this->rule.match(PktType::getType(), this->pktName, this->state), true);
  BOOST_CHECK_EQUAL(this->rule.check(PktType::getType(), tlv::SignatureSha256WithRsa,
                                     this->pktName, "/foo/bar", this->state), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Filters, PktType, PktTypes, RuleFixture<PktType>)
{
  this->rule.addFilter(make_unique<RegexNameFilter>(Regex("^<foo><bar>$")));

  BOOST_CHECK_EQUAL(this->rule.match(PktType::getType(), this->pktName, this->state), true);
  BOOST_CHECK_EQUAL(this->rule.match(PktType::getType(), "/not" + this->pktName.toUri(), this->state), false);

  this->rule.addFilter(make_unique<RegexNameFilter>(Regex("^<not><foo><bar>$")));

  BOOST_CHECK_EQUAL(this->rule.match(PktType::getType(), this->pktName, this->state), true);
  BOOST_CHECK_EQUAL(this->rule.match(PktType::getType(), "/not" + this->pktName.toUri(), this->state), true);

  BOOST_CHECK_EQUAL(this->rule.check(PktType::getType(), tlv::SignatureSha256WithRsa,
                                     this->pktName, "/foo/bar", this->state), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Checkers, PktType, PktTypes, RuleFixture<PktType>)
{
  auto testChecker = [this] (const Name& klName, bool expectedOutcome) {
    BOOST_TEST_CONTEXT(klName << " expected=" << expectedOutcome) {
      this->state = PktType::makeState(); // reset state
      BOOST_CHECK_EQUAL(this->rule.check(PktType::getType(), tlv::SignatureSha256WithRsa,
                                         this->pktName, klName, this->state),
                        expectedOutcome);

      auto outcome = this->state->getOutcome();
      if (expectedOutcome) {
        BOOST_CHECK(boost::logic::indeterminate(outcome));
      }
      else {
        BOOST_CHECK(!boost::logic::indeterminate(outcome));
        BOOST_CHECK(!bool(outcome));
      }
    }
  };

  this->rule.addChecker(make_unique<HyperRelationChecker>(tlv::SignatureSha256WithRsa,
                                                          "^(<>+)$", "\\1",
                                                          "^<always>(<>+)$", "\\1",
                                                          NameRelation::EQUAL));
  testChecker("/always/foo/bar", true);
  testChecker("/seldomly/foo/bar", false);
  testChecker("/never/foo/bar", false);

  this->rule.addChecker(make_unique<HyperRelationChecker>(tlv::SignatureSha256WithRsa,
                                                          "^(<>+)$", "\\1",
                                                          "^<seldomly>(<>+)$", "\\1",
                                                          NameRelation::EQUAL));
  testChecker("/always/foo/bar", true);
  testChecker("/seldomly/foo/bar", true);
  testChecker("/never/foo/bar", false);
}

BOOST_AUTO_TEST_SUITE(Create)

BOOST_AUTO_TEST_CASE(Errors)
{
  BOOST_CHECK_THROW(Rule::create(makeSection(""), "test-config"), Error);

  std::string config = R"CONF(
      id rule-id
      for something
    )CONF";
  BOOST_CHECK_THROW(Rule::create(makeSection(config), "test-config"), Error);

  config = R"CONF(
      id rule-id
      for data
    )CONF";
  BOOST_CHECK_THROW(Rule::create(makeSection(config), "test-config"), Error); // at least one checker required

  config = R"CONF(
      id rule-id
      for data
      checker
      {
        type hierarchical
        sig-type rsa-sha256
      }
      other stuff
    )CONF";
  BOOST_CHECK_THROW(Rule::create(makeSection(config), "test-config"), Error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(FilterAndChecker, PktType, PktTypes, RuleFixture<PktType>)
{
  std::string config = R"CONF(
      id rule-id
      for )CONF" + (PktType::getType() == tlv::Data ? "data"s : "interest"s) + R"CONF(
      filter
      {
        type name
        regex ^<foo><bar>$
      }
      checker
      {
        type customized
        sig-type rsa-sha256
        key-locator
        {
          type name
          hyper-relation
          {
            k-regex ^(<>+)$
            k-expand \\1
            h-relation equal
            p-regex ^(<>+)$
            p-expand \\1
          }
        }
      }
    )CONF";
  auto rule = Rule::create(makeSection(config), "test-config");

  BOOST_CHECK(rule->match(PktType::getType(), this->pktName, this->state));
  BOOST_CHECK(!rule->match(PktType::getType(), "/not" + this->pktName.toUri(), this->state));

  BOOST_CHECK(rule->check(PktType::getType(), tlv::SignatureSha256WithRsa, this->pktName, "/foo/bar", this->state));
  BOOST_CHECK(!rule->check(PktType::getType(), tlv::SignatureSha256WithEcdsa, this->pktName, "/foo/bar", this->state));

  this->state = PktType::makeState(); // reset state
  BOOST_CHECK(!rule->check(PktType::getType(), tlv::SignatureSha256WithRsa, this->pktName, "/not/foo/bar", this->state));
}

BOOST_AUTO_TEST_SUITE_END() // Create

BOOST_AUTO_TEST_SUITE_END() // TestRule
BOOST_AUTO_TEST_SUITE_END() // ValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace validator_config
} // inline namespace v2
} // namespace security
} // namespace ndn
