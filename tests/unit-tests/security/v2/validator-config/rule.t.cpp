/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "security/v2/validator-config/rule.hpp"

#include "boost-test.hpp"
#include "common.hpp"
#include "identity-management-fixture.hpp"
#include "../validator-fixture.hpp"

#include <boost/mpl/vector_c.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {
namespace tests {

using namespace ndn::tests;
using namespace ndn::security::v2::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(ValidatorConfig)

template<uint32_t PktType>
class RuleFixture : public IdentityManagementFixture
{
public:
  RuleFixture()
    : rule(ruleId, PktType)
    , pktName("/foo/bar")
  {
    if (PktType == tlv::Interest) {
      pktName = Name("/foo/bar/SigInfo/SigValue");
    }
  }

public:
  const std::string ruleId = "rule-id";
  Rule rule;
  Name pktName;
};

using PktTypes = boost::mpl::vector_c<uint32_t, tlv::Data, tlv::Interest>;

BOOST_AUTO_TEST_SUITE(TestRule)

BOOST_FIXTURE_TEST_CASE(Errors, RuleFixture<tlv::Data>)
{
  BOOST_CHECK_THROW(rule.match(tlv::Interest, this->pktName), Error);

  auto state = make_shared<DummyValidationState>();
  BOOST_CHECK_THROW(rule.check(tlv::Interest, this->pktName, "/foo/bar", state), Error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Constructor, PktType, PktTypes, RuleFixture<PktType::value>)
{
  BOOST_CHECK_EQUAL(this->rule.getId(), this->ruleId);
  BOOST_CHECK_EQUAL(this->rule.getPktType(), PktType::value);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(EmptyRule, PktType, PktTypes, RuleFixture<PktType::value>)
{
  BOOST_CHECK_EQUAL(this->rule.match(PktType::value, this->pktName), true);

  auto state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(this->rule.check(PktType::value, this->pktName, "/foo/bar", state), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Filters, PktType, PktTypes, RuleFixture<PktType::value>)
{
  this->rule.addFilter(make_unique<RegexNameFilter>(Regex("^<foo><bar>$")));

  BOOST_CHECK_EQUAL(this->rule.match(PktType::value, this->pktName), true);
  BOOST_CHECK_EQUAL(this->rule.match(PktType::value, "/not" + this->pktName.toUri()), false);

  this->rule.addFilter(make_unique<RegexNameFilter>(Regex("^<not><foo><bar>$")));

  BOOST_CHECK_EQUAL(this->rule.match(PktType::value, this->pktName), true);
  BOOST_CHECK_EQUAL(this->rule.match(PktType::value, "/not" + this->pktName.toUri()), true);

  auto state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(this->rule.check(PktType::value, this->pktName, "/foo/bar", state), false);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Checkers, PktType, PktTypes, RuleFixture<PktType::value>)
{
  this->rule.addChecker(make_unique<HyperRelationChecker>("^(<>+)$", "\\1",
                                                        "^<not>?(<>+)$", "\\1",
                                                        NameRelation::EQUAL));

  auto state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(this->rule.check(PktType::value, this->pktName, "/foo/bar", state), true);

  state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(this->rule.check(PktType::value, this->pktName, "/not/foo/bar", state), true);

  this->rule.addChecker(make_unique<HyperRelationChecker>("^(<>+)$", "\\1",
                                                        "^(<>+)$", "\\1",
                                                        NameRelation::EQUAL));
  state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(this->rule.check(PktType::value, this->pktName, "/foo/bar", state), true);

  state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(this->rule.check(PktType::value, this->pktName, "/not/foo/bar", state), false);
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

BOOST_FIXTURE_TEST_CASE_TEMPLATE(FilterAndChecker, PktType, PktTypes, RuleFixture<PktType::value>)
{
  std::string config = std::string("") + R"CONF(
      id rule-id
      for )CONF" + (PktType::value == tlv::Data ? "data" : "interest") + R"CONF(
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

  BOOST_CHECK_EQUAL(rule->match(PktType::value, this->pktName), true);
  BOOST_CHECK_EQUAL(rule->match(PktType::value, "/not" + this->pktName.toUri()), false);

  auto state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(rule->check(PktType::value, this->pktName, "/foo/bar", state), true);

  state = make_shared<DummyValidationState>();
  BOOST_CHECK_EQUAL(rule->check(PktType::value, this->pktName, "/not/foo/bar", state), false);
}

BOOST_AUTO_TEST_SUITE_END() // Create

BOOST_AUTO_TEST_SUITE_END() // TestRule
BOOST_AUTO_TEST_SUITE_END() // ValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn
