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

#include "security/in-memory-pib-impl.hpp"
#include "security/pib.hpp"
#include "pib-data-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityInMemoryPibImpl)

BOOST_AUTO_TEST_CASE(TpmLocatorManagement)
{
  InMemoryPibImpl pibImpl;

  BOOST_CHECK_EQUAL(pibImpl.getTpmLocator(), "tpm-memory:");
  BOOST_CHECK_THROW(pibImpl.setTpmLocator(""), PibImpl::Error);
}

BOOST_FIXTURE_TEST_CASE(IdentityManagement, PibDataFixture)
{
  InMemoryPibImpl pibImpl;

  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), false);
  pibImpl.addIdentity(id1);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), true);
  pibImpl.removeIdentity(id1);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), false);

  BOOST_CHECK_THROW(pibImpl.getDefaultIdentity(), Pib::Error);
  pibImpl.setDefaultIdentity(id1);
  BOOST_CHECK_EQUAL(pibImpl.hasIdentity(id1), true);
  BOOST_CHECK_EQUAL(pibImpl.getDefaultIdentity(), id1);

  pibImpl.addIdentity(id2);
  std::set<Name> idNames = pibImpl.getIdentities();
  BOOST_CHECK_EQUAL(idNames.size(), 2);
  BOOST_CHECK_EQUAL(idNames.count(id1), 1);
  BOOST_CHECK_EQUAL(idNames.count(id2), 1);
}

BOOST_FIXTURE_TEST_CASE(KeyManagement, PibDataFixture)
{
  InMemoryPibImpl pibImpl;

  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), false);
  pibImpl.addKey(id1, id1Key1Name.get(-1), id1Key1);
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), true);
  pibImpl.removeKey(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL(pibImpl.hasKey(id1, id1Key1Name.get(-1)), false);

  BOOST_CHECK_THROW(pibImpl.getKeyBits(id1, id1Key1Name.get(-1)), Pib::Error);
  pibImpl.addKey(id1, id1Key1Name.get(-1), id1Key1);
  BOOST_CHECK_NO_THROW(pibImpl.getKeyBits(id1, id1Key1Name.get(-1)));
  const PublicKey& keyBits = pibImpl.getKeyBits(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL_COLLECTIONS(keyBits.get().buf(), keyBits.get().buf() + keyBits.get().size(),
                                id1Key1.get().buf(), id1Key1.get().buf() + id1Key1.get().size());

  pibImpl.addKey(id1, id1Key2Name.get(-1), id1Key2);
  pibImpl.addKey(id2, id2Key2Name.get(-1), id2Key2);

  std::set<name::Component> keyNames = pibImpl.getKeysOfIdentity(id1);
  BOOST_CHECK_EQUAL(keyNames.size(), 2);
  BOOST_CHECK_EQUAL(keyNames.count(id1Key1Name.get(-1)), 1);
  BOOST_CHECK_EQUAL(keyNames.count(id1Key2Name.get(-1)), 1);

  BOOST_CHECK_THROW(pibImpl.getDefaultKeyOfIdentity(id1), Pib::Error);
  pibImpl.setDefaultKeyOfIdentity(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL(pibImpl.getDefaultKeyOfIdentity(id1), id1Key1Name.get(-1));
  BOOST_CHECK_THROW(pibImpl.setDefaultKeyOfIdentity(id1, name::Component("non-existing")),
                    Pib::Error);
}

BOOST_FIXTURE_TEST_CASE(CertificateManagement, PibDataFixture)
{
  InMemoryPibImpl pibImpl;

  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert1.getName()), false);
  pibImpl.addCertificate(id1Key1Cert1);
  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert1.getName()), true);
  pibImpl.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL(pibImpl.hasCertificate(id1Key1Cert1.getName()), false);

  BOOST_CHECK_THROW(pibImpl.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  pibImpl.addCertificate(id1Key1Cert1);
  BOOST_CHECK_NO_THROW(pibImpl.getCertificate(id1Key1Cert1.getName()));
  const IdentityCertificate& cert = pibImpl.getCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL_COLLECTIONS(cert.wireEncode().wire(),
                                cert.wireEncode().wire() + cert.wireEncode().size(),
                                id1Key1Cert1.wireEncode().wire(),
                                id1Key1Cert1.wireEncode().wire() + id1Key1Cert1.wireEncode().size());

  pibImpl.addCertificate(id1Key1Cert2);
  pibImpl.addCertificate(id1Key2Cert2);

  std::set<Name> certNames = pibImpl.getCertificatesOfKey(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL(certNames.size(), 2);
  BOOST_CHECK_EQUAL(certNames.count(id1Key1Cert1.getName()), 1);
  BOOST_CHECK_EQUAL(certNames.count(id1Key1Cert2.getName()), 1);

  BOOST_CHECK_THROW(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)), Pib::Error);
  pibImpl.setDefaultCertificateOfKey(id1, id1Key1Name.get(-1), id1Key1Cert1.getName());
  BOOST_CHECK_NO_THROW(pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1)));
  const IdentityCertificate& defaultCert =
    pibImpl.getDefaultCertificateOfKey(id1, id1Key1Name.get(-1));
  BOOST_CHECK_EQUAL_COLLECTIONS(defaultCert.wireEncode().wire(),
                                defaultCert.wireEncode().wire() + defaultCert.wireEncode().size(),
                                id1Key1Cert1.wireEncode().wire(),
                                id1Key1Cert1.wireEncode().wire() + id1Key1Cert1.wireEncode().size());

  BOOST_CHECK_THROW(pibImpl.setDefaultCertificateOfKey(id1, id1Key1Name.get(-1), id1Key2Cert1.getName()),
                    Pib::Error);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
