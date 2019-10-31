/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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
#include "ndn-cxx/security/pib/impl/pib-memory.hpp"
#include "ndn-cxx/security/pib/pib.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace tests {

using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_FIXTURE_TEST_SUITE(TestKey, PibDataFixture)

BOOST_AUTO_TEST_CASE(ValidityChecking)
{
  Key key;
  BOOST_CHECK(!key);
  BOOST_CHECK_EQUAL(static_cast<bool>(key), false);

  auto keyImpl = make_shared<detail::KeyImpl>(id1Key1Name, id1Key1.data(), id1Key1.size(),
                                              make_shared<pib::PibMemory>());
  key = Key(keyImpl);
  BOOST_CHECK(key);
  BOOST_CHECK_EQUAL(!key, false);
}

// pib::Key is a wrapper of pib::detail::KeyImpl.  Since the functionalities of KeyImpl
// have already been tested in detail/key-impl.t.cpp, we only test the shared property
// of pib::Key in this test case.
BOOST_AUTO_TEST_CASE(SharedImpl)
{
  auto keyImpl = make_shared<detail::KeyImpl>(id1Key1Name, id1Key1.data(), id1Key1.size(),
                                              make_shared<pib::PibMemory>());
  Key key1(keyImpl);
  Key key2(keyImpl);
  BOOST_CHECK_EQUAL(key1, key2);
  BOOST_CHECK_NE(key1, Key());
  BOOST_CHECK_EQUAL(Key(), Key());

  key1.addCertificate(id1Key1Cert1);
  BOOST_CHECK_NO_THROW(key2.getCertificate(id1Key1Cert1.getName()));
  key2.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key1.getCertificate(id1Key1Cert1.getName()), pib::Pib::Error);

  key1.setDefaultCertificate(id1Key1Cert1);
  BOOST_CHECK_NO_THROW(key2.getDefaultCertificate());
}

BOOST_AUTO_TEST_CASE(Helpers)
{
  BOOST_CHECK_EQUAL(v2::constructKeyName("/hello", name::Component("world")), "/hello/KEY/world");

  BOOST_CHECK_EQUAL(v2::isValidKeyName("/hello"), false);
  BOOST_CHECK_EQUAL(v2::isValidKeyName("/hello/KEY"), false);
  BOOST_CHECK_EQUAL(v2::isValidKeyName("/hello/KEY/world"), true);

  BOOST_CHECK_EQUAL(v2::isValidKeyName("/KEY/hello"), true);
  BOOST_CHECK_EQUAL(v2::isValidKeyName("/hello/world/KEY/!"), true);

  BOOST_CHECK_EQUAL(v2::extractIdentityFromKeyName("/KEY/hello"), "/");
  BOOST_CHECK_EQUAL(v2::extractIdentityFromKeyName("/hello/KEY/world"), "/hello");
  BOOST_CHECK_EQUAL(v2::extractIdentityFromKeyName("/hello/world/KEY/!"), "/hello/world");

  BOOST_CHECK_THROW(v2::extractIdentityFromKeyName("/hello"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestKey
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
