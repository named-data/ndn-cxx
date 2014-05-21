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
 *
 * @author Yingdi Yu <yingdi0@cs.ucla.edu>
 */

#include "security/validator-config.hpp"

#include "security/key-chain.hpp"
#include "util/io.hpp"
#include "util/scheduler.hpp"

#include <boost/asio.hpp>

#include "boost-test.hpp"

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestValidatorConfig)

void
onValidated(const shared_ptr<const Data>& data)
{
  BOOST_CHECK(true);
}

void
onValidationFailed(const shared_ptr<const Data>& data, const string& failureInfo)
{
  std::cerr << "Failure Info: " << failureInfo << std::endl;
  BOOST_CHECK(false);
}

void
onIntentionalFailureValidated(const shared_ptr<const Data>& data)
{
  BOOST_CHECK(false);
}

void
onIntentionalFailureInvalidated(const shared_ptr<const Data>& data, const string& failureInfo)
{
  BOOST_CHECK(true);
}

void
onValidated2(const shared_ptr<const Interest>& interest)
{
  BOOST_CHECK(true);
}

void
onValidationFailed2(const shared_ptr<const Interest>& interest, const string& failureInfo)
{
  std::cerr << "Interest Name: " << interest->getName() << std::endl;
  std::cerr << "Failure Info: " << failureInfo << std::endl;
  BOOST_CHECK(false);
}

void
onIntentionalFailureValidated2(const shared_ptr<const Interest>& interest)
{
  BOOST_CHECK(false);
}

void
onIntentionalFailureInvalidated2(const shared_ptr<const Interest>& interest,
                                 const string& failureInfo)
{
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(NameFilter)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/NameFilter");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));
  Name certName = keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-1.cert");

  Name dataName1("/simple/equal");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity));

  Name dataName2("/simple/different");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, identity));

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
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  validator.validate(*data2,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  keyChain.deleteIdentity(identity);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-1.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(NameFilter2)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/NameFilter2");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));
  Name certName = keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-2.cert");

  Name dataName1("/simple/isPrefixOf");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity));

  Name dataName2("/simple/notPrefixOf");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, identity));

  Name dataName3("/simple/isPrefixOf/anotherLevel");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data3, identity));

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
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  validator.validate(*data2,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  validator.validate(*data3,
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  keyChain.deleteIdentity(identity);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-2.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(NameFilter3)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/NameFilter3");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));
  Name certName = keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-3.cert");

  Name dataName1("/simple/isStrictPrefixOf");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity));

  Name dataName2("/simple");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, identity));

  Name dataName3("/simple/isStrictPrefixOf/anotherLevel");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data3, identity));

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
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  validator.validate(*data2,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  validator.validate(*data3,
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  keyChain.deleteIdentity(identity);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-3.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(NameFilter4)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/NameFilter4");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));
  Name certName = keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-4.cert");

  Name dataName1("/simple/regex");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity));

  Name dataName2("/simple/regex-wrong");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, identity));

  Name dataName3("/simple/regex/correct");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data3, identity));

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
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  validator.validate(*data2,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  validator.validate(*data3,
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  keyChain.deleteIdentity(identity);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-4.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(KeyLocatorNameChecker1)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/KeyLocatorNameChecker1");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));
  Name certName = keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<IdentityCertificate> idCert = keyChain.getCertificate(certName);
  io::save(*idCert, "trust-anchor-5.cert");

  Name dataName1 = identity;
  dataName1.append("1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity));

  Name dataName2 = identity;
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, identity));

  Name dataName3("/TestValidatorConfig/KeyLocatorNameChecker1");
  shared_ptr<Data> data3 = make_shared<Data>(dataName3);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data3, identity));

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
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  validator.validate(*data2,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  validator.validate(*data3,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  keyChain.deleteIdentity(identity);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-5.cert"));
  boost::filesystem::remove(CERT_PATH);
}

struct FacesFixture
{
  FacesFixture()
    : regPrefixId(0)
    , regPrefixId2(0)
  {}

  void
  onInterest(shared_ptr<Face> face, shared_ptr<Data> data)
  {
    face->put(*data);
    face->unsetInterestFilter(regPrefixId);
  }

  void
  onInterest2(shared_ptr<Face> face, shared_ptr<Data> data)
  {
    face->put(*data);
    face->unsetInterestFilter(regPrefixId2);
  }

  void
  onRegFailed()
  {}

  void
  validate1(shared_ptr<ValidatorConfig> validator, shared_ptr<Data> data)
  {
    validator->validate(*data,
                        bind(&onValidated, _1),
                        bind(&onValidationFailed, _1, _2));
  }

  void
  validate2(shared_ptr<ValidatorConfig> validator, shared_ptr<Data> data)
  {
    validator->validate(*data,
                        bind(&onIntentionalFailureValidated, _1),
                        bind(&onIntentionalFailureInvalidated, _1, _2));
  }

  void
  validate3(shared_ptr<ValidatorConfig> validator, shared_ptr<Interest> interest)
  {
    validator->validate(*interest,
                        bind(&onValidated2, _1),
                        bind(&onValidationFailed2, _1, _2));
  }

  void
  validate4(shared_ptr<ValidatorConfig> validator, shared_ptr<Interest> interest)
  {
    validator->validate(*interest,
                        bind(&onIntentionalFailureValidated2, _1),
                        bind(&onIntentionalFailureInvalidated2, _1, _2));
  }

  void
  terminate(shared_ptr<Face> face)
  {
    face->getIoService().stop();
  }

  const RegisteredPrefixId* regPrefixId;
  const RegisteredPrefixId* regPrefixId2;
};

BOOST_FIXTURE_TEST_CASE(HierarchicalChecker, FacesFixture)
{
  KeyChain keyChain;
  std::vector<CertificateSubjectDescription> subjectDescription;

  Name root("/TestValidatorConfig");
  Name rootCertName = keyChain.createIdentity(root);
  shared_ptr<IdentityCertificate> rootCert =
    keyChain.getCertificate(rootCertName);
  io::save(*rootCert, "trust-anchor-6.cert");


  Name sld("/TestValidatorConfig/HierarchicalChecker");
  Name sldKeyName = keyChain.generateRsaKeyPairAsDefault(sld, true);
  shared_ptr<IdentityCertificate> sldCert =
    keyChain.prepareUnsignedIdentityCertificate(sldKeyName,
                                                root,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(7300),
                                                subjectDescription);
  keyChain.signByIdentity(*sldCert, root);
  keyChain.addCertificateAsIdentityDefault(*sldCert);

  Name nld("/TestValidatorConfig/HierarchicalChecker/NextLevel");
  Name nldKeyName = keyChain.generateRsaKeyPairAsDefault(nld, true);
  shared_ptr<IdentityCertificate> nldCert =
    keyChain.prepareUnsignedIdentityCertificate(nldKeyName,
                                                sld,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(7300),
                                                subjectDescription);
  keyChain.signByIdentity(*nldCert, sld);
  keyChain.addCertificateAsIdentityDefault(*nldCert);

  shared_ptr<Face> face = make_shared<Face>();
  Face face2(face->getIoService());
  Scheduler scheduler(face->getIoService());

  scheduler.scheduleEvent(time::seconds(1),
                          bind(&FacesFixture::terminate, this, face));

  regPrefixId = face->setInterestFilter(sldCert->getName().getPrefix(-1),
                                        bind(&FacesFixture::onInterest, this, face, sldCert),
                                        RegisterPrefixSuccessCallback(),
                                        bind(&FacesFixture::onRegFailed, this));

  regPrefixId2 = face->setInterestFilter(nldCert->getName().getPrefix(-1),
                                         bind(&FacesFixture::onInterest2, this, face, nldCert),
                                         RegisterPrefixSuccessCallback(),
                                         bind(&FacesFixture::onRegFailed, this));

  Name dataName1 = nld;
  dataName1.append("data1");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, nld));

  Name dataName2("/ConfValidatorTest");
  dataName2.append("data1");
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, nld));


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


  shared_ptr<ValidatorConfig> validator = shared_ptr<ValidatorConfig>(new ValidatorConfig(face2));
  validator->load(CONFIG, CONFIG_PATH.native());

  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::validate1, this,
                               validator, data1));

  scheduler.scheduleEvent(time::milliseconds(400),
                          bind(&FacesFixture::validate2, this,
                               validator, data2));

  BOOST_REQUIRE_NO_THROW(face->processEvents());

  keyChain.deleteIdentity(root);
  keyChain.deleteIdentity(sld);
  keyChain.deleteIdentity(nld);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-6.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(FixedSingerChecker)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/FixedSingerChecker");

  Name identity1 = identity;
  identity1.append("1").appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity1));
  Name certName1 = keyChain.getDefaultCertificateNameForIdentity(identity1);
  shared_ptr<IdentityCertificate> idCert1 = keyChain.getCertificate(certName1);
  io::save(*idCert1, "trust-anchor-7.cert");

  Name identity2 = identity;
  identity2.append("2").appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity2));

  Name dataName1 = identity;
  dataName1.append("data").appendVersion();
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity1));

  Name dataName2 = identity;
  dataName2.append("data").appendVersion();
  shared_ptr<Data> data2 = make_shared<Data>(dataName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data2, identity2));

  Name interestName("/TestValidatorConfig/FixedSingerChecker/fakeSigInfo/fakeSigValue");
  shared_ptr<Interest> interest = make_shared<Interest>(interestName);

  const std::string CONFIG =
    "rule\n"
    "{\n"
    "  id \"FixedSingerChecker Data Rule\"\n"
    "  for data\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/FixedSingerChecker\n"
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
    "  id \"FixedSingerChecker Interest Rule\"\n"
    "  for interest\n"
    "  filter"
    "  {\n"
    "    type name\n"
    "    name /TestValidatorConfig/FixedSingerChecker\n"
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
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  validator.validate(*data2,
                     bind(&onIntentionalFailureValidated, _1),
                     bind(&onIntentionalFailureInvalidated, _1, _2));

  validator.validate(*interest,
                     bind(&onIntentionalFailureValidated2, _1),
                     bind(&onIntentionalFailureInvalidated2, _1, _2));


  keyChain.deleteIdentity(identity1);
  keyChain.deleteIdentity(identity2);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-7.cert"));
  boost::filesystem::remove(CERT_PATH);
}


BOOST_FIXTURE_TEST_CASE(Nrd, FacesFixture)
{
  KeyChain keyChain;
  std::vector<CertificateSubjectDescription> subjectDescription;

  Name root("/TestValidatorConfig");
  Name rootCertName = keyChain.createIdentity(root);
  shared_ptr<IdentityCertificate> rootCert =
    keyChain.getCertificate(rootCertName);
  io::save(*rootCert, "trust-anchor-8.cert");


  Name sld("/TestValidatorConfig/Nrd-1");
  Name sldKeyName = keyChain.generateRsaKeyPairAsDefault(sld, true);
  shared_ptr<IdentityCertificate> sldCert =
    keyChain.prepareUnsignedIdentityCertificate(sldKeyName,
                                                root,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(7300),
                                                subjectDescription);
  keyChain.signByIdentity(*sldCert, root);
  keyChain.addCertificateAsIdentityDefault(*sldCert);

  Name nld("/TestValidatorConfig/Nrd-1/Nrd-2");
  Name nldKeyName = keyChain.generateRsaKeyPairAsDefault(nld, true);
  shared_ptr<IdentityCertificate> nldCert =
    keyChain.prepareUnsignedIdentityCertificate(nldKeyName,
                                                sld,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(7300),
                                                subjectDescription);
  keyChain.signByIdentity(*nldCert, sld);
  keyChain.addCertificateAsIdentityDefault(*nldCert);

  shared_ptr<Face> face = make_shared<Face>();
  Face face2(face->getIoService());
  Scheduler scheduler(face->getIoService());

  scheduler.scheduleEvent(time::seconds(1),
                          bind(&FacesFixture::terminate, this, face));

  regPrefixId = face->setInterestFilter(sldCert->getName().getPrefix(-1),
                                        bind(&FacesFixture::onInterest, this, face, sldCert),
                                        RegisterPrefixSuccessCallback(),
                                        bind(&FacesFixture::onRegFailed, this));

  regPrefixId2 = face->setInterestFilter(nldCert->getName().getPrefix(-1),
                                         bind(&FacesFixture::onInterest2, this, face, nldCert),
                                         RegisterPrefixSuccessCallback(),
                                         bind(&FacesFixture::onRegFailed, this));

  Name interestName1("/localhost/nrd/register/option/timestamp/nonce");
  shared_ptr<Interest> interest1 = make_shared<Interest>(interestName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*interest1, nld));

  Name interestName2("/localhost/nrd/non-register");
  shared_ptr<Interest> interest2 = make_shared<Interest>(interestName2);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*interest2, nld));

  Name interestName3("/localhost/nrd/register/option/timestamp/nonce");
  shared_ptr<Interest> interest3 = make_shared<Interest>(interestName3);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*interest3, root));

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
    "    regex ^<localhost><nrd>[<register><unregister><advertise><withdraw>]<>{3}$\n"
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


  shared_ptr<ValidatorConfig> validator = shared_ptr<ValidatorConfig>(new ValidatorConfig(face2));
  validator->load(CONFIG, CONFIG_PATH.native());

  // should succeed
  scheduler.scheduleEvent(time::milliseconds(200),
                          bind(&FacesFixture::validate3, this,
                               validator, interest1));
  // should fail
  scheduler.scheduleEvent(time::milliseconds(400),
                          bind(&FacesFixture::validate4, this,
                               validator, interest2));
  // should succeed
  scheduler.scheduleEvent(time::milliseconds(600),
                          bind(&FacesFixture::validate3, this,
                               validator, interest3));
  // should fail
  scheduler.scheduleEvent(time::milliseconds(600),
                          bind(&FacesFixture::validate4, this,
                               validator, interest4));

  BOOST_REQUIRE_NO_THROW(face->processEvents());

  keyChain.deleteIdentity(root);
  keyChain.deleteIdentity(sld);
  keyChain.deleteIdentity(nld);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-8.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(Reset)
{
  KeyChain keyChain;

  Name root("/TestValidatorConfig/Reload");
  Name rootCertName = keyChain.createIdentity(root);
  shared_ptr<IdentityCertificate> rootCert =
    keyChain.getCertificate(rootCertName);
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
    "    regex ^<localhost><nrd>[<register><unregister><advertise><withdraw>]<>{3}$\n"
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

  keyChain.deleteIdentity(root);

  const boost::filesystem::path CERT_PATH =
    (boost::filesystem::current_path() / std::string("trust-anchor-8.cert"));
  boost::filesystem::remove(CERT_PATH);
}

BOOST_AUTO_TEST_CASE(Wildcard)
{
  KeyChain keyChain;

  Name identity("/TestValidatorConfig/Wildcard");
  identity.appendVersion();
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));

  Name dataName1("/any/data");
  shared_ptr<Data> data1 = make_shared<Data>(dataName1);
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data1, identity));

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
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  keyChain.deleteIdentity(identity);
}


BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
