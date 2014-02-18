/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "security/key-chain.hpp"
#include "util/time.hpp"

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestKeyChain)

BOOST_AUTO_TEST_CASE (ExportIdentity)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keyChain;

  Name identity(string("/TestKeyChain/ExportIdentity/") + boost::lexical_cast<std::string>(time::now()));
  keyChain.createIdentity(identity);
  
  Block exported = keyChain.exportIdentity(identity, "1234");

  Name keyName = keyChain.getDefaultKeyNameForIdentity(identity);
  Name certName = keyChain.getDefaultCertificateNameForKey(keyName);

  keyChain.deleteIdentity(identity);

  BOOST_REQUIRE(keyChain.doesIdentityExist(identity) == false);
  BOOST_REQUIRE(keyChain.doesPublicKeyExist(keyName) == false);
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(keyChain.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
  BOOST_REQUIRE(keyChain.doesCertificateExist(certName) == false);

  keyChain.importIdentity(exported, "1234");

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

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
