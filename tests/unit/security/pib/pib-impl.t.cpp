/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "security/pib/pib-memory.hpp"
#include "security/pib/pib-sqlite3.hpp"
#include "security/pib/pib.hpp"
#include "security/security-common.hpp"

#include "boost-test.hpp"
#include "pib-data-fixture.hpp"

#include <boost/filesystem.hpp>
#include <boost/mpl/list.hpp>

namespace ndn {
namespace security {
namespace pib {
namespace tests {

using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_AUTO_TEST_SUITE(TestPibImpl)

using pib::Pib;

class PibMemoryFixture : public PibDataFixture
{
public:
  PibMemory pib;
};

class PibSqlite3Fixture : public PibDataFixture
{
public:
  PibSqlite3Fixture()
    : tmpPath(boost::filesystem::path(UNIT_TEST_CONFIG_PATH) / "DbTest")
    , pib(tmpPath.c_str())
  {
  }

  ~PibSqlite3Fixture()
  {
    boost::filesystem::remove_all(tmpPath);
  }

public:
  boost::filesystem::path tmpPath;
  PibSqlite3 pib;
};

typedef boost::mpl::list<PibMemoryFixture,
                         PibSqlite3Fixture> PibImpls;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(TpmLocator, T, PibImpls, T)
{
  // Basic getting and setting
  BOOST_CHECK_NO_THROW(this->pib.getTpmLocator());

  BOOST_CHECK_NO_THROW(this->pib.setTpmLocator("tpmLocator"));
  BOOST_CHECK_EQUAL(this->pib.getTpmLocator(), "tpmLocator");

  // Add cert, and do not change TPM locator
  this->pib.addCertificate(this->id1Key1Cert1);
  BOOST_CHECK(this->pib.hasIdentity(this->id1));
  BOOST_CHECK(this->pib.hasKey(this->id1Key1Name));
  BOOST_CHECK(this->pib.hasCertificate(this->id1Key1Cert1.getName()));

  // Set TPM locator to the same value, nothing should change
  this->pib.setTpmLocator("tpmLocator");
  BOOST_CHECK(this->pib.hasIdentity(this->id1));
  BOOST_CHECK(this->pib.hasKey(this->id1Key1Name));
  BOOST_CHECK(this->pib.hasCertificate(this->id1Key1Cert1.getName()));

  // Change TPM locator (contents of PIB should not change)
  this->pib.setTpmLocator("newTpmLocator");
  BOOST_CHECK(this->pib.hasIdentity(this->id1));
  BOOST_CHECK(this->pib.hasKey(this->id1Key1Name));
  BOOST_CHECK(this->pib.hasCertificate(this->id1Key1Cert1.getName()));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(IdentityManagement, T, PibImpls, T)
{
  // no default setting, throw Error
  BOOST_CHECK_THROW(this->pib.getDefaultIdentity(), Pib::Error);

  // check id1, which should not exist
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id1), false);

  // add id1, should be default
  this->pib.addIdentity(this->id1);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id1), true);
  BOOST_CHECK_NO_THROW(this->pib.getDefaultIdentity());
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id1);

  // add id2, should not be default
  this->pib.addIdentity(this->id2);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id2), true);
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id1);

  // set id2 explicitly as default
  this->pib.setDefaultIdentity(this->id2);
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id2);

  // remove id2, should not have default identity
  this->pib.removeIdentity(this->id2);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id2), false);
  BOOST_CHECK_THROW(this->pib.getDefaultIdentity(), Pib::Error);

  // add id2 again, should be default
  this->pib.addIdentity(this->id2);
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id2);

  // get all identities, should contain id1 and id2
  std::set<Name> idNames = this->pib.getIdentities();
  BOOST_CHECK_EQUAL(idNames.size(), 2);
  BOOST_CHECK_EQUAL(idNames.count(this->id1), 1);
  BOOST_CHECK_EQUAL(idNames.count(this->id2), 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ClearIdentities, T, PibImpls, T)
{
  this->pib.setTpmLocator("tpmLocator");

  // Add id, key, and cert
  this->pib.addCertificate(this->id1Key1Cert1);
  BOOST_CHECK(this->pib.hasIdentity(this->id1));
  BOOST_CHECK(this->pib.hasKey(this->id1Key1Name));
  BOOST_CHECK(this->pib.hasCertificate(this->id1Key1Cert1.getName()));

  // Clear identities
  this->pib.clearIdentities();
  BOOST_CHECK_EQUAL(this->pib.getIdentities().size(), 0);
  BOOST_CHECK_EQUAL(this->pib.getKeysOfIdentity(this->id1).size(), 0);
  BOOST_CHECK_EQUAL(this->pib.getCertificatesOfKey(this->id1Key1Name).size(), 0);
  BOOST_CHECK_EQUAL(this->pib.getTpmLocator(), "tpmLocator");
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(KeyManagement, T, PibImpls, T)
{
  // no default setting, throw Error
  BOOST_CHECK_THROW(this->pib.getDefaultKeyOfIdentity(this->id1), Pib::Error);

  // check id1Key1, should not exist, neither should id1.
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key1Name), false);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id1), false);

  // add id1Key1, should be default, id1 should be added implicitly
  this->pib.addKey(this->id1, this->id1Key1Name, this->id1Key1.data(), this->id1Key1.size());
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key1Name), true);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id1), true);
  const Buffer& keyBits = this->pib.getKeyBits(this->id1Key1Name);
  BOOST_CHECK(keyBits == this->id1Key1);
  BOOST_CHECK_NO_THROW(this->pib.getDefaultKeyOfIdentity(this->id1));
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id1), this->id1Key1Name);

  // add id1Key2, should not be default
  this->pib.addKey(this->id1, this->id1Key2Name, this->id1Key2.data(), this->id1Key2.size());
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key2Name), true);
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id1), this->id1Key1Name);

  // set id1Key2 explicitly as default
  this->pib.setDefaultKeyOfIdentity(this->id1, this->id1Key2Name);
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id1), this->id1Key2Name);

  // set a non-existing key as default, throw Error
  BOOST_CHECK_THROW(this->pib.setDefaultKeyOfIdentity(this->id1, Name("/non-existing")),
                    Pib::Error);

  // remove id1Key2, should not have default key
  this->pib.removeKey(this->id1Key2Name);
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key2Name), false);
  BOOST_CHECK_THROW(this->pib.getKeyBits(this->id1Key2Name), Pib::Error);
  BOOST_CHECK_THROW(this->pib.getDefaultKeyOfIdentity(this->id1), Pib::Error);

  // add id1Key2 back, should be default
  this->pib.addKey(this->id1, this->id1Key2Name, this->id1Key2.data(), this->id1Key2.size());
  BOOST_CHECK_NO_THROW(this->pib.getKeyBits(this->id1Key2Name));
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id1), this->id1Key2Name);

  // get all the keys: id1Key1 and id1Key2
  std::set<Name> keyNames = this->pib.getKeysOfIdentity(this->id1);
  BOOST_CHECK_EQUAL(keyNames.size(), 2);
  BOOST_CHECK_EQUAL(keyNames.count(this->id1Key1Name), 1);
  BOOST_CHECK_EQUAL(keyNames.count(this->id1Key2Name), 1);

  // remove id1, should remove all the keys
  this->pib.removeIdentity(this->id1);
  keyNames = this->pib.getKeysOfIdentity(this->id1);
  BOOST_CHECK_EQUAL(keyNames.size(), 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(CertificateManagement, T, PibImpls, T)
{
  // no default setting, throw Error
  BOOST_CHECK_THROW(this->pib.getDefaultCertificateOfKey(this->id1Key1Name), Pib::Error);

  // check id1Key1Cert1, should not exist, neither should id1 and id1Key1
  BOOST_CHECK_EQUAL(this->pib.hasCertificate(this->id1Key1Cert1.getName()), false);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id1), false);
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key1Name), false);

  // add id1Key1Cert1, should be default, id1 and id1Key1 should be added implicitly
  this->pib.addCertificate(this->id1Key1Cert1);
  BOOST_CHECK_EQUAL(this->pib.hasCertificate(this->id1Key1Cert1.getName()), true);
  BOOST_CHECK_EQUAL(this->pib.hasIdentity(this->id1), true);
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key1Name), true);
  BOOST_CHECK_EQUAL(this->pib.getCertificate(this->id1Key1Cert1.getName()).wireEncode(),
                    this->id1Key1Cert1.wireEncode());
  BOOST_CHECK_NO_THROW(this->pib.getDefaultCertificateOfKey(this->id1Key1Name));
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id1Key1Name), this->id1Key1Cert1);

  // add id1Key1Cert2, should not be default
  this->pib.addCertificate(this->id1Key1Cert2);
  BOOST_CHECK_EQUAL(this->pib.hasCertificate(this->id1Key1Cert2.getName()), true);
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id1Key1Name), this->id1Key1Cert1);

  // set id1Key1Cert2 explicitly as default
  this->pib.setDefaultCertificateOfKey(this->id1Key1Name, this->id1Key1Cert2.getName());
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id1Key1Name), this->id1Key1Cert2);

  // set a non-existing cert as default, throw Error
  BOOST_CHECK_THROW(this->pib.setDefaultCertificateOfKey(this->id1Key1Name, Name("/non-existing")),
                    Pib::Error);

  // remove id1Key1Cert2, should not have default cert
  this->pib.removeCertificate(this->id1Key1Cert2.getName());
  BOOST_CHECK_EQUAL(this->pib.hasCertificate(this->id1Key1Cert2.getName()), false);
  BOOST_CHECK_THROW(this->pib.getCertificate(this->id1Key1Cert2.getName()), Pib::Error);
  BOOST_CHECK_THROW(this->pib.getDefaultCertificateOfKey(this->id1Key1Name), Pib::Error);

  // add id1Key1Cert2, should be default
  this->pib.addCertificate(this->id1Key1Cert2);
  BOOST_CHECK_NO_THROW(this->pib.getCertificate(this->id1Key1Cert1.getName()));
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id1Key1Name), this->id1Key1Cert2);

  // get all certificates: id1Key1Cert1 and id1Key1Cert2
  std::set<Name> certNames = this->pib.getCertificatesOfKey(this->id1Key1Name);
  BOOST_CHECK_EQUAL(certNames.size(), 2);
  BOOST_CHECK_EQUAL(certNames.count(this->id1Key1Cert1.getName()), 1);
  BOOST_CHECK_EQUAL(certNames.count(this->id1Key1Cert2.getName()), 1);

  // remove id1Key1, should remove all the certs
  this->pib.removeKey(this->id1Key1Name);
  certNames = this->pib.getCertificatesOfKey(this->id1Key1Name);
  BOOST_CHECK_EQUAL(certNames.size(), 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(DefaultsManagement, T, PibImpls, T)
{
  this->pib.addIdentity(this->id1);
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id1);

  this->pib.addIdentity(this->id2);
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id1);

  this->pib.removeIdentity(this->id1);
  BOOST_CHECK_THROW(this->pib.getDefaultIdentity(), Pib::Error);

  this->pib.addKey(this->id2, this->id2Key1Name, this->id2Key1.data(), this->id2Key1.size());
  BOOST_CHECK_EQUAL(this->pib.getDefaultIdentity(), this->id2);
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id2), this->id2Key1Name);

  this->pib.addKey(this->id2, this->id2Key2Name, this->id2Key2.data(), this->id2Key2.size());
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id2), this->id2Key1Name);

  this->pib.removeKey(this->id2Key1Name);
  BOOST_CHECK_THROW(this->pib.getDefaultKeyOfIdentity(this->id2), Pib::Error);

  this->pib.addCertificate(this->id2Key2Cert1);
  BOOST_CHECK_EQUAL(this->pib.getDefaultKeyOfIdentity(this->id2), this->id2Key2Name);
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id2Key2Name).getName(), this->id2Key2Cert1.getName());

  this->pib.addCertificate(this->id2Key2Cert2);
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id2Key2Name).getName(), this->id2Key2Cert1.getName());

  this->pib.removeCertificate(this->id2Key2Cert2.getName());
  BOOST_CHECK_EQUAL(this->pib.getDefaultCertificateOfKey(this->id2Key2Name).getName(), this->id2Key2Cert1.getName());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Overwrite, T, PibImpls, T)
{
  // check id1Key1, should not exist
  this->pib.removeIdentity(this->id1);
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key1Name), false);

  // add id1Key1
  this->pib.addKey(this->id1, this->id1Key1Name, this->id1Key1.data(), this->id1Key1.size());
  BOOST_CHECK_EQUAL(this->pib.hasKey(this->id1Key1Name), true);
  const Buffer& keyBits = this->pib.getKeyBits(this->id1Key1Name);
  BOOST_CHECK(keyBits == this->id1Key1);

  // check overwrite, add a key with the same name.
  this->pib.addKey(this->id1, this->id1Key1Name, this->id1Key2.data(), this->id1Key2.size());
  const Buffer& keyBits2 = this->pib.getKeyBits(this->id1Key1Name);
  BOOST_CHECK(keyBits2 == this->id1Key2);

  // check id1Key1Cert1, should not exist
  this->pib.removeIdentity(this->id1);
  BOOST_CHECK_EQUAL(this->pib.hasCertificate(this->id1Key1Cert1.getName()), false);

  // add id1Key1Cert1
  this->pib.addKey(this->id1, this->id1Key1Name, this->id1Key1.data(), this->id1Key1.size());
  this->pib.addCertificate(this->id1Key1Cert1);
  BOOST_CHECK_EQUAL(this->pib.hasCertificate(this->id1Key1Cert1.getName()), true);

  auto cert = this->pib.getCertificate(this->id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL(cert.wireEncode(), this->id1Key1Cert1.wireEncode());

  // Create a fake cert with the same name
  auto cert2 = this->id1Key2Cert1;
  cert2.setName(this->id1Key1Cert1.getName());
  cert2.setSignature(this->id1Key2Cert1.getSignature());
  this->pib.addCertificate(cert2);

  auto cert3 = this->pib.getCertificate(this->id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL(cert3.wireEncode(), cert2.wireEncode());

  // both key and certificate are overwritten
  Buffer keyBits3 = this->pib.getKeyBits(this->id1Key1Name);
  BOOST_CHECK(keyBits3 == this->id1Key2);
}

BOOST_AUTO_TEST_SUITE_END() // TestPibImpl
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
