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
 */

#include "security/key-chain.hpp"
#include <boost/filesystem.hpp>

#include "boost-test.hpp"

using namespace std;

namespace ndn {
namespace tests {

class KeychainConfigFileFixture
{
public:
  KeychainConfigFileFixture()
  {
    if (std::getenv("TEST_HOME"))
      m_HOME = std::getenv("TEST_HOME");
  }

  ~KeychainConfigFileFixture()
  {
    if (!m_HOME.empty())
      setenv("TEST_HOME", m_HOME.c_str(), 1);
    else
      unsetenv("TEST_HOME");
  }

protected:
  std::string m_HOME;
};

BOOST_FIXTURE_TEST_SUITE(SecurityTestKeyChain, KeychainConfigFileFixture)

BOOST_AUTO_TEST_CASE(ConstructorNormalConfig)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-home", 1);

  BOOST_REQUIRE_NO_THROW(KeyChain());

  path pibPath(absolute(std::getenv("TEST_HOME")));
  pibPath /= ".ndn/ndnsec-public-info.db";

  boost::filesystem::remove(pibPath);
}

BOOST_AUTO_TEST_CASE(ConstructorEmptyConfig)
{
  using namespace boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/security/config-file-empty-home", 1);

  BOOST_REQUIRE_NO_THROW(KeyChain());

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
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestKeyChain/ExportIdentity/");
  identity.appendVersion();
  keyChain.createIdentity(identity);

  shared_ptr<SecuredBag> exported = keyChain.exportIdentity(identity, "1234");

  Block block = exported->wireEncode();

  Name keyName = keyChain.getDefaultKeyNameForIdentity(identity);
  Name certName = keyChain.getDefaultCertificateNameForKey(keyName);

  keyChain.deleteIdentity(identity);

  BOOST_REQUIRE(keyChain.doesIdentityExist(identity) == false);
  BOOST_REQUIRE(keyChain.doesPublicKeyExist(keyName) == false);
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
  BOOST_REQUIRE(keyChain.doesCertificateExist(certName) == false);

  SecuredBag imported;
  imported.wireDecode(block);
  keyChain.importIdentity(imported, "1234");

  BOOST_REQUIRE(keyChain.doesIdentityExist(identity));
  BOOST_REQUIRE(keyChain.doesPublicKeyExist(keyName));
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE));
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC));
  BOOST_REQUIRE(keyChain.doesCertificateExist(certName));

  keyChain.deleteIdentity(identity);

  BOOST_REQUIRE(keyChain.doesIdentityExist(identity) == false);
  BOOST_REQUIRE(keyChain.doesPublicKeyExist(keyName) == false);
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
  BOOST_REQUIRE(keyChain.doesCertificateExist(certName) == false);
}

BOOST_AUTO_TEST_CASE(PrepareIdentityCertificate)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestKeyChain/PrepareIdentityCertificate/");
  identity.appendVersion();
  keyChain.createIdentity(identity);

  vector<CertificateSubjectDescription> subjectDescription;
  Name lowerIdentity = identity;
  lowerIdentity.append("Lower").appendVersion();
  Name lowerKeyName = keyChain.generateRsaKeyPair(lowerIdentity, true);
  shared_ptr<IdentityCertificate> idCert
    = keyChain.prepareUnsignedIdentityCertificate(lowerKeyName, identity,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(365),
                                                  subjectDescription);
  BOOST_CHECK(static_cast<bool>(idCert));
  BOOST_CHECK(idCert->getName().getPrefix(5) ==
              Name().append(identity).append("KEY").append("Lower"));


  Name anotherIdentity("/TestKeyChain/PrepareIdentityCertificate/Another/");
  anotherIdentity.appendVersion();
  Name anotherKeyName = keyChain.generateRsaKeyPair(anotherIdentity, true);
  shared_ptr<IdentityCertificate> idCert2
    = keyChain.prepareUnsignedIdentityCertificate(anotherKeyName, identity,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(365),
                                                  subjectDescription);
  BOOST_CHECK(static_cast<bool>(idCert2));
  BOOST_CHECK(idCert2->getName().getPrefix(5) == Name().append(anotherIdentity).append("KEY"));


  Name wrongKeyName1;
  shared_ptr<IdentityCertificate> idCert3
    = keyChain.prepareUnsignedIdentityCertificate(wrongKeyName1, identity,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(365),
                                                  subjectDescription);
  BOOST_CHECK(!static_cast<bool>(idCert3));


  Name wrongKeyName2("/TestKeyChain/PrepareIdentityCertificate");
  shared_ptr<IdentityCertificate> idCert4
    = keyChain.prepareUnsignedIdentityCertificate(wrongKeyName2, identity,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(365),
                                                  subjectDescription);
  BOOST_CHECK(!static_cast<bool>(idCert4));


  Name wrongKeyName3("/TestKeyChain/PrepareIdentityCertificate/ksk-1234");
  shared_ptr<IdentityCertificate> idCert5
    = keyChain.prepareUnsignedIdentityCertificate(wrongKeyName3, identity,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(365),
                                                  subjectDescription);
  BOOST_CHECK(!static_cast<bool>(idCert5));

  keyChain.deleteIdentity(identity);
  keyChain.deleteIdentity(lowerIdentity);
  keyChain.deleteIdentity(anotherIdentity);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
