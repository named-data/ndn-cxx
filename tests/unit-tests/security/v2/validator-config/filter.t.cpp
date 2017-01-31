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

#include "security/v2/validator-config/filter.hpp"
#include "security/command-interest-signer.hpp"

#include "boost-test.hpp"
#include "common.hpp"
#include "identity-management-fixture.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(ValidatorConfig)

class FilterFixture : public IdentityManagementFixture
{
public:
  Interest
  makeSignedInterest(const Name& name)
  {
    Interest interest(name);
    m_keyChain.sign(interest);
    return interest;
  }
};

BOOST_FIXTURE_TEST_SUITE(TestFilter, FilterFixture)

#define CHECK_FOR_MATCHES(filter, same, longer, shorter, different)     \
  BOOST_CHECK_EQUAL(filter.match(tlv::Interest, makeSignedInterest("/foo/bar").getName()), same); \
  BOOST_CHECK_EQUAL(filter.match(tlv::Data, Data("/foo/bar").getName()), same);   \
  BOOST_CHECK_EQUAL(filter.match(tlv::Interest, makeSignedInterest("/foo/bar/bar").getName()), longer); \
  BOOST_CHECK_EQUAL(filter.match(tlv::Data, Data("/foo/bar/bar").getName()), longer);       \
  BOOST_CHECK_EQUAL(filter.match(tlv::Interest, makeSignedInterest("/foo").getName()), shorter); \
  BOOST_CHECK_EQUAL(filter.match(tlv::Data, Data("/foo").getName()), shorter);              \
  BOOST_CHECK_EQUAL(filter.match(tlv::Interest, makeSignedInterest("/other/prefix").getName()), different); \
  BOOST_CHECK_EQUAL(filter.match(tlv::Data, Data("/other/prefix").getName()), different);

BOOST_AUTO_TEST_CASE(RelationName)
{
  RelationNameFilter f1("/foo/bar", NameRelation::EQUAL);
  CHECK_FOR_MATCHES(f1, true, false, false, false);

  RelationNameFilter f2("/foo/bar", NameRelation::IS_PREFIX_OF);
  CHECK_FOR_MATCHES(f2, true, true, false, false);

  RelationNameFilter f3("/foo/bar", NameRelation::IS_STRICT_PREFIX_OF);
  CHECK_FOR_MATCHES(f3, false, true, false, false);
}

BOOST_AUTO_TEST_CASE(RegexName)
{
  RegexNameFilter f1(Regex("^<foo><bar>$"));
  CHECK_FOR_MATCHES(f1, true, false, false, false);

  RegexNameFilter f2(Regex("^<foo><bar><>*$"));
  CHECK_FOR_MATCHES(f2, true, true, false, false);

  RegexNameFilter f3(Regex("^<foo><bar><>+$"));
  CHECK_FOR_MATCHES(f3, false, true, false, false);
}

BOOST_FIXTURE_TEST_SUITE(Create, FilterFixture)

BOOST_AUTO_TEST_CASE(Errors)
{
  BOOST_CHECK_THROW(Filter::create(makeSection(""), "test-config"), Error);
  BOOST_CHECK_THROW(Filter::create(makeSection("type unknown"), "test-config"), Error);
  BOOST_CHECK_THROW(Filter::create(makeSection("type name"), "test-config"), Error);

  std::string config = R"CONF(
      type name
      not-name-or-regex stuff
    )CONF";
  BOOST_CHECK_THROW(Filter::create(makeSection(config), "test-config"), Error);

  config = R"CONF(
      type name
      name /foo/bar
    )CONF";
  BOOST_CHECK_THROW(Filter::create(makeSection(config), "test-config"), Error);

  config = R"CONF(
      type name
      name /foo/bar
      not-relation stuff
    )CONF";
  BOOST_CHECK_THROW(Filter::create(makeSection(config), "test-config"), Error);

  config = R"CONF(
      type name
      name /foo/bar
      relation equal
      not-end stuff
    )CONF";
  BOOST_CHECK_THROW(Filter::create(makeSection(config), "test-config"), Error);

  config = R"CONF(
      type name
      regex ^<foo><bar>$
      not-end stuff
    )CONF";
  BOOST_CHECK_THROW(Filter::create(makeSection(config), "test-config"), Error);
}

BOOST_AUTO_TEST_CASE(NameFilter)
{
  std::string config = R"CONF(
      type name
      name /foo/bar
      relation equal
    )CONF";
  auto f1 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f1), true, false, false, false);

  config = R"CONF(
      type name
      name /foo/bar
      relation is-prefix-of
    )CONF";
  auto f2 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f2), true, true, false, false);

  config = R"CONF(
      type name
      name /foo/bar
      relation is-strict-prefix-of
    )CONF";
  auto f3 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f3), false, true, false, false);
}

BOOST_AUTO_TEST_CASE(RegexFilter)
{
  std::string config = R"CONF(
      type name
      regex ^<foo><bar>$
    )CONF";
  auto f1 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f1), true, false, false, false);

  config = R"CONF(
      type name
      regex ^<foo><bar><>*$
    )CONF";
  auto f2 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f2), true, true, false, false);

  config = R"CONF(
      type name
      regex ^<foo><bar><>+$
    )CONF";
  auto f3 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f3), false, true, false, false);

  config = R"CONF(
      type name
      regex ^<>*$
    )CONF";
  auto f4 = Filter::create(makeSection(config), "test-config");
  CHECK_FOR_MATCHES((*f4), true, true, true, true);
}

BOOST_AUTO_TEST_SUITE_END() // Create

BOOST_AUTO_TEST_SUITE_END() // TestFilter
BOOST_AUTO_TEST_SUITE_END() // ValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn
