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

#include "ndn-cxx/security/pib/key.hpp"
#include "ndn-cxx/security/pib/impl/key-impl.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn::tests {

using namespace ndn::security::pib;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestKey, PibDataFixture)

BOOST_AUTO_TEST_CASE(ValidityChecking)
{
  Key key;
  BOOST_TEST(!key);
  BOOST_TEST(key == Key());
  BOOST_CHECK_THROW(key.getName(), std::domain_error);

  auto impl = std::make_shared<KeyImpl>(id1Key1Name, id1Key1, makePibWithKey(id1Key1Name, id1Key1));
  key = Key(impl);
  BOOST_TEST(key);
  BOOST_TEST(key != Key());
  BOOST_TEST(key.getName() == id1Key1Name);

  impl.reset();
  BOOST_TEST(!key);
  BOOST_CHECK_THROW(key.getName(), std::domain_error);
}

// pib::Key is a wrapper of pib::KeyImpl. Since the functionality of KeyImpl is
// already tested in key-impl.t.cpp, we only test the shared property of pib::Key
// in this test case.
BOOST_AUTO_TEST_CASE(SharedImpl)
{
  auto keyImpl = std::make_shared<KeyImpl>(id1Key1Name, id1Key1, makePibWithKey(id1Key1Name, id1Key1));
  Key key1(keyImpl);
  Key key2(keyImpl);

  BOOST_TEST(key1 == key2);
  BOOST_TEST(key1 != Key());
  BOOST_TEST(Key() != key2);
  BOOST_TEST(Key() == Key());

  BOOST_CHECK_THROW(key2.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  key1.addCertificate(id1Key1Cert1);
  BOOST_TEST(key2.getCertificate(id1Key1Cert1.getName()) == id1Key1Cert1);

  key2.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key1.getCertificate(id1Key1Cert1.getName()), Pib::Error);

  key1.setDefaultCertificate(id1Key1Cert1);
  BOOST_TEST(key2.getDefaultCertificate() == id1Key1Cert1);
}

BOOST_AUTO_TEST_CASE(Helpers)
{
  using ndn::security::constructKeyName;
  using ndn::security::isValidKeyName;
  using ndn::security::extractIdentityFromKeyName;

  BOOST_CHECK_EQUAL(constructKeyName("/hello", name::Component("world")), "/hello/KEY/world");

  BOOST_CHECK_EQUAL(isValidKeyName("/hello"), false);
  BOOST_CHECK_EQUAL(isValidKeyName("/hello/KEY"), false);
  BOOST_CHECK_EQUAL(isValidKeyName("/hello/KEY/world"), true);

  BOOST_CHECK_EQUAL(isValidKeyName("/KEY/hello"), true);
  BOOST_CHECK_EQUAL(isValidKeyName("/hello/world/KEY/!"), true);

  BOOST_CHECK_EQUAL(extractIdentityFromKeyName("/KEY/hello"), "/");
  BOOST_CHECK_EQUAL(extractIdentityFromKeyName("/hello/KEY/world"), "/hello");
  BOOST_CHECK_EQUAL(extractIdentityFromKeyName("/hello/world/KEY/!"), "/hello/world");

  BOOST_CHECK_THROW(extractIdentityFromKeyName("/hello"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestKey
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
