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

#include "security/pib-memory.hpp"
#include "security/pib-sqlite3.hpp"
#include "security/pib.hpp"
#include "pib-data-fixture.hpp"

#include <boost/filesystem.hpp>
#include <boost/mpl/list.hpp>
#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityPibImpl)

class PibMemoryWrapper
{
public:
  PibMemory impl;
};

class PibSqlite3Wrapper
{
public:
  PibSqlite3Wrapper()
    : tmpPath(boost::filesystem::path(UNIT_TEST_CONFIG_PATH) / "DbTest")
    , impl(tmpPath.c_str())
  {
  }

  ~PibSqlite3Wrapper()
  {
    boost::filesystem::remove_all(tmpPath);
  }

  boost::filesystem::path tmpPath;
  PibSqlite3 impl;
};

typedef boost::mpl::list<PibMemoryWrapper,
                         PibSqlite3Wrapper> PibImpls;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(IdentityManagement, T, PibImpls, PibDataFixture)
{
  T wrapper;
  PibImpl& pibImpl = wrapper.impl;

  // no default setting, throw Error
  BOOST_CHECK_THROW(pibImpl.getDefaultIdentity(), Pib::Error);

  // check id1, which should not exist
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), false);

  // add id1, should be default
  pibImpl.addIdentity(id1);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), true);
  BOOST_CHECK_NO_THROW(pibImpl.getDefaultIdentity());
  BOOST_CHECK_EQUAL(pibImpl.getDefaultIdentity(), id1);

  // add id2, should not be default
  pibImpl.addIdentity(id2);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id2), true);
  BOOST_CHECK_EQUAL(pibImpl.getDefaultIdentity(), id1);

  // set id2 explicitly as default
  pibImpl.setDefaultIdentity(id2);
  BOOST_CHECK_EQUAL(pibImpl.getDefaultIdentity(), id2);

  // remove id2, should not have default identity
  pibImpl.removeIdentity(id2);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id2), false);
  BOOST_CHECK_THROW(pibImpl.getDefaultIdentity(), Pib::Error);

  // add id2 again, should be default
  pibImpl.addIdentity(id2);
  BOOST_CHECK_EQUAL(pibImpl.getDefaultIdentity(), id2);

  // get all identities, should contain id1 and id2
  std::set<Name> idNames = pibImpl.getIdentities();
  BOOST_CHECK_EQUAL(idNames.size(), 2);
  BOOST_CHECK_EQUAL(idNames.count(id1), 1);
  BOOST_CHECK_EQUAL(idNames.count(id2), 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(KeyManagement, T, PibImpls, PibDataFixture)
{
  T wrapper;
  PibImpl& pibImpl = wrapper.impl;

  // no default setting, throw Error
  BOOST_CHECK_THROW(pibImpl.getDefaultKeyOfIdentity(id1), Pib::Error);

  // check id1Key1, should not exist, neither should id1.
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), false);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), false);

  // add id1Key1, should be default, id1 should be added implicitly
  pibImpl.addKey(id1, id1Key1Name.get(-1), id1Key1);
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), true);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), true);
  const PublicKey& keyBits = pibImpl.getKeyBits(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL_COLLECTIONS(keyBits.get().buf(), keyBits.get().buf() + keyBits.get().size(),
                                id1Key1.get().buf(), id1Key1.get().buf() + id1Key1.get().size());
  BOOST_CHECK_NO_THROW(pibImpl.getDefaultKeyOfIdentity(id1));
  BOOST_CHECK_EQUAL(pibImpl.getDefaultKeyOfIdentity(id1), id1Key1Name.get(-1));

  // add id1Key2, should not be default
  pibImpl.addKey(id1, id1Key2Name.get(-1), id1Key2);
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key2Name.get(-1)), true);
  BOOST_CHECK_EQUAL(pibImpl.getDefaultKeyOfIdentity(id1), id1Key1Name.get(-1));

  // set id1Key2 explicitly as default
  pibImpl.setDefaultKeyOfIdentity(id1, id1Key2Name.get(-1));
  BOOST_CHECK_EQUAL(pibImpl.getDefaultKeyOfIdentity(id1), id1Key2Name.get(-1));

  // set a non-existing key as default, throw Error
  BOOST_CHECK_THROW(pibImpl.setDefaultKeyOfIdentity(id1, name::Component("non-existing")),
                    Pib::Error);

  // remove id1Key2, should not have default key
  pibImpl.removeKey(id1, id1Key2Name.get(-1));
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key2Name.get(-1)), false);
  BOOST_CHECK_THROW(pibImpl.getKeyBits(id1, id1Key2Name.get(-1)), Pib::Error);
  BOOST_CHECK_THROW(pibImpl.getDefaultKeyOfIdentity(id1), Pib::Error);

  // add id1Key2 back, should be default
  pibImpl.addKey(id1, id1Key2Name.get(-1), id1Key2);
  BOOST_CHECK_NO_THROW(pibImpl.getKeyBits(id1, id1Key2Name.get(-1)));
  BOOST_CHECK_EQUAL(pibImpl.getDefaultKeyOfIdentity(id1), id1Key2Name.get(-1));

  // get all the keys: id1Key1 and id1Key2
  std::set<name::Component> keyNames = pibImpl.getKeysOfIdentity(id1);
  BOOST_CHECK_EQUAL(keyNames.size(), 2);
  BOOST_CHECK_EQUAL(keyNames.count(id1Key1Name.get(-1)), 1);
  BOOST_CHECK_EQUAL(keyNames.count(id1Key2Name.get(-1)), 1);

  // remove id1, should remove all the keys
  pibImpl.removeIdentity(id1);
  keyNames = pibImpl.getKeysOfIdentity(id1);
  BOOST_CHECK_EQUAL(keyNames.size(), 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(CertificateManagement, T, PibImpls, PibDataFixture)
{
  T wrapper;
  PibImpl& pibImpl = wrapper.impl;

  // no default setting, throw Error
  BOOST_CHECK_THROW(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), Pib::Error);

  // check id1Key1Cert1, should not exist, neither should id1 and id1Key1
  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert1.getName()), false);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), false);
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), false);

  // add id1Key1Cert1, should be default, id1 and id1Key1 should be added implicitly
  pibImpl.addCertificate(id1Key1Cert1);
  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert1.getName()), true);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), true);
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), true);
  const IdentityCertificate& cert = pibImpl.getCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL_COLLECTIONS(cert.wireEncode().wire(),
                                cert.wireEncode().wire() + cert.wireEncode().size(),
                                id1Key1Cert1.wireEncode().wire(),
                                id1Key1Cert1.wireEncode().wire() + id1Key1Cert1.wireEncode().size());
  BOOST_CHECK_NO_THROW(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)));
  BOOST_CHECK_EQUAL(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), id1Key1Cert1);

  // add id1Key1Cert2, should not be default
  pibImpl.addCertificate(id1Key1Cert2);
  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert2.getName()), true);
  BOOST_CHECK_EQUAL(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), id1Key1Cert1);

  // set id1Key1Cert2 explicitly as default
  pibImpl.setDefaultCertificateOfKey(id1, id1Key1Name.get(-1), id1Key1Cert2.getName());
  BOOST_CHECK_EQUAL(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), id1Key1Cert2);

  // set a non-existing cert as default, throw Error
  BOOST_CHECK_THROW(pibImpl.setDefaultCertificateOfKey(id1, id1Key1Name.get(-1), Name("/non-existing")),
                    Pib::Error);

  // remove id1Key1Cert2, should not have default cert
  pibImpl.removeCertificate(id1Key1Cert2.getName());
  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert2.getName()), false);
  BOOST_CHECK_THROW(pibImpl.getCertificate(id1Key1Cert2.getName()), Pib::Error);
  BOOST_CHECK_THROW(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), Pib::Error);

  // add id1Key1Cert2, should be default
  pibImpl.addCertificate(id1Key1Cert2);
  BOOST_CHECK_NO_THROW(pibImpl.getCertificate(id1Key1Cert1.getName()));
  BOOST_CHECK_EQUAL(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), id1Key1Cert2);

  // get all certificates: id1Key1Cert1 and id1Key1Cert2
  std::set<Name> certNames = pibImpl.getCertificatesOfKey(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL(certNames.size(), 2);
  BOOST_CHECK_EQUAL(certNames.count(id1Key1Cert1.getName()), 1);
  BOOST_CHECK_EQUAL(certNames.count(id1Key1Cert2.getName()), 1);

  // remove id1Key1, should remove all the certs
  pibImpl.removeKey(id1, id1Key1Name.get(-1));
  certNames = pibImpl.getCertificatesOfKey(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL(certNames.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
