/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/security/pib/impl/key-impl.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn::tests {

using namespace ndn::security::pib;

BOOST_AUTO_TEST_SUITE(Security)

class KeyImplFixture : public PibDataFixture
{
protected:
  const shared_ptr<PibImpl> pibImpl = makePibWithKey(id1Key1Name, id1Key1);
  KeyImpl key11{id1Key1Name, id1Key1, pibImpl};
};

BOOST_FIXTURE_TEST_SUITE(TestKeyImpl, KeyImplFixture)

BOOST_AUTO_TEST_CASE(Properties)
{
  BOOST_TEST(key11.getIdentity() == id1);
  BOOST_TEST(key11.getName() == id1Key1Name);
  BOOST_TEST(key11.getKeyType() == KeyType::EC);
  BOOST_TEST(key11.getPublicKey() == id1Key1, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(CertificateOperations)
{
  // key does not have any certificates
  BOOST_CHECK_EQUAL(key11.getCertificates().size(), 0);

  // get non-existing certificate, throw Pib::Error
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  // get default certificate, throw Pib::Error
  BOOST_CHECK_THROW(key11.getDefaultCertificate(), Pib::Error);
  // set non-existing certificate as default certificate, throw Pib::Error
  BOOST_CHECK_THROW(key11.setDefaultCertificate(id1Key1Cert1.getName()), Pib::Error);

  // add certificate
  key11.addCertificate(id1Key1Cert1);
  const auto& addedCert = key11.getCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL(addedCert, id1Key1Cert1);

  // new certificate becomes default certificate when there was no default certificate
  const auto& defaultCert0 = key11.getDefaultCertificate();
  BOOST_CHECK_EQUAL(defaultCert0, id1Key1Cert1);

  // remove certificate
  key11.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  BOOST_CHECK_THROW(key11.getDefaultCertificate(), Pib::Error);

  // set default certificate directly
  BOOST_REQUIRE_NO_THROW(key11.setDefaultCertificate(id1Key1Cert1));
  const auto& defaultCert1 = key11.getDefaultCertificate();
  BOOST_CHECK_EQUAL(defaultCert1, id1Key1Cert1);

  // add another certificate
  key11.addCertificate(id1Key1Cert2);
  BOOST_CHECK_EQUAL(key11.getCertificates().size(), 2);

  // set default certificate through name and check return value
  BOOST_CHECK_EQUAL(key11.setDefaultCertificate(id1Key1Cert2.getName()), id1Key1Cert2);
  const auto& defaultCert2 = key11.getDefaultCertificate();
  BOOST_CHECK_EQUAL(defaultCert2, id1Key1Cert2);

  // remove certificate
  key11.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  BOOST_CHECK_EQUAL(key11.getCertificates().size(), 1);

  // set removed certificate as default, certificate is implicitly added
  BOOST_REQUIRE_NO_THROW(key11.setDefaultCertificate(id1Key1Cert1));
  const auto& defaultCert3 = key11.getDefaultCertificate();
  BOOST_CHECK_EQUAL(defaultCert3, id1Key1Cert1);
  BOOST_CHECK_EQUAL(key11.getCertificates().size(), 2);

  // remove all certificates
  key11.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  BOOST_CHECK_EQUAL(key11.getCertificates().size(), 1);
  key11.removeCertificate(id1Key1Cert2.getName());
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert2.getName()), Pib::Error);
  BOOST_CHECK_EQUAL(key11.getCertificates().size(), 0);
  BOOST_CHECK_THROW(key11.getDefaultCertificate(), Pib::Error);
}

class ReplaceFixture : public KeyChainFixture, public KeyImplFixture
{
};

BOOST_FIXTURE_TEST_CASE(ReplaceCertificate, ReplaceFixture)
{
  key11.addCertificate(id1Key1Cert1);
  BOOST_CHECK_EQUAL(key11.getCertificate(id1Key1Cert1.getName()), id1Key1Cert1);

  auto otherCert = id1Key1Cert1;
  SignatureInfo info;
  info.setValidityPeriod(security::ValidityPeriod::makeRelative(-1_s, 10_s));
  m_keyChain.sign(otherCert, security::SigningInfo().setSignatureInfo(info));
  BOOST_TEST(otherCert.getName() == id1Key1Cert1.getName());
  BOOST_TEST(otherCert.getContent() == id1Key1Cert1.getContent());
  BOOST_TEST(otherCert != id1Key1Cert1);

  key11.addCertificate(otherCert); // overwrite cert
  BOOST_TEST(key11.getCertificate(id1Key1Cert1.getName()) == otherCert);
}

BOOST_AUTO_TEST_CASE(Errors)
{
  // illegal key name
  BOOST_CHECK_THROW(KeyImpl(Name("/wrong"), id1Key1, pibImpl), std::invalid_argument);

  BOOST_CHECK_THROW(key11.addCertificate(id1Key2Cert1), std::invalid_argument);
  BOOST_CHECK_THROW(key11.removeCertificate(id1Key2Cert1.getName()), std::invalid_argument);
  BOOST_CHECK_THROW(key11.getCertificate(id1Key2Cert1.getName()), std::invalid_argument);
  BOOST_CHECK_THROW(key11.setDefaultCertificate(id1Key2Cert1), std::invalid_argument);
  BOOST_CHECK_THROW(key11.setDefaultCertificate(id1Key2Cert1.getName()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(UnknownKeyType)
{
  Name keyName = security::constructKeyName(id1, name::Component::fromEscapedString("foo"));
  Buffer invalidKey{0x01, 0x02, 0x03, 0x04};
  pibImpl->addKey(id1, keyName, invalidKey);

  KeyImpl unknown(keyName, invalidKey, pibImpl);
  BOOST_TEST(unknown.getIdentity() == id1);
  BOOST_TEST(unknown.getName() == keyName);
  BOOST_TEST(unknown.getKeyType() == KeyType::NONE);
  BOOST_TEST(unknown.getPublicKey() == invalidKey, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyImpl
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
