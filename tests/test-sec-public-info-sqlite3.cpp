/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif

#include <boost/test/unit_test.hpp>

#include "security/key-chain.hpp"

using namespace std;
using namespace ndn;


BOOST_AUTO_TEST_SUITE(TestSecPublicInfoSqlite3)

BOOST_AUTO_TEST_CASE (Delete)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keyChain;

  Name identity("/tmp");
  Name certName1 = keyChain.createIdentity(identity);
  Name keyName1 = IdentityCertificate::certificateNameToPublicKeyName(certName1);  
  Name keyName2 = keyChain.generateRSAKeyPairAsDefault(identity);
  ptr_lib::shared_ptr<IdentityCertificate> cert2 = keyChain.selfSign(keyName2);
  Name certName2 = cert2->getName();
  keyChain.addCertificateAsKeyDefault(*cert2);
  Name keyName3 = keyChain.generateRSAKeyPairAsDefault(identity);
  ptr_lib::shared_ptr<IdentityCertificate> cert3 = keyChain.selfSign(keyName3);
  Name certName3 = cert3->getName();
  keyChain.addCertificateAsKeyDefault(*cert3);
  ptr_lib::shared_ptr<IdentityCertificate> cert4 = keyChain.selfSign(keyName3);
  Name certName4 = cert4->getName();
  keyChain.addCertificateAsKeyDefault(*cert4);
  ptr_lib::shared_ptr<IdentityCertificate> cert5 = keyChain.selfSign(keyName3);
  Name certName5 = cert5->getName();
  keyChain.addCertificateAsKeyDefault(*cert5);

  BOOST_REQUIRE_EQUAL(keyChain.doesIdentityExist(identity), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesPublicKeyExist(keyName1), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesPublicKeyExist(keyName2), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesPublicKeyExist(keyName3), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName1), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName2), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName3), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName4), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName5), true);

  keyChain.deleteCertificate(certName5);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName5), false);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName3), true);
  BOOST_REQUIRE_EQUAL(keyChain.doesCertificateExist(certName4), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName3), true);

  keyChain.deleteKey(keyName3);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName4), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName3), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName3), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName2), true);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName1), true);
  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), true);

  keyChain.deleteIdentity(identity);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName2), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName2), false);
  BOOST_CHECK_EQUAL(keyChain.doesCertificateExist(certName1), false);
  BOOST_CHECK_EQUAL(keyChain.doesPublicKeyExist(keyName1), false);
  BOOST_CHECK_EQUAL(keyChain.doesIdentityExist(identity), false);
}

BOOST_AUTO_TEST_SUITE_END()
