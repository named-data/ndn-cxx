/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "security/key-chain.hpp"

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestKeyChain)

BOOST_AUTO_TEST_CASE (ExportIdentity)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keyChain;

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

BOOST_AUTO_TEST_CASE (PrepareIdentityCertificate)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keyChain;
  
  Name identity("/TestKeyChain/PrepareIdentityCertificate/");
  identity.appendVersion();
  keyChain.createIdentity(identity);

  vector<CertificateSubjectDescription> subjectDescription;
  Name lowerIdentity = identity;
  lowerIdentity.append("Lower").appendVersion();
  Name lowerKeyName = keyChain.generateRSAKeyPair(lowerIdentity, true);
  shared_ptr<IdentityCertificate> idCert
    = keyChain.prepareUnsignedIdentityCertificate(lowerKeyName, identity,
						  time::system_clock::now(),
						  time::system_clock::now() + time::days(365),
						  subjectDescription);
  BOOST_CHECK(static_cast<bool>(idCert));
  BOOST_CHECK(idCert->getName().getPrefix(5) == Name().append(identity).append("KEY").append("Lower"));


  Name anotherIdentity("/TestKeyChain/PrepareIdentityCertificate/Another/");
  anotherIdentity.appendVersion();
  Name anotherKeyName = keyChain.generateRSAKeyPair(anotherIdentity, true);
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

} // namespace ndn
