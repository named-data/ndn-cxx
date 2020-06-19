/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#include "ndn-cxx/security/validator-config/checker.hpp"
#include "ndn-cxx/security/command-interest-signer.hpp"
#include "ndn-cxx/security/validation-policy.hpp"
#include "ndn-cxx/security/validation-state.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/validator-fixture.hpp"
#include "tests/unit/security/validator-config/common.hpp"

namespace ndn {
namespace security {
inline namespace v2 {
namespace validator_config {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(ValidatorConfig)

class CheckerFixture : public IdentityManagementFixture
{
public:
  CheckerFixture()
  {
    names.push_back("/foo/bar");
    names.push_back("/foo/bar/bar");
    names.push_back("/foo");
    names.push_back("/other/prefix");
  }

  static Name
  makeKeyLocatorName(const Name& name)
  {
    return Name(name).append("KEY").append("v=1");
  }

public:
  std::vector<Name> names;
};

BOOST_FIXTURE_TEST_SUITE(TestChecker, CheckerFixture)

class NameRelationEqual : public CheckerFixture
{
public:
  NameRelationEqual()
    : checker("/foo/bar", NameRelation::EQUAL)
  {
  }

public:
  NameRelationChecker checker;
  std::vector<std::vector<bool>> outcomes = {{true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false}};
};

class NameRelationIsPrefixOf : public CheckerFixture
{
public:
  NameRelationIsPrefixOf()
    : checker("/foo/bar", NameRelation::IS_PREFIX_OF)
  {
  }

public:
  NameRelationChecker checker;
  std::vector<std::vector<bool>> outcomes = {{true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false}};
};

class NameRelationIsStrictPrefixOf : public CheckerFixture
{
public:
  NameRelationIsStrictPrefixOf()
    : checker("/foo/bar", NameRelation::IS_STRICT_PREFIX_OF)
  {
  }

public:
  NameRelationChecker checker;
  std::vector<std::vector<bool>> outcomes = {{false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false}};
};

class RegexEqual : public CheckerFixture
{
public:
  RegexEqual()
    : checker(Regex("^<foo><bar><KEY><>$"))
  {
  }

public:
  RegexChecker checker;
  std::vector<std::vector<bool>> outcomes = {{true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false}};
};

class RegexIsPrefixOf : public CheckerFixture
{
public:
  RegexIsPrefixOf()
    : checker(Regex("^<foo><bar><>*<KEY><>$"))
  {
  }

public:
  RegexChecker checker;
  std::vector<std::vector<bool>> outcomes = {{true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false}};
};

class RegexIsStrictPrefixOf : public CheckerFixture
{
public:
  RegexIsStrictPrefixOf()
    : checker(Regex("^<foo><bar><>+<KEY><>$"))
  {
  }

public:
  RegexChecker checker;
  std::vector<std::vector<bool>> outcomes = {{false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false}};
};

class HyperRelationEqual : public CheckerFixture
{
public:
  HyperRelationEqual()
    : checker("^(<>+)$", "\\1", "^(<>+)<KEY><>$", "\\1", NameRelation::EQUAL)
  {
  }

public:
  HyperRelationChecker checker;
  std::vector<std::vector<bool>> outcomes = {{true,  false, false, false},
                                             {false, true,  false, false},
                                             {false, false, true,  false},
                                             {false, false, false, true}};
};

class HyperRelationIsPrefixOf : public CheckerFixture
{
public:
  HyperRelationIsPrefixOf()
    : checker("^(<>+)$", "\\1", "^(<>+)<KEY><>$", "\\1", NameRelation::IS_PREFIX_OF)
  {
  }

public:
  HyperRelationChecker checker;
  std::vector<std::vector<bool>> outcomes = {{true,  false, true,  false},
                                             {true,  true,  true,  false},
                                             {false, false, true,  false},
                                             {false, false, false, true}};
};

class HyperRelationIsStrictPrefixOf : public CheckerFixture
{
public:
  HyperRelationIsStrictPrefixOf()
    : checker("^(<>+)$", "\\1", "^(<>+)<KEY><>$", "\\1", NameRelation::IS_STRICT_PREFIX_OF)
  {
  }

public:
  HyperRelationChecker checker;
  std::vector<std::vector<bool>> outcomes = {{false, false, true,  false},
                                             {true,  false, true,  false},
                                             {false, false, false, false},
                                             {false, false, false, false}};
};

class Hierarchical : public CheckerFixture
{
public:
  Hierarchical()
    : checkerPtr(Checker::create(makeSection(R"CONF(
          type hierarchical
          sig-type rsa-sha256
        )CONF"), "test-config"))
    , checker(*checkerPtr)
  {
  }

public:
  std::unique_ptr<Checker> checkerPtr;
  Checker& checker;

  std::vector<std::vector<bool>> outcomes = {{true,  false, true,  false},
                                             {true,  true,  true,  false},
                                             {false, false, true,  false},
                                             {false, false, false, true}};
};

class CustomizedNameRelation : public CheckerFixture
{
public:
  CustomizedNameRelation()
    : checkerPtr(Checker::create(makeSection(R"CONF(
          type customized
          sig-type rsa-sha256
          key-locator
          {
            type name
            name /foo/bar
            relation equal
          }
        )CONF"), "test-config"))
    , checker(*checkerPtr)
  {
  }

public:
  std::unique_ptr<Checker> checkerPtr;
  Checker& checker;

  std::vector<std::vector<bool>> outcomes = {{true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false}};
};

class CustomizedRegex : public CheckerFixture
{
public:
  CustomizedRegex()
    : checkerPtr(Checker::create(makeSection(R"CONF(
          type customized
          sig-type rsa-sha256
          key-locator
          {
            type name
            regex ^<foo><bar><KEY><>$
          }
        )CONF"), "test-config"))
    , checker(*checkerPtr)
  {
  }

public:
  std::unique_ptr<Checker> checkerPtr;
  Checker& checker;

  std::vector<std::vector<bool>> outcomes = {{true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false}};
};

class CustomizedHyperRelation : public CheckerFixture
{
public:
  CustomizedHyperRelation()
    : checkerPtr(Checker::create(makeSection(R"CONF(
          type customized
          sig-type rsa-sha256
          key-locator
          {
            type name
            hyper-relation
            {
              k-regex ^(<>+)<KEY><>$
              k-expand \\1
              h-relation is-prefix-of
              p-regex ^(<>+)$
              p-expand \\1
            }
          }
        )CONF"), "test-config"))
    , checker(*checkerPtr)
  {
  }

public:
  std::unique_ptr<Checker> checkerPtr;
  Checker& checker;

  std::vector<std::vector<bool>> outcomes = {{true,  false, true,  false},
                                             {true,  true,  true,  false},
                                             {false, false, true,  false},
                                             {false, false, false, true}};
};

using Tests = boost::mpl::vector<NameRelationEqual, NameRelationIsPrefixOf, NameRelationIsStrictPrefixOf,
                                 RegexEqual, RegexIsPrefixOf, RegexIsStrictPrefixOf,
                                 HyperRelationEqual, HyperRelationIsPrefixOf, HyperRelationIsStrictPrefixOf,
                                 Hierarchical,
                                 CustomizedNameRelation, CustomizedRegex, CustomizedHyperRelation>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(DataChecks, T, Tests, T)
{
  using namespace ndn::security::v2::tests;
  using PktType = DataPkt;

  BOOST_REQUIRE_EQUAL(this->outcomes.size(), this->names.size());
  for (size_t i = 0; i < this->names.size(); ++i) {
    BOOST_REQUIRE_EQUAL(this->outcomes[i].size(), this->names.size());
    for (size_t j = 0; j < this->names.size(); ++j) {
      auto pktName = PktType::makeName(this->names[i], this->m_keyChain);
      auto klName = this->makeKeyLocatorName(this->names[j]);
      bool expectedOutcome = this->outcomes[i][j];

      auto state = PktType::makeState();
      BOOST_CHECK_EQUAL(this->checker.check(PktType::getType(), pktName, klName, state), expectedOutcome);
      BOOST_CHECK_EQUAL(boost::logic::indeterminate(state->getOutcome()), expectedOutcome);
      BOOST_CHECK_EQUAL(bool(state->getOutcome()), false);
    }
  }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(InterestV02Checks, T, Tests, T)
{
  using namespace ndn::security::v2::tests;
  using PktType = InterestV02Pkt;

  BOOST_REQUIRE_EQUAL(this->outcomes.size(), this->names.size());
  for (size_t i = 0; i < this->names.size(); ++i) {
    BOOST_REQUIRE_EQUAL(this->outcomes[i].size(), this->names.size());
    for (size_t j = 0; j < this->names.size(); ++j) {
      auto pktName = PktType::makeName(this->names[i], this->m_keyChain);
      auto klName = this->makeKeyLocatorName(this->names[j]);
      bool expectedOutcome = this->outcomes[i][j];

      auto state = PktType::makeState();
      BOOST_CHECK_EQUAL(this->checker.check(PktType::getType(), pktName, klName, state), expectedOutcome);
      BOOST_CHECK_EQUAL(boost::logic::indeterminate(state->getOutcome()), expectedOutcome);
      BOOST_CHECK_EQUAL(bool(state->getOutcome()), false);
    }
  }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(InterestV03Checks, T, Tests, T)
{
  using namespace ndn::security::v2::tests;
  using PktType = InterestV03Pkt;

  BOOST_REQUIRE_EQUAL(this->outcomes.size(), this->names.size());
  for (size_t i = 0; i < this->names.size(); ++i) {
    BOOST_REQUIRE_EQUAL(this->outcomes[i].size(), this->names.size());
    for (size_t j = 0; j < this->names.size(); ++j) {
      auto pktName = PktType::makeName(this->names[i], this->m_keyChain);
      auto klName = this->makeKeyLocatorName(this->names[j]);
      bool expectedOutcome = this->outcomes[i][j];

      auto state = PktType::makeState();
      BOOST_CHECK_EQUAL(this->checker.check(PktType::getType(), pktName, klName, state), expectedOutcome);
      BOOST_CHECK_EQUAL(boost::logic::indeterminate(state->getOutcome()), expectedOutcome);
      BOOST_CHECK_EQUAL(bool(state->getOutcome()), false);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestChecker
BOOST_AUTO_TEST_SUITE_END() // ValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace validator_config
} // inline namespace v2
} // namespace security
} // namespace ndn
