/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "security/validator-config.hpp"

#include "security/signing-helpers.hpp"
#include "util/io.hpp"
#include "util/scheduler.hpp"
#include "util/dummy-client-face.hpp"
#include "lp/tags.hpp"
#include "lp/nack.hpp"

#include <boost/asio.hpp>
#include <boost/logic/tribool.hpp>

#include "identity-management-fixture.hpp"
#include "../identity-management-time-fixture.hpp"
#include "../make-interest-data.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)

// Needed to create Face instance
class ValidatorConfigFixture : public IdentityManagementV1TimeFixture
{
public:
  ValidatorConfigFixture()
    : m_v2KeyChain("pib-memory:", "tpm-memory:")
    , face(nullptr, m_v2KeyChain)
    , validator(face)
  {
  }

public:
  v2::KeyChain m_v2KeyChain;
  Face face;
  ValidatorConfig validator;
};

BOOST_FIXTURE_TEST_SUITE(TestValidatorConfig, ValidatorConfigFixture)

BOOST_AUTO_TEST_CASE(NameFilter)
{
  Name identity("/TestValidatorConfig/NameFilter");
  identity.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity), "trust-anchor-1.cert"));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name dataName1("/simple/equal");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity)));

  Name dataName2("/simple/different");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity)));

  std::string CONFIG_1 =
    "rule\n"
    "{\n"
    "  id \"Simple Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /simple/equal\n"
    "    relation equal\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      name ";

  std::string CONFIG_2 =
    "\n"
    "      relation equal\n"
    "    }\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-1.cert\"\n"
    "}\n";
  const std::string CONFIG = CONFIG_1 + certName.getPrefix(-1).toUri() + CONFIG_2;

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });
}

BOOST_AUTO_TEST_CASE(NameFilter2)
{
  Name identity("/TestValidatorConfig/NameFilter2");
  identity.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity), "trust-anchor-2.cert"));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name dataName1("/simple/isPrefixOf");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity)));

  Name dataName2("/simple/notPrefixOf");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity)));

  Name dataName3("/simple/isPrefixOf/anotherLevel");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3, security::signingByIdentity(identity)));

  std::string CONFIG_1 =
    "rule\n"
    "{\n"
    "  id \"Simple2 Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /simple/isPrefixOf\n"
    "    relation is-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      name ";

  std::string CONFIG_2 =
    "\n"
    "      relation equal\n"
    "    }\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-2.cert\"\n"
    "}\n";
  const std::string CONFIG = CONFIG_1 + certName.getPrefix(-1).toUri() + CONFIG_2;

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
}

BOOST_AUTO_TEST_CASE(NameFilter3)
{
  Name identity("/TestValidatorConfig/NameFilter3");
  identity.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity), "trust-anchor-3.cert"));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name dataName1("/simple/isStrictPrefixOf");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity)));

  Name dataName2("/simple");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity)));

  Name dataName3("/simple/isStrictPrefixOf/anotherLevel");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3, security::signingByIdentity(identity)));

  std::string CONFIG_1 =
    "rule\n"
    "{\n"
    "  id \"Simple3 Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /simple/isStrictPrefixOf\n"
    "    relation is-strict-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      name ";

  std::string CONFIG_2 =
    "\n"
    "      relation equal\n"
    "    }\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-3.cert\"\n"
    "}\n";
  const std::string CONFIG = CONFIG_1 + certName.getPrefix(-1).toUri() + CONFIG_2;

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
}

BOOST_AUTO_TEST_CASE(NameFilter4)
{
  Name identity("/TestValidatorConfig/NameFilter4");
  identity.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity), "trust-anchor-4.cert"));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name dataName1("/simple/regex");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity)));

  Name dataName2("/simple/regex-wrong");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity)));

  Name dataName3("/simple/regex/correct");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3, security::signingByIdentity(identity)));

  std::string CONFIG_1 =
    "rule\n"
    "{\n"
    "  id \"Simple3 Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    regex ^<simple><regex>\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      name ";

  std::string CONFIG_2 =
    "\n"
    "      relation equal\n"
    "    }\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-4.cert\"\n"
    "}\n";
  const std::string CONFIG = CONFIG_1 + certName.getPrefix(-1).toUri() + CONFIG_2;

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
}

BOOST_AUTO_TEST_CASE(KeyLocatorNameChecker1)
{
  Name identity("/TestValidatorConfig/KeyLocatorNameChecker1");
  identity.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity), "trust-anchor-5.cert"));

  Name dataName1 = identity;
  dataName1.append("1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity)));

  Name dataName2 = identity;
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity)));

  Name dataName3("/TestValidatorConfig/KeyLocatorNameChecker1");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3, security::signingByIdentity(identity)));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"Simple3 Rule\"\n"
    "  for data\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      hyper-relation\n"
    "      {\n"
    "        k-regex ^([^<KEY>]*)<KEY>(<>*)<><ID-CERT>$\n"
    "        k-expand \\\\1\\\\2\n"
    "        h-relation is-strict-prefix-of\n"
    "        p-regex ^(<>*)$\n"
    "        p-expand \\\\1\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-5.cert\"\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });
}

BOOST_AUTO_TEST_CASE(FixedSignerChecker)
{
  Name identity("/TestValidatorConfig/FixedSignerChecker");

  Name identity1 = identity;
  identity1.append("1").appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity1), "trust-anchor-7.cert"));

  Name identity2 = identity;
  identity2.append("2").appendVersion();
  addIdentity(identity2);

  Name dataName1 = identity;
  dataName1.append("data").appendVersion();
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity1)));

  Name dataName2 = identity;
  dataName2.append("data").appendVersion();
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity2)));

  Name interestName("/TestValidatorConfig/FixedSignerChecker/fakeSigInfo/fakeSigValue");
  shared_ptr<Interest> interest = make_shared<Interest>(interestName);

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Data Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/FixedSignerChecker\n"
    "    relation is-strict-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type rsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-7.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n"
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Interest Rule\"\n"
    "  for interest\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/FixedSignerChecker\n"
    "    relation is-strict-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type rsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-7.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*interest,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });
}

BOOST_AUTO_TEST_CASE(MultiCheckers)
{
  Name identity1("/TestValidatorConfig/MultiCheckers/");
  identity1.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity1), "trust-anchor-multi-1.cert"));

  Name identity2("/TestValidatorConfig/");
  identity2.appendVersion();
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity2), "trust-anchor-multi-2.cert"));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"Simple Rule01\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /\n"
    "    relation is-prefix-of\n"
    "  }\n"
    "  checker\n" // checker 1, signer should have prefix /TestValidatorConfig/MultiCheckers
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      name /TestValidatorConfig/MultiCheckers/\n"
    "      relation is-prefix-of\n"
    "    }\n"
    "  }\n"
    "  checker\n" // checker 2, data should have same prefix of its signer
    "  {\n"
    "    type hierarchical\n"
    "    sig-type rsa-sha256\n"
    "  }\n"
    "  checker\n" // checker 3, the signer should be identity1
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type rsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-multi-1.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n";

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-multicheckers.conf"));

  ValidatorConfig validator;
  validator.load(CONFIG, CONFIG_PATH.c_str());
  conf::Checker& checker0 = *validator.m_dataRules.front()->m_checkers[0];
  conf::Checker& checker1 = *validator.m_dataRules.front()->m_checkers[1];
  conf::Checker& checker2 = *validator.m_dataRules.front()->m_checkers[2];

  auto data1 = makeData(Name(identity1).append("Test"));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity1)));
  BOOST_CHECK_EQUAL(checker0.check(*data1), 0);
  BOOST_CHECK_EQUAL(checker1.check(*data1), 0);
  BOOST_CHECK_EQUAL(checker2.check(*data1), 1);

  auto data2 = makeData(Name(identity2).append("Data2"));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(identity2)));
  BOOST_CHECK_EQUAL(checker0.check(*data2), -1);
  BOOST_CHECK_EQUAL(checker1.check(*data2), 0);
  BOOST_CHECK_EQUAL(checker2.check(*data2), -1);

  auto data3 = makeData(Name(identity2).append("Data3"));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3, security::signingByIdentity(identity1)));
  BOOST_CHECK_EQUAL(checker0.check(*data3), 0);
  BOOST_CHECK_EQUAL(checker1.check(*data3), -1);
  BOOST_CHECK_EQUAL(checker2.check(*data3), 1);

  auto data4 = makeData("/Data4");
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data4, security::signingByIdentity(identity2)));
  BOOST_CHECK_EQUAL(checker0.check(*data4), -1);
  BOOST_CHECK_EQUAL(checker1.check(*data4), -1);
  BOOST_CHECK_EQUAL(checker2.check(*data4), -1);

  int count = 0;
  validator.validate(*data1,
    [&] (const shared_ptr<const Data>&) {
      BOOST_CHECK(true);
      count++;
    },
    [] (const shared_ptr<const Data>&, const std::string& str) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [&] (const shared_ptr<const Data>&, const std::string& str) {
      BOOST_CHECK(true);
      count++;
    });

  validator.validate(*data3,
    [&] (const shared_ptr<const Data>&) {
      BOOST_CHECK(true);
      count++;
    },
    [] (const shared_ptr<const Data>&, const std::string& str) { BOOST_CHECK(false); });

  validator.validate(*data4,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [&] (const shared_ptr<const Data>&, const std::string& str) {
      BOOST_CHECK(true);
      count++;
    });

  BOOST_CHECK_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE(Reset)
{
  Name root("/TestValidatorConfig/Reload");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(root), "trust-anchor-8.cert"));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"NRD Prefix Registration Command Rule\"\n"
    "  for interest\n"
    "  filter\n"
    "  {\n"
    "    type name\n"
    "    regex ^<localhost><nrd>[<register><unregister><advertise><withdraw>]<>$\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT>$\n"
    "    }\n"
    "  }\n"
    "}\n"
    "rule\n"
    "{\n"
    "  id \"Testbed Hierarchy Rule\"\n"
    "  for data\n"
    "  filter\n"
    "  {\n"
    "    type name\n"
    "    regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT><>$\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type hierarchical\n"
    "    sig-type rsa-sha256\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-8.cert\"\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());
  BOOST_CHECK_EQUAL(validator.isEmpty(), false);

  validator.reset();
  BOOST_CHECK(validator.isEmpty());
}

BOOST_AUTO_TEST_CASE(TrustAnchorWildcard)
{
  Name identity("/TestValidatorConfig/Wildcard");
  identity.appendVersion();
  addIdentity(identity);

  Name dataName1("/any/data");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(identity)));

  std::string CONFIG =
    "trust-anchor\n"
    "{\n"
    "  type any\n"
    "}\n";

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
}

BOOST_AUTO_TEST_CASE(SignedInterestTest)
{
  Name certName = addIdentity("/TestValidatorConfig/SignedInterestTest");
  BOOST_REQUIRE(saveIdentityCertificate(certName, "trust-anchor-9.cert"));

  Name interestName("/TestValidatorConfig/SignedInterestTest");
  Name interestName1 = interestName;
  interestName1.append("1");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  Name interestName2 = interestName;
  interestName2.append("2");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);

  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1, security::signingByCertificate(certName)));
  advanceClocks(time::milliseconds(10));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2, security::signingByCertificate(certName)));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Interest Rule\"\n"
    "  for interest\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/SignedInterestTest\n"
    "    relation is-strict-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type rsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-9.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });
  advanceClocks(time::milliseconds(10));
}

BOOST_AUTO_TEST_CASE(MaxKeyTest)
{
  Name identity("/TestValidatorConfig/MaxKeyTest");

  Name identity1 = identity;
  identity1.append("Key1");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity1), "trust-anchor-10-1.cert"));

  Name identity2 = identity;
  identity2.append("Key2");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity2), "trust-anchor-10-2.cert"));

  Name identity3 = identity;
  identity3.append("Key3");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity3), "trust-anchor-10-3.cert"));


  Name interestName("/TestValidatorConfig/MaxKeyTest");
  Name interestName1 = interestName;
  interestName1.append("1");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  Name interestName2 = interestName;
  interestName2.append("2");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);
  Name interestName3 = interestName;
  interestName3.append("3");
  shared_ptr<Interest> interest3 = make_shared<Interest>(interestName3);

  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1, security::signingByIdentity(identity1)));
  advanceClocks(time::milliseconds(10));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2, security::signingByIdentity(identity2)));
  advanceClocks(time::milliseconds(10));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest3, security::signingByIdentity(identity3)));
  advanceClocks(time::milliseconds(10));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Interest Rule\"\n"
    "  for interest\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/MaxKeyTest\n"
    "    relation is-strict-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type rsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-1.cert\"\n"
    "    }\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-2.cert\"\n"
    "    }\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-3.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  ValidatorConfig validator(face,
                            ValidatorConfig::DEFAULT_CERTIFICATE_CACHE,
                            ValidatorConfig::DEFAULT_GRACE_INTERVAL,
                            10,
                            2,                 // Two keys can be tracked
                            time::seconds(1)); // TTL is set to 1 sec
  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  // Should succeed because identity1's key has been cleaned up due to space limit.
  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));
}

BOOST_AUTO_TEST_CASE(MaxKeyTest2)
{
  Name identity("/TestValidatorConfig/MaxKeyTest");

  Name identity1 = identity;
  identity1.append("Key1");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity1), "trust-anchor-10-1.cert"));

  Name identity2 = identity;
  identity2.append("Key2");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity2), "trust-anchor-10-2.cert"));

  Name identity3 = identity;
  identity3.append("Key3");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity3), "trust-anchor-10-3.cert"));

  Name identity4 = identity;
  identity4.append("Key4");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(identity4), "trust-anchor-10-4.cert"));


  Name interestName("/TestValidatorConfig/MaxKeyTest");
  Name interestName1 = interestName;
  interestName1.append("1");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  Name interestName2 = interestName;
  interestName2.append("2");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);
  Name interestName3 = interestName;
  interestName3.append("3");
  shared_ptr<Interest> interest3 = make_shared<Interest>(interestName3);
  Name interestName4 = interestName;
  interestName4.append("4");
  shared_ptr<Interest> interest4 = make_shared<Interest>(interestName4);

  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1, security::signingByIdentity(identity1)));
  advanceClocks(time::milliseconds(10));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2, security::signingByIdentity(identity2)));
  advanceClocks(time::milliseconds(10));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest3, security::signingByIdentity(identity3)));
  advanceClocks(time::milliseconds(10));
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest4, security::signingByIdentity(identity4)));
  advanceClocks(time::milliseconds(10));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Interest Rule\"\n"
    "  for interest\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/MaxKeyTest\n"
    "    relation is-strict-prefix-of\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type rsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-1.cert\"\n"
    "    }\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-2.cert\"\n"
    "    }\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-3.cert\"\n"
    "    }\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-10-4.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  ValidatorConfig validator(face,
                            ValidatorConfig::DEFAULT_CERTIFICATE_CACHE,
                            ValidatorConfig::DEFAULT_GRACE_INTERVAL,
                            10,
                            3,                 // Three keys can be tracked
                            time::seconds(1)); // TTL is set to 1 sec
  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });

  advanceClocks(time::seconds(2));

  validator.validate(*interest4,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  // Should succeed because identity1 and identity2's key has been cleaned up due to ttl limit.
  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10));
}

BOOST_AUTO_TEST_CASE(FixedSignerChecker2)
{
  Name rsaIdentity("/TestValidatorConfig/FixedSignerChecker2/Rsa");
  addIdentity(rsaIdentity, RsaKeyParams());

  Name ecIdentity("/TestValidatorConfig/FixedSignerChecker2/Ec");
  auto identity = addIdentity(ecIdentity, EcKeyParams());
  BOOST_REQUIRE(saveIdentityCertificate(identity, "trust-anchor-11.cert"));

  Name dataName("/TestValidatorConfig/FixedSignerChecker2");
  shared_ptr<Data> dataRsa = make_shared<Data>(dataName);
  m_keyChain.sign(*dataRsa, security::signingByIdentity(rsaIdentity));
  shared_ptr<Data> dataEc = make_shared<Data>(dataName);
  m_keyChain.sign(*dataEc, security::signingByIdentity(ecIdentity));

  shared_ptr<Interest> interestRsa = make_shared<Interest>(dataName);
  m_keyChain.sign(*interestRsa, security::signingByIdentity(rsaIdentity));
  shared_ptr<Interest> interestEc = make_shared<Interest>(dataName);
  m_keyChain.sign(*interestEc, security::signingByIdentity(ecIdentity));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Data Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/FixedSignerChecker2\n"
    "    relation equal\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type ecdsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-11.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n"
    "rule\n"
    "{\n"
    "  id \"FixedSignerChecker Interest Rule\"\n"
    "  for interest\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/FixedSignerChecker2\n"
    "    relation equal\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type fixed-signer\n"
    "    sig-type ecdsa-sha256\n"
    "    signer\n"
    "    {\n"
    "      type file\n"
    "      file-name \"trust-anchor-11.cert\"\n"
    "    }\n"
    "  }\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  validator.validate(*dataEc,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*dataRsa,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  validator.validate(*interestEc,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });

  validator.validate(*interestRsa,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });
}


struct FacesFixture : public ValidatorConfigFixture
{
  FacesFixture()
    : face1(io, m_v2KeyChain, {true, true})
    , face2(io, m_v2KeyChain, {true, true})
    , readInterestOffset1(0)
    , readDataOffset1(0)
    , readInterestOffset2(0)
    , readDataOffset2(0)
  {
  }

  bool
  passPacket()
  {
    bool hasPassed = false;

    checkFace(face1.sentInterests, readInterestOffset1, face2, hasPassed);
    checkFace(face1.sentData, readDataOffset1, face2, hasPassed);
    checkFace(face2.sentInterests, readInterestOffset2, face1, hasPassed);
    checkFace(face2.sentData, readDataOffset2, face1, hasPassed);

    return hasPassed;
  }

  template<typename Packet>
  void
  checkFace(std::vector<Packet>& receivedPackets,
            size_t& readPacketOffset,
            util::DummyClientFace& receiver,
            bool& hasPassed)
  {
    while (receivedPackets.size() > readPacketOffset) {
      receiver.receive(receivedPackets[readPacketOffset]);
      readPacketOffset++;
      hasPassed = true;
    }
  }

public:
  util::DummyClientFace face1;
  util::DummyClientFace face2;

  size_t readInterestOffset1;
  size_t readDataOffset1;
  size_t readInterestOffset2;
  size_t readDataOffset2;
};

BOOST_FIXTURE_TEST_CASE(HierarchicalChecker, FacesFixture)
{
  std::vector<v1::CertificateSubjectDescription> subjectDescription;

  Name root("/TestValidatorConfig");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(root), "trust-anchor-6.cert"));


  Name sld("/TestValidatorConfig/HierarchicalChecker");
  addIdentity(sld);
  advanceClocks(time::milliseconds(100));
  Name sldKeyName = m_keyChain.generateRsaKeyPairAsDefault(sld, true);
  shared_ptr<v1::IdentityCertificate> sldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(sldKeyName,
                                                  root,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*sldCert, security::signingByIdentity(root));
  m_keyChain.addCertificateAsIdentityDefault(*sldCert);

  Name nld("/TestValidatorConfig/HierarchicalChecker/NextLevel");
  addIdentity(nld);
  advanceClocks(time::milliseconds(100));
  Name nldKeyName = m_keyChain.generateRsaKeyPairAsDefault(nld, true);
  shared_ptr<v1::IdentityCertificate> nldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(nldKeyName,
                                                  sld,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*nldCert, security::signingByIdentity(sld));
  m_keyChain.addCertificateAsIdentityDefault(*nldCert);

  face1.setInterestFilter(sldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1.put(*sldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  face1.setInterestFilter(nldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1.put(*nldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  Name dataName1 = nld;
  dataName1.append("data1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(nld)));

  Name dataName2("/ConfValidatorTest");
  dataName2.append("data1");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(nld)));

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"Simple3 Rule\"\n"
    "  for data\n"
    "  checker\n"
    "  {\n"
    "    type hierarchical\n"
    "    sig-type rsa-sha256\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-6.cert\"\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  auto validator = make_shared<ValidatorConfig>(&face2);
  validator->load(CONFIG, CONFIG_PATH.c_str());

  advanceClocks(time::milliseconds(2), 100);
  validator->validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  validator->validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());
}

BOOST_FIXTURE_TEST_CASE(Nrd, FacesFixture)
{
  advanceClocks(time::nanoseconds(1));

  std::vector<v1::CertificateSubjectDescription> subjectDescription;

  Name root("/TestValidatorConfig");
  BOOST_REQUIRE(saveIdentityCertificate(addIdentity(root), "trust-anchor-8.cert"));

  Name sld("/TestValidatorConfig/Nrd-1");
  addIdentity(sld);
  advanceClocks(time::milliseconds(100));
  Name sldKeyName = m_keyChain.generateRsaKeyPairAsDefault(sld, true);
  shared_ptr<v1::IdentityCertificate> sldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(sldKeyName,
                                                  root,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*sldCert, security::signingByIdentity(root));
  m_keyChain.addCertificateAsIdentityDefault(*sldCert);

  Name nld("/TestValidatorConfig/Nrd-1/Nrd-2");
  addIdentity(nld);
  advanceClocks(time::milliseconds(100));
  Name nldKeyName = m_keyChain.generateRsaKeyPairAsDefault(nld, true);
  shared_ptr<v1::IdentityCertificate> nldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(nldKeyName,
                                                  sld,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*nldCert, security::signingByIdentity(sld));
  m_keyChain.addCertificateAsIdentityDefault(*nldCert);

  face1.setInterestFilter(sldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1.put(*sldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  face1.setInterestFilter(nldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1.put(*nldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  advanceClocks(time::milliseconds(10));
  Name interestName1("/localhost/nrd/register/option");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1, security::signingByIdentity(nld)));

  advanceClocks(time::milliseconds(10));
  Name interestName2("/localhost/nrd/non-register");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2, security::signingByIdentity(nld)));

  advanceClocks(time::milliseconds(10));
  Name interestName3("/localhost/nrd/register/option");
  shared_ptr<Interest> interest3 = make_shared<Interest>(interestName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest3, security::signingByIdentity(root)));

  advanceClocks(time::milliseconds(10));
  Name interestName4("/localhost/nrd/register/option/timestamp/nonce/fakeSigInfo/fakeSigValue");
  shared_ptr<Interest> interest4 = make_shared<Interest>(interestName4);

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"NRD Prefix Registration Command Rule\"\n"
    "  for interest\n"
    "  filter\n"
    "  {\n"
    "    type name\n"
    "    regex ^<localhost><nrd>[<register><unregister><advertise><withdraw>]<>$\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT>$\n"
    "    }\n"
    "  }\n"
    "}\n"
    "rule\n"
    "{\n"
    "  id \"Testbed Hierarchy Rule\"\n"
    "  for data\n"
    "  filter\n"
    "  {\n"
    "    type name\n"
    "    regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT><>$\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type hierarchical\n"
    "    sig-type rsa-sha256\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type file\n"
    "  file-name \"trust-anchor-8.cert\"\n"
    "}\n";
  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  auto validator = make_shared<ValidatorConfig>(&face2);
  validator->load(CONFIG, CONFIG_PATH.c_str());

  advanceClocks(time::milliseconds(2), 100);

  // should succeed
  validator->validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  // should fail
  validator->validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  // should succeed
  validator->validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(false); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  // should fail
  validator->validate(*interest4,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const std::string&) { BOOST_CHECK(true); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());
}

struct DirTestFixture : public ValidatorConfigFixture
{
  DirTestFixture()
    : validator(&face, ValidatorConfig::DEFAULT_CERTIFICATE_CACHE,
                ValidatorConfig::DEFAULT_GRACE_INTERVAL, 0)
  {
    certDirPath = (boost::filesystem::current_path() / std::string("test-cert-dir"));
    boost::filesystem::create_directory(certDirPath);

    firstCertPath = (boost::filesystem::current_path() /
                     std::string("test-cert-dir") /
                     std::string("trust-anchor-1.cert"));

    secondCertPath = (boost::filesystem::current_path() /
                      std::string("test-cert-dir") /
                      std::string("trust-anchor-2.cert"));

    firstIdentity = Name("/TestValidatorConfig/Dir/First");
    addIdentity(firstIdentity);
    Name firstCertName = m_keyChain.getDefaultCertificateNameForIdentity(firstIdentity);
    firstCert = m_keyChain.getCertificate(firstCertName);
    io::save(*firstCert, firstCertPath.string());

    secondIdentity = Name("/TestValidatorConfig/Dir/Second");
    addIdentity(secondIdentity);
    Name secondCertName = m_keyChain.getDefaultCertificateNameForIdentity(secondIdentity);
    secondCert = m_keyChain.getCertificate(secondCertName);
  }

  ~DirTestFixture()
  {
    boost::filesystem::remove_all(certDirPath);
  }

public:
  boost::filesystem::path certDirPath;
  boost::filesystem::path firstCertPath;
  boost::filesystem::path secondCertPath;

  Name firstIdentity;
  Name secondIdentity;

  shared_ptr<v1::IdentityCertificate> firstCert;
  shared_ptr<v1::IdentityCertificate> secondCert;

  ValidatorConfig validator;
};

BOOST_FIXTURE_TEST_CASE(TrustAnchorDir, DirTestFixture)
{
  advanceClocks(time::milliseconds(10));

  Name dataName1("/any/data/1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1, security::signingByIdentity(firstIdentity)));

  Name dataName2("/any/data/2");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2, security::signingByIdentity(secondIdentity)));

  std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"Any Rule\"\n"
    "  for data\n"
    "  filter\n"
    "  {\n"
    "    type name\n"
    "    regex ^<>*$\n"
    "  }\n"
    "  checker\n"
    "  {\n"
    "    type customized\n"
    "    sig-type rsa-sha256\n"
    "    key-locator\n"
    "    {\n"
    "      type name\n"
    "      regex ^<>*$\n"
    "    }\n"
    "  }\n"
    "}\n"
    "trust-anchor\n"
    "{\n"
    "  type dir\n"
    "  dir test-cert-dir\n"
    "  refresh 1s\n"
    "}\n";

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));

  validator.load(CONFIG, CONFIG_PATH.c_str());

  advanceClocks(time::milliseconds(10), 20);
  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10), 20);

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(true); });
  advanceClocks(time::milliseconds(10), 20);

  io::save(*secondCert, secondCertPath.string());
  advanceClocks(time::milliseconds(10), 200);

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10), 20);

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const std::string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10), 20);
}

class DirectCertFetchFixture : public ValidatorConfigFixture
{
public:
  DirectCertFetchFixture()
    : clientFace(io, m_v2KeyChain, {true, true})
    , validationResult(boost::logic::indeterminate)
  {
    auto certName = addIdentity(ca);
    saveIdentityCertificate(certName, "trust-anchor-1.cert");
    addSubCertificate(user, ca);

    userCertName = m_keyChain.getDefaultCertificateNameForIdentity(user);
    userCert = m_keyChain.getCertificate(userCertName);
  }

protected:
  void
  runTest(const std::function<void(const Interest&, const Interest&)> respond)
  {
    optional<Interest> directInterest;
    optional<Interest> infrastructureInterest;
    clientFace.onSendInterest.connect([&] (const Interest& interest) {
      const Name& interestName = interest.getName();
      if (interestName == userCert->getName().getPrefix(-1)) {
        auto nextHopFaceIdTag = interest.getTag<lp::NextHopFaceIdTag>();
        if (nextHopFaceIdTag != nullptr) {
          BOOST_CHECK(!directInterest);
          directInterest = interest;
        }
        else {
          BOOST_CHECK(!infrastructureInterest);
          infrastructureInterest = interest;
        }
        if (static_cast<bool>(directInterest) && static_cast<bool>(infrastructureInterest)) {
          io.post([directInterest, infrastructureInterest, respond] {
              respond(directInterest.value(), infrastructureInterest.value());
            });
          directInterest = nullopt;
          infrastructureInterest = nullopt;
        }
      }
    });

    const boost::filesystem::path CONFIG_PATH =
      (boost::filesystem::current_path() / std::string("unit-test-direct.conf"));
    ValidatorConfig validator(&clientFace);
    validator.load(CONFIG, CONFIG_PATH.c_str());
    validator.setDirectCertFetchEnabled(true);

    shared_ptr<Interest> interest = make_shared<Interest>(interestName);
    interest->setTag(make_shared<lp::IncomingFaceIdTag>(123));
    BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest, security::signingByIdentity(user)));

    validator.validate(*interest,
                       [&] (const shared_ptr<const Interest>&) {
                         BOOST_CHECK(boost::logic::indeterminate(validationResult));
                         validationResult = true;
                       },
                       [&] (const shared_ptr<const Interest>&, const std::string& s) {
                         BOOST_CHECK(boost::logic::indeterminate(validationResult));
                         validationResult = false;
                       });

    advanceClocks(time::milliseconds(200), 60);

    BOOST_CHECK(!boost::logic::indeterminate(validationResult));
  }

public:
  util::DummyClientFace clientFace;
  const Name ca = Name("/DirectCertFetch");
  const Name user = Name("/DirectCertFetch/user");
  const Name interestName = Name("/DirectCertFetch/user/tag-interest");
  const std::string CONFIG = R"_TEXT_(
      rule
      {
        id "RuleForInterest"
        for interest
        checker
        {
          type hierarchical
          sig-type rsa-sha256
        }
      }
      rule
      {
        id "RuleForData"
        for data
        filter
        {
          type name
          regex ^<>*$
        }
        checker
        {
          type customized
          sig-type rsa-sha256
          key-locator
        {
          type name
           regex ^<>*$
          }
        }
      }
      trust-anchor
      {
        type file
        file-name "trust-anchor-1.cert"
      }
      )_TEXT_";
  boost::logic::tribool validationResult;
  Name userCertName;
  shared_ptr<v1::IdentityCertificate> userCert;
};

BOOST_FIXTURE_TEST_SUITE(DirectCertFetch, DirectCertFetchFixture)

BOOST_AUTO_TEST_CASE(CertFetchSuccess)
{
  runTest([this] (const Interest& directInterest, const Interest& infrastructureInterest) {
      this->clientFace.receive(*userCert);
    });

  BOOST_CHECK_EQUAL(validationResult, true);
}

BOOST_AUTO_TEST_CASE(CertFetchTimeout)
{
  // In this test case, certificate request would time out
  runTest([this] (const Interest& directInterest, const Interest& infrastructureInterest) { });

  BOOST_CHECK_EQUAL(validationResult, false);
}

BOOST_AUTO_TEST_CASE(CertFetchNack)
{
  runTest([this] (const Interest& directInterest, const Interest& infrastructureInterest) {
      lp::Nack nackInfrastructureInterest(infrastructureInterest);
      nackInfrastructureInterest.setHeader(lp::NackHeader().setReason(lp::NackReason::NO_ROUTE));
      this->clientFace.receive(nackInfrastructureInterest);
    });

  BOOST_CHECK_EQUAL(validationResult, false);
}

BOOST_AUTO_TEST_SUITE_END() // DirectCertFetch
BOOST_AUTO_TEST_SUITE_END() // TestValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
