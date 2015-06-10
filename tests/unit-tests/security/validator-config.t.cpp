/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "security/key-chain.hpp"
#include "util/io.hpp"
#include "util/scheduler.hpp"
#include "util/dummy-client-face.hpp"

#include <boost/asio.hpp>

#include "identity-management-fixture.hpp"
#include "../identity-management-time-fixture.hpp"
#include "boost-test.hpp"

using namespace std;

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityValidatorConfig)

BOOST_FIXTURE_TEST_CASE(NameFilter, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/NameFilter");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = m_keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-1.cert");

  Name dataName1("/simple/equal");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName2("/simple/different");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-1.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(NameFilter2, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/NameFilter2");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = m_keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-2.cert");

  Name dataName1("/simple/isPrefixOf");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName2("/simple/notPrefixOf");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName3("/simple/isPrefixOf/anotherLevel");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-2.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(NameFilter3, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/NameFilter3");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = m_keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-3.cert");

  Name dataName1("/simple/isStrictPrefixOf");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName2("/simple");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName3("/simple/isStrictPrefixOf/anotherLevel");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-3.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(NameFilter4, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/NameFilter4");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = m_keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-4.cert");

  Name dataName1("/simple/regex");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName2("/simple/regex-wrong");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName3("/simple/regex/correct");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-4.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(KeyLocatorNameChecker1, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/KeyLocatorNameChecker1");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = m_keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-5.cert");

  Name dataName1 = identity;
  dataName1.append("1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName2 = identity;
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  Name dataName3("/TestValidatorConfig/KeyLocatorNameChecker1");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*data3,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-5.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(FixedSignerChecker, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/FixedSignerChecker");

  Name identity1 = identity;
  identity1.append("1").appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity1));
  Name certName1 = m_keyChain.getDefaultCertificateNameForIdentity(identity1);
  shared_ptr<IdentityCertificate> idCert1 = m_keyChain.getCertificate(certName1);
  io::save(*idCert1, "trust-anchor-7.cert");

  Name identity2 = identity;
  identity2.append("2").appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity2));

  Name dataName1 = identity;
  dataName1.append("data").appendVersion();
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity1)));

  Name dataName2 = identity;
  dataName2.append("data").appendVersion();
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity2)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*interest,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-7.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(Reset, security::IdentityManagementFixture)
{
  Name root("/TestValidatorConfig/Reload");
  BOOST_REQUIRE_NO_THROW(addIdentity(root));
  Name rootCertName = m_keyChain.getDefaultCertificateNameForIdentity(root);
  shared_ptr<IdentityCertificate> rootCert = m_keyChain.getCertificate(rootCertName);
  io::save(*rootCert, "trust-anchor-8.cert");

  Face face;

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


  shared_ptr<ValidatorConfig> validator = shared_ptr<ValidatorConfig>(new ValidatorConfig(face));

  validator->load(CONFIG, CONFIG_PATH.native());
  BOOST_CHECK_EQUAL(validator->isEmpty(), false);

  validator->reset();
  BOOST_CHECK(validator->isEmpty());

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-8.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(TrustAnchorWildcard, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/Wildcard");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity));

  Name dataName1("/any/data");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity)));

  std::string CONFIG =
    "trust-anchor\n"
    "{\n"
    "  type any\n"
    "}\n";

  const boost::filesystem::path CONFIG_PATH =
    (boost::filesystem::current_path() / std::string("unit-test-nfd.conf"));


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });
}

BOOST_FIXTURE_TEST_CASE(SignedInterestTest, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/SignedInterestTest");

  Name identity1 = identity;
  identity1.appendVersion();
  BOOST_REQUIRE_NO_THROW(addIdentity(identity1));
  Name certName1 = m_keyChain.getDefaultCertificateNameForIdentity(identity1);
  shared_ptr<IdentityCertificate> idCert1 = m_keyChain.getCertificate(certName1);
  io::save(*idCert1, "trust-anchor-9.cert");

  Name interestName("/TestValidatorConfig/SignedInterestTest");
  Name interestName1 = interestName;
  interestName1.append("1");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  Name interestName2 = interestName;
  interestName2.append("2");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);

  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity1)));
  usleep(10000);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity1)));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-9.cert"));
  boost::filesystem::remove(CERT_PATH);
}


BOOST_FIXTURE_TEST_CASE(MaxKeyTest, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/MaxKeyTest");

  Name identity1 = identity;
  identity1.append("Key1");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity1));
  Name certName1 = m_keyChain.getDefaultCertificateNameForIdentity(identity1);
  shared_ptr<IdentityCertificate> idCert1 = m_keyChain.getCertificate(certName1);
  io::save(*idCert1, "trust-anchor-10-1.cert");

  Name identity2 = identity;
  identity2.append("Key2");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity2));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);
  shared_ptr<IdentityCertificate> idCert2 = m_keyChain.getCertificate(certName2);
  io::save(*idCert2, "trust-anchor-10-2.cert");

  Name identity3 = identity;
  identity3.append("Key3");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity3));
  Name certName3 = m_keyChain.getDefaultCertificateNameForIdentity(identity3);
  shared_ptr<IdentityCertificate> idCert3 = m_keyChain.getCertificate(certName3);
  io::save(*idCert3, "trust-anchor-10-3.cert");


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

  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity1)));
  usleep(10000);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity2)));
  usleep(10000);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity3)));

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


  Face face;
  ValidatorConfig validator(face,
                            ValidatorConfig::DEFAULT_CERTIFICATE_CACHE,
                            ValidatorConfig::DEFAULT_GRACE_INTERVAL,
                            10,
                            2,                 // Two keys can be tracked
                            time::seconds(1)); // TTL is set to 1 sec
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  // Should succeed because identity1's key has been cleaned up due to space limit.
  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  const boost::filesystem::path CERT_PATH1 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-1.cert"));
  boost::filesystem::remove(CERT_PATH1);

  const boost::filesystem::path CERT_PATH2 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-2.cert"));
  boost::filesystem::remove(CERT_PATH2);

  const boost::filesystem::path CERT_PATH3 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-3.cert"));
  boost::filesystem::remove(CERT_PATH3);
}

BOOST_FIXTURE_TEST_CASE(MaxKeyTest2, security::IdentityManagementFixture)
{
  Name identity("/TestValidatorConfig/MaxKeyTest");

  Name identity1 = identity;
  identity1.append("Key1");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity1));
  Name certName1 = m_keyChain.getDefaultCertificateNameForIdentity(identity1);
  shared_ptr<IdentityCertificate> idCert1 = m_keyChain.getCertificate(certName1);
  io::save(*idCert1, "trust-anchor-10-1.cert");

  Name identity2 = identity;
  identity2.append("Key2");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity2));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);
  shared_ptr<IdentityCertificate> idCert2 = m_keyChain.getCertificate(certName2);
  io::save(*idCert2, "trust-anchor-10-2.cert");

  Name identity3 = identity;
  identity3.append("Key3");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity3));
  Name certName3 = m_keyChain.getDefaultCertificateNameForIdentity(identity3);
  shared_ptr<IdentityCertificate> idCert3 = m_keyChain.getCertificate(certName3);
  io::save(*idCert3, "trust-anchor-10-3.cert");

  Name identity4 = identity;
  identity4.append("Key4");
  BOOST_REQUIRE_NO_THROW(addIdentity(identity4));
  Name certName4 = m_keyChain.getDefaultCertificateNameForIdentity(identity4);
  shared_ptr<IdentityCertificate> idCert4 = m_keyChain.getCertificate(certName4);
  io::save(*idCert4, "trust-anchor-10-4.cert");


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


  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity1)));
  usleep(10000);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity2)));
  usleep(10000);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity3)));
  usleep(10000);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest4,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             identity4)));

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


  Face face;
  ValidatorConfig validator(face,
                            ValidatorConfig::DEFAULT_CERTIFICATE_CACHE,
                            ValidatorConfig::DEFAULT_GRACE_INTERVAL,
                            10,
                            3,                 // Two keys can be tracked
                            time::seconds(1)); // TTL is set to 1 sec
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  sleep(2);

  validator.validate(*interest4,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  // Should succeed because identity1 and identity2's key has been cleaned up due to ttl limit.
  validator.validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });


  const boost::filesystem::path CERT_PATH1 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-1.cert"));
  boost::filesystem::remove(CERT_PATH1);

  const boost::filesystem::path CERT_PATH2 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-2.cert"));
  boost::filesystem::remove(CERT_PATH2);

  const boost::filesystem::path CERT_PATH3 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-3.cert"));
  boost::filesystem::remove(CERT_PATH3);

  const boost::filesystem::path CERT_PATH4 =
    (boost::filesystem::current_path() / std::string("trust-anchor-10-4.cert"));
  boost::filesystem::remove(CERT_PATH4);
}

BOOST_FIXTURE_TEST_CASE(FixedSignerChecker2, security::IdentityManagementFixture)
{
  Name rsaIdentity("/TestValidatorConfig/FixedSignerChecker2/Rsa");
  BOOST_REQUIRE_NO_THROW(addIdentity(rsaIdentity));
  Name rsaCertName = m_keyChain.getDefaultCertificateNameForIdentity(rsaIdentity);

  Name ecdsaIdentity("/TestValidatorConfig/FixedSignerChecker2/Ecdsa");
  BOOST_REQUIRE_NO_THROW(addIdentity(ecdsaIdentity, EcdsaKeyParams()));
  Name ecdsaCertName = m_keyChain.getDefaultCertificateNameForIdentity(ecdsaIdentity);
  shared_ptr<IdentityCertificate> ecdsaCert = m_keyChain.getCertificate(ecdsaCertName);
  io::save(*ecdsaCert, "trust-anchor-11.cert");


  Name dataName("/TestValidatorConfig/FixedSignerChecker2");
  shared_ptr<Data> dataRsa = make_shared<Data>(dataName);
  m_keyChain.sign(*dataRsa,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        rsaIdentity));
  shared_ptr<Data> dataEcdsa = make_shared<Data>(dataName);
  m_keyChain.sign(*dataEcdsa,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        ecdsaIdentity));

  shared_ptr<Interest> interestRsa = make_shared<Interest>(dataName);
  m_keyChain.sign(*interestRsa,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        rsaIdentity));
  shared_ptr<Interest> interestEcdsa = make_shared<Interest>(dataName);
  m_keyChain.sign(*interestEcdsa,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        ecdsaIdentity));

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


  Face face;
  ValidatorConfig validator(face);
  validator.load(CONFIG, CONFIG_PATH.native());

  validator.validate(*dataEcdsa,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*dataRsa,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  validator.validate(*interestEcdsa,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  validator.validate(*interestRsa,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-11.cert"));
  boost::filesystem::remove(CERT_PATH);
}


struct FacesFixture : public security::IdentityManagementTimeFixture
{
  FacesFixture()
    : face1(util::makeDummyClientFace(io, {true, true}))
    , face2(util::makeDummyClientFace(io, {true, true}))
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

    checkFace(face1->sentInterests, readInterestOffset1, *face2, hasPassed);
    checkFace(face1->sentDatas, readDataOffset1, *face2, hasPassed);
    checkFace(face2->sentInterests, readInterestOffset2, *face1, hasPassed);
    checkFace(face2->sentInterests, readDataOffset2, *face1, hasPassed);

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

  ~FacesFixture()
  {
  }

public:
  shared_ptr<util::DummyClientFace> face1;
  shared_ptr<util::DummyClientFace> face2;

  size_t readInterestOffset1;
  size_t readDataOffset1;
  size_t readInterestOffset2;
  size_t readDataOffset2;
};

BOOST_FIXTURE_TEST_CASE(HierarchicalChecker, FacesFixture)
{
  std::vector<CertificateSubjectDescription> subjectDescription;

  Name root("/TestValidatorConfig");
  BOOST_REQUIRE_NO_THROW(addIdentity(root));
  Name rootCertName = m_keyChain.getDefaultCertificateNameForIdentity(root);
  shared_ptr<IdentityCertificate> rootCert = m_keyChain.getCertificate(rootCertName);
  io::save(*rootCert, "trust-anchor-6.cert");


  Name sld("/TestValidatorConfig/HierarchicalChecker");
  BOOST_REQUIRE_NO_THROW(addIdentity(sld));
  advanceClocks(time::milliseconds(100));
  Name sldKeyName = m_keyChain.generateRsaKeyPairAsDefault(sld, true);
  shared_ptr<IdentityCertificate> sldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(sldKeyName,
                                                  root,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*sldCert,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        root));
  m_keyChain.addCertificateAsIdentityDefault(*sldCert);

  Name nld("/TestValidatorConfig/HierarchicalChecker/NextLevel");
  BOOST_REQUIRE_NO_THROW(addIdentity(nld));
  advanceClocks(time::milliseconds(100));
  Name nldKeyName = m_keyChain.generateRsaKeyPairAsDefault(nld, true);
  shared_ptr<IdentityCertificate> nldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(nldKeyName,
                                                  sld,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*nldCert,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        sld));
  m_keyChain.addCertificateAsIdentityDefault(*nldCert);

  face1->setInterestFilter(sldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1->put(*sldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  face1->setInterestFilter(nldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1->put(*nldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  Name dataName1 = nld;
  dataName1.append("data1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             nld)));

  Name dataName2("/ConfValidatorTest");
  dataName2.append("data1");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             nld)));


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


  auto validator = make_shared<ValidatorConfig>(face2.get());
  validator->load(CONFIG, CONFIG_PATH.native());

  advanceClocks(time::milliseconds(2), 100);
  validator->validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  validator->validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-6.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_FIXTURE_TEST_CASE(Nrd, FacesFixture)
{
  advanceClocks(time::milliseconds(0));

  std::vector<CertificateSubjectDescription> subjectDescription;

  Name root("/TestValidatorConfig");
  BOOST_REQUIRE_NO_THROW(addIdentity(root));
  Name rootCertName = m_keyChain.getDefaultCertificateNameForIdentity(root);
  shared_ptr<IdentityCertificate> rootCert = m_keyChain.getCertificate(rootCertName);
  io::save(*rootCert, "trust-anchor-8.cert");


  Name sld("/TestValidatorConfig/Nrd-1");
  BOOST_REQUIRE_NO_THROW(addIdentity(sld));
  advanceClocks(time::milliseconds(100));
  Name sldKeyName = m_keyChain.generateRsaKeyPairAsDefault(sld, true);
  shared_ptr<IdentityCertificate> sldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(sldKeyName,
                                                  root,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*sldCert,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        root));
  m_keyChain.addCertificateAsIdentityDefault(*sldCert);

  Name nld("/TestValidatorConfig/Nrd-1/Nrd-2");
  BOOST_REQUIRE_NO_THROW(addIdentity(nld));
  advanceClocks(time::milliseconds(100));
  Name nldKeyName = m_keyChain.generateRsaKeyPairAsDefault(nld, true);
  shared_ptr<IdentityCertificate> nldCert =
    m_keyChain.prepareUnsignedIdentityCertificate(nldKeyName,
                                                  sld,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*nldCert,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        sld));
  m_keyChain.addCertificateAsIdentityDefault(*nldCert);

  face1->setInterestFilter(sldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1->put(*sldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  face1->setInterestFilter(nldCert->getName().getPrefix(-1),
    [&] (const InterestFilter&, const Interest&) { face1->put(*nldCert); },
    RegisterPrefixSuccessCallback(),
    [] (const Name&, const std::string&) {});

  advanceClocks(time::milliseconds(10));
  Name interestName1("/localhost/nrd/register/option");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             nld)));

  advanceClocks(time::milliseconds(10));
  Name interestName2("/localhost/nrd/non-register");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             nld)));

  advanceClocks(time::milliseconds(10));
  Name interestName3("/localhost/nrd/register/option");
  shared_ptr<Interest> interest3 = make_shared<Interest>(interestName3);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*interest3,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             root)));

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


  auto validator = make_shared<ValidatorConfig>(face2.get());
  validator->load(CONFIG, CONFIG_PATH.native());

  advanceClocks(time::milliseconds(2), 100);

  // should succeed
  validator->validate(*interest1,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  // should fail
  validator->validate(*interest2,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  // should succeed
  validator->validate(*interest3,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(false); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  // should fail
  validator->validate(*interest4,
    [] (const shared_ptr<const Interest>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Interest>&, const string&) { BOOST_CHECK(true); });

  do {
    advanceClocks(time::milliseconds(2), 10);
  } while (passPacket());

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-8.cert"));
  boost::filesystem::remove(CERT_PATH);
}

struct DirTestFixture : public security::IdentityManagementTimeFixture
{
  DirTestFixture()
    : face(util::makeDummyClientFace(io, {true, true}))
    , validator(face.get(), ValidatorConfig::DEFAULT_CERTIFICATE_CACHE,
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
    BOOST_REQUIRE_NO_THROW(addIdentity(firstIdentity));
    Name firstCertName = m_keyChain.getDefaultCertificateNameForIdentity(firstIdentity);
    firstCert = m_keyChain.getCertificate(firstCertName);
    io::save(*firstCert, firstCertPath.string());

    secondIdentity = Name("/TestValidatorConfig/Dir/Second");
    BOOST_REQUIRE_NO_THROW(addIdentity(secondIdentity));
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

  shared_ptr<IdentityCertificate> firstCert;
  shared_ptr<IdentityCertificate> secondCert;

  shared_ptr<util::DummyClientFace> face;
  ValidatorConfig validator;
};

BOOST_FIXTURE_TEST_CASE(TrustAnchorDir, DirTestFixture)
{
  advanceClocks(time::milliseconds(10));

  Name dataName1("/any/data/1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data1,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             firstIdentity)));

  Name dataName2("/any/data/2");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(m_keyChain.sign(*data2,
                                       security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                                             secondIdentity)));

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

  validator.load(CONFIG, CONFIG_PATH.native());

  advanceClocks(time::milliseconds(10), 20);
  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10), 20);

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(false); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(true); });
  advanceClocks(time::milliseconds(10), 20);

  io::save(*secondCert, secondCertPath.string());
  advanceClocks(time::milliseconds(10), 200);

  validator.validate(*data1,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10), 20);

  validator.validate(*data2,
    [] (const shared_ptr<const Data>&) { BOOST_CHECK(true); },
    [] (const shared_ptr<const Data>&, const string&) { BOOST_CHECK(false); });
  advanceClocks(time::milliseconds(10), 20);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
