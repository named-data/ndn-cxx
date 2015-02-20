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

#include "security/key-chain.hpp"
#include "../util/test-home-environment-fixture.hpp"
#include <boost/filesystem.hpp>

#include "boost-test.hpp"
#include "dummy-keychain.hpp"

namespace ndn {
namespace tests {

using std::vector;

BOOST_FIXTURE_TEST_SUITE(SecurityKeyChain, util::TestHomeEnvironmentFixture)

BOOST_AUTO_TEST_CASE(ConstructorNormalConfig)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-home", 1);

  BOOST_REQUIRE_NO_THROW(KeyChain());

  KeyChain keyChain;
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(),
                    "pib-sqlite3:/tmp/test/ndn-cxx/keychain/sqlite3-file/");
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(),
                    "tpm-file:/tmp/test/ndn-cxx/keychain/sqlite3-file/");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(),
                    "tpm-file:/tmp/test/ndn-cxx/keychain/sqlite3-file/");

  path pibPath(absolute(std::getenv("TEST_HOME")));
  pibPath /= ".ndn/ndnsec-public-info.db";

  boost::filesystem::remove(pibPath);
}

BOOST_AUTO_TEST_CASE(ConstructorEmptyConfig)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-empty-home", 1);

#if defined(NDN_CXX_HAVE_OSX_SECURITY)
  std::string oldHOME;
  if (std::getenv("OLD_HOME"))
    oldHOME = std::getenv("OLD_HOME");

  std::string HOME;
  if (std::getenv("HOME"))
    HOME = std::getenv("HOME");

  if (!oldHOME.empty())
    setenv("HOME", oldHOME.c_str(), 1);
  else
    unsetenv("HOME");
#endif

  BOOST_REQUIRE_NO_THROW(KeyChain());
  KeyChain keyChain;
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(),
                    "pib-sqlite3:/tmp/test/ndn-cxx/keychain/sqlite3-empty/");

#if defined(NDN_CXX_HAVE_OSX_SECURITY)
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-osxkeychain:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-osxkeychain:");
#else
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(),
                    "tpm-file:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(),
                    "tpm-file:");
#endif

#if defined(NDN_CXX_HAVE_OSX_SECURITY)
  if (!HOME.empty())
    setenv("HOME", HOME.c_str(), 1);
  else
    unsetenv("HOME");
#endif

  path pibPath(absolute(std::getenv("TEST_HOME")));
  pibPath /= ".ndn/ndnsec-public-info.db";

  boost::filesystem::remove(pibPath);
}

BOOST_AUTO_TEST_CASE(ConstructorEmpty2Config)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-empty2-home", 1);

  BOOST_REQUIRE_NO_THROW(KeyChain());

  KeyChain keyChain;
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(),
                    "pib-sqlite3:");
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(),
                    "tpm-file:/tmp/test/ndn-cxx/keychain/empty-file/");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(),
                    "tpm-file:/tmp/test/ndn-cxx/keychain/empty-file/");

  path pibPath(absolute(std::getenv("TEST_HOME")));
  pibPath /= ".ndn/ndnsec-public-info.db";

  boost::filesystem::remove(pibPath);
}

BOOST_AUTO_TEST_CASE(ConstructorMalConfig)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-malformed-home", 1);

  BOOST_REQUIRE_THROW(KeyChain(), KeyChain::Error); // Wrong configuration. Error expected.
}

BOOST_AUTO_TEST_CASE(ConstructorMal2Config)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-malformed2-home", 1);

  BOOST_REQUIRE_THROW(KeyChain(), KeyChain::Error); // Wrong configuration. Error expected.
}

BOOST_AUTO_TEST_CASE(ExportIdentity)
{
  KeyChain keyChain;

  Name identity("/TestKeyChain/ExportIdentity/");
  identity.appendVersion();
  keyChain.createIdentity(identity);

  shared_ptr<SecuredBag> exported = keyChain.exportIdentity(identity, "1234");

  Block block = exported->wireEncode();

  Name keyName = keyChain.getDefaultKeyNameForIdentity(identity);
  Name certName = keyChain.getDefaultCertificateNameForKey(keyName);

  keyChain.deleteIdentity(identity);

  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName), false);
  BOOST_CHECK_EQUAL(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_CHECK_EQUAL(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName), false);

  SecuredBag imported;
  imported.wireDecode(block);
  keyChain.importIdentity(imported, "1234");

  BOOST_CHECK(keyChain.doesIdentityExist(identity));
  BOOST_CHECK(keyChain.doesPublicKeyExist(keyName));
  BOOST_CHECK(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE));
  BOOST_CHECK(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC));
  BOOST_CHECK(keyChain.doesCertificateExist(certName));

  keyChain.deleteIdentity(identity);

  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName), false);
  BOOST_CHECK_EQUAL(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_CHECK_EQUAL(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName), false);
}

BOOST_AUTO_TEST_CASE(PrepareIdentityCertificate)
{
  KeyChain keyChain;

  Name identity("/TestKeyChain/PrepareIdentityCertificate/");
  identity.appendVersion();
  keyChain.createIdentity(identity);

  vector<CertificateSubjectDescription> subjectDescription;
  Name lowerIdentity = identity;
  lowerIdentity.append("Lower").appendVersion();
  Name lowerKeyName = keyChain.generateRsaKeyPair(lowerIdentity, true);
  shared_ptr<IdentityCertificate> idCert =
    keyChain.prepareUnsignedIdentityCertificate(lowerKeyName, identity,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(365),
                                                subjectDescription);
  BOOST_CHECK(static_cast<bool>(idCert));
  BOOST_CHECK_EQUAL(idCert->getName().getPrefix(5),
                    Name().append(identity).append("KEY").append("Lower"));

  shared_ptr<IdentityCertificate> idCert11 =
    keyChain.prepareUnsignedIdentityCertificate(lowerKeyName, identity,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(365),
                                                subjectDescription,
                                                lowerIdentity);
  BOOST_CHECK(static_cast<bool>(idCert11));
  BOOST_CHECK_EQUAL(idCert11->getName().getPrefix(6),
              Name().append(lowerIdentity).append("KEY"));

  Name anotherIdentity("/TestKeyChain/PrepareIdentityCertificate/Another/");
  anotherIdentity.appendVersion();
  Name anotherKeyName = keyChain.generateRsaKeyPair(anotherIdentity, true);
  shared_ptr<IdentityCertificate> idCert2 =
    keyChain.prepareUnsignedIdentityCertificate(anotherKeyName, identity,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(365),
                                                subjectDescription);
  BOOST_CHECK(static_cast<bool>(idCert2));
  BOOST_CHECK_EQUAL(idCert2->getName().getPrefix(5), Name().append(anotherIdentity).append("KEY"));


  Name wrongKeyName1;
  shared_ptr<IdentityCertificate> idCert3 =
    keyChain.prepareUnsignedIdentityCertificate(wrongKeyName1, identity,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(365),
                                                subjectDescription);
  BOOST_CHECK_EQUAL(static_cast<bool>(idCert3), false);


  Name wrongKeyName2("/TestKeyChain/PrepareIdentityCertificate");
  shared_ptr<IdentityCertificate> idCert4 =
    keyChain.prepareUnsignedIdentityCertificate(wrongKeyName2, identity,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(365),
                                                subjectDescription);
  BOOST_CHECK_EQUAL(static_cast<bool>(idCert4), false);


  Name wrongKeyName3("/TestKeyChain/PrepareIdentityCertificate/ksk-1234");
  shared_ptr<IdentityCertificate> idCert5 =
    keyChain.prepareUnsignedIdentityCertificate(wrongKeyName3, identity,
                                                time::system_clock::now(),
                                                time::system_clock::now() + time::days(365),
                                                subjectDescription);
  BOOST_CHECK_EQUAL(static_cast<bool>(idCert5), false);

  keyChain.deleteIdentity(identity);
  keyChain.deleteIdentity(lowerIdentity);
  keyChain.deleteIdentity(anotherIdentity);
}

BOOST_AUTO_TEST_CASE(Delete)
{
  KeyChain keyChain;

  Name identity("/TestSecPublicInfoSqlite3/Delete");
  identity.appendVersion();

  Name certName1;
  BOOST_REQUIRE_NO_THROW(certName1 = keyChain.createIdentity(identity));

  Name keyName1 = IdentityCertificate::certificateNameToPublicKeyName(certName1);
  Name keyName2;
  BOOST_REQUIRE_NO_THROW(keyName2 = keyChain.generateRsaKeyPairAsDefault(identity));

  shared_ptr<IdentityCertificate> cert2;
  BOOST_REQUIRE_NO_THROW(cert2 = keyChain.selfSign(keyName2));
  Name certName2 = cert2->getName();
  BOOST_REQUIRE_NO_THROW(keyChain.addCertificateAsKeyDefault(*cert2));

  Name keyName3;
  BOOST_REQUIRE_NO_THROW(keyName3 = keyChain.generateRsaKeyPairAsDefault(identity));

  shared_ptr<IdentityCertificate> cert3;
  BOOST_REQUIRE_NO_THROW(cert3 = keyChain.selfSign(keyName3));
  Name certName3 = cert3->getName();
  BOOST_REQUIRE_NO_THROW(keyChain.addCertificateAsKeyDefault(*cert3));
  shared_ptr<IdentityCertificate> cert4;
  BOOST_REQUIRE_NO_THROW(cert4 = keyChain.selfSign(keyName3));
  Name certName4 = cert4->getName();
  BOOST_REQUIRE_NO_THROW(keyChain.addCertificateAsKeyDefault(*cert4));
  shared_ptr<IdentityCertificate> cert5;
  BOOST_REQUIRE_NO_THROW(cert5 = keyChain.selfSign(keyName3));
  Name certName5 = cert5->getName();
  BOOST_REQUIRE_NO_THROW(keyChain.addCertificateAsKeyDefault(*cert5));

  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName1), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName2), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName3), true);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName1), true);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName2), true);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName3), true);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName4), true);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName5), true);

  BOOST_REQUIRE_NO_THROW(keyChain.deleteCertificate(certName5));
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName5), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName3), true);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName4), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName3), true);

  BOOST_REQUIRE_NO_THROW(keyChain.deleteKey(keyName3));
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName4), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName3), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName3), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName2), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName1), true);
  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), true);

  BOOST_REQUIRE_NO_THROW(keyChain.deleteIdentity(identity));
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName2), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName2), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName1), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName1), false);
  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), false);
}

BOOST_AUTO_TEST_CASE(KeyChainWithCustomTpmAndPib)
{
  BOOST_REQUIRE_NO_THROW((KeyChain("pib-dummy", "tpm-dummy")));
  BOOST_REQUIRE_NO_THROW((KeyChain("pib-dummy2", "tpm-dummy2")));
  BOOST_REQUIRE_NO_THROW((KeyChain("dummy", "dummy")));
  BOOST_REQUIRE_NO_THROW((KeyChain("dummy:", "dummy:")));
  BOOST_REQUIRE_NO_THROW((KeyChain("dummy:/something", "dummy:/something")));

  KeyChain keyChain("dummy", "dummy");
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-dummy:");
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-dummy:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-dummy:");
  BOOST_CHECK_EQUAL(keyChain.getDefaultIdentity(), "/dummy/key");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
