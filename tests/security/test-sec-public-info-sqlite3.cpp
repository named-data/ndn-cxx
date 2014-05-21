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

#include "security/sec-public-info-sqlite3.hpp"
#include "security/key-chain.hpp"
#include "util/time.hpp"

#include "boost-test.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestSecPublicInfoSqlite3)

BOOST_AUTO_TEST_CASE(Delete)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

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

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
