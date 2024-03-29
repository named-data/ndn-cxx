/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#include "tests/boost-test.hpp"
#include "tests/unit/security/validator-fixture.hpp"
#include "tests/unit/security/validator-config/common.hpp"

#include <boost/mp11/algorithm.hpp>

namespace ndn::tests {

using namespace ndn::security::validator_config;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(ValidatorConfig)
BOOST_AUTO_TEST_SUITE(TestChecker)

class CheckerFixture : public KeyChainFixture
{
public:
  CheckerFixture()
  {
    names.emplace_back("/foo/bar");
    names.emplace_back("/foo/bar/bar");
    names.emplace_back("/foo");
    names.emplace_back("/other/prefix");
  }

  static Name
  makeKeyLocatorKeyName(const Name& name)
  {
    static PartialName suffix("KEY/keyid");
    return Name(name).append(suffix);
  }

  static Name
  makeKeyLocatorCertName(const Name& name)
  {
    static PartialName suffix("KEY/keyid/issuer/v=1");
    return Name(name).append(suffix);
  }

public:
  std::vector<Name> names;
};

class NameRelationEqual : public CheckerFixture
{
public:
  NameRelationChecker checker{tlv::SignatureSha256WithRsa, "/foo/bar", NameRelation::EQUAL};
  std::vector<std::vector<bool>> outcomes = {{true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false}};
};

class NameRelationIsPrefixOf : public CheckerFixture
{
public:
  NameRelationChecker checker{tlv::SignatureSha256WithRsa, "/foo/bar", NameRelation::IS_PREFIX_OF};
  std::vector<std::vector<bool>> outcomes = {{true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false}};
};

class NameRelationIsStrictPrefixOf : public CheckerFixture
{
public:
  NameRelationChecker checker{tlv::SignatureSha256WithRsa, "/foo/bar", NameRelation::IS_STRICT_PREFIX_OF};
  std::vector<std::vector<bool>> outcomes = {{false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false}};
};

class RegexEqual : public CheckerFixture
{
public:
  RegexChecker checker{tlv::SignatureSha256WithRsa, Regex("^<foo><bar><KEY><>{1,3}$")};
  std::vector<std::vector<bool>> outcomes = {{true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false},
                                             {true, false, false, false}};
};

class RegexIsPrefixOf : public CheckerFixture
{
public:
  RegexChecker checker{tlv::SignatureSha256WithRsa, Regex("^<foo><bar><>*<KEY><>{1,3}$")};
  std::vector<std::vector<bool>> outcomes = {{true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false},
                                             {true, true, false, false}};
};

class RegexIsStrictPrefixOf : public CheckerFixture
{
public:
  RegexChecker checker{tlv::SignatureSha256WithRsa, Regex("^<foo><bar><>+<KEY><>{1,3}$")};
  std::vector<std::vector<bool>> outcomes = {{false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false},
                                             {false, true, false, false}};
};

class HyperRelationEqual : public CheckerFixture
{
public:
  HyperRelationChecker checker{tlv::SignatureSha256WithRsa,
                               "^(<>+)$", "\\1", "^(<>+)<KEY><>{1,3}$", "\\1", NameRelation::EQUAL};
  std::vector<std::vector<bool>> outcomes = {{true,  false, false, false},
                                             {false, true,  false, false},
                                             {false, false, true,  false},
                                             {false, false, false, true}};
};

class HyperRelationIsPrefixOf : public CheckerFixture
{
public:
  HyperRelationChecker checker{tlv::SignatureSha256WithRsa,
                               "^(<>+)$", "\\1", "^(<>+)<KEY><>{1,3}$", "\\1", NameRelation::IS_PREFIX_OF};
  std::vector<std::vector<bool>> outcomes = {{true,  false, true,  false},
                                             {true,  true,  true,  false},
                                             {false, false, true,  false},
                                             {false, false, false, true}};
};

class HyperRelationIsStrictPrefixOf : public CheckerFixture
{
public:
  HyperRelationChecker checker{tlv::SignatureSha256WithRsa,
                               "^(<>+)$", "\\1", "^(<>+)<KEY><>{1,3}$", "\\1", NameRelation::IS_STRICT_PREFIX_OF};
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
            regex ^<foo><bar><KEY><>{1,3}$
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
              k-regex ^(<>+)<KEY><>{1,3}$
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

using CheckerFixtures = boost::mp11::mp_list<
  NameRelationEqual,
  NameRelationIsPrefixOf,
  NameRelationIsStrictPrefixOf,
  RegexEqual,
  RegexIsPrefixOf,
  RegexIsStrictPrefixOf,
  HyperRelationEqual,
  HyperRelationIsPrefixOf,
  HyperRelationIsStrictPrefixOf,
  Hierarchical,
  CustomizedNameRelation,
  CustomizedRegex,
  CustomizedHyperRelation
>;

// Cartesian product of [DataPkt, InterestV02Pkt, InterestV03Pkt] and CheckerFixtures.
// Each element is an mp_list<PktType, Fixture>.
using Tests = boost::mp11::mp_product<
  boost::mp11::mp_list,
  boost::mp11::mp_list<DataPkt, InterestV02Pkt, InterestV03Pkt>,
  CheckerFixtures
>;

template<typename PktType, typename C>
static void
testChecker(C& checker, tlv::SignatureTypeValue sigType, const Name& pktName, const Name& klName, bool expectedOutcome)
{
  BOOST_TEST_INFO_SCOPE("Packet = " << pktName);
  BOOST_TEST_INFO_SCOPE("SignatureType = " << sigType);
  BOOST_TEST_INFO_SCOPE("KeyLocator = " << klName);

  auto state = PktType::makeState();
  auto result = checker.check(PktType::getType(), sigType, pktName, klName, *state);
  BOOST_TEST(bool(result) == expectedOutcome);
  BOOST_TEST(boost::logic::indeterminate(state->getOutcome()));
  if (!result) {
    BOOST_TEST(!result.getErrorMessage().empty());
  }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Checks, T, Tests, boost::mp11::mp_second<T>)
{
  using PktType = boost::mp11::mp_first<T>;

  BOOST_REQUIRE_EQUAL(this->outcomes.size(), this->names.size());
  for (size_t i = 0; i < this->names.size(); ++i) {
    BOOST_REQUIRE_EQUAL(this->outcomes[i].size(), this->names.size());

    auto pktName = PktType::makeName(this->names[i], this->m_keyChain);
    for (size_t j = 0; j < this->names.size(); ++j) {
      bool expectedOutcome = this->outcomes[i][j];

      auto klName = this->makeKeyLocatorKeyName(this->names[j]);
      testChecker<PktType>(this->checker, tlv::SignatureSha256WithRsa, pktName, klName, expectedOutcome);
      testChecker<PktType>(this->checker, tlv::SignatureSha256WithEcdsa, pktName, klName, false);

      klName = this->makeKeyLocatorCertName(this->names[j]);
      testChecker<PktType>(this->checker, tlv::SignatureSha256WithRsa, pktName, klName, expectedOutcome);
      testChecker<PktType>(this->checker, tlv::SignatureSha256WithEcdsa, pktName, klName, false);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestChecker
BOOST_AUTO_TEST_SUITE_END() // ValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
