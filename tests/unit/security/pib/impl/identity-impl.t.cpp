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

#include "ndn-cxx/security/pib/impl/identity-impl.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn::tests {

using namespace ndn::security::pib;

BOOST_AUTO_TEST_SUITE(Security)

class IdentityImplFixture : public PibDataFixture
{
protected:
  const shared_ptr<PibImpl> pibImpl = makePibWithIdentity(id1);
  IdentityImpl identity1{id1, pibImpl};
};

BOOST_FIXTURE_TEST_SUITE(TestIdentityImpl, IdentityImplFixture)

BOOST_AUTO_TEST_CASE(Properties)
{
  BOOST_TEST(identity1.getName() == id1);
}

BOOST_AUTO_TEST_CASE(KeyOperations)
{
  // identity does not have any keys
  BOOST_CHECK_EQUAL(identity1.getKeys().size(), 0);

  // get non-existing key, throw Pib::Error
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  // get default key, throw Pib::Error
  BOOST_CHECK_THROW(identity1.getDefaultKey(), Pib::Error);
  // set non-existing key as default key, throw Pib::Error
  BOOST_CHECK_THROW(identity1.setDefaultKey(id1Key1Name), Pib::Error);

  // add key
  identity1.addKey(id1Key1, id1Key1Name);
  const auto& addedKey = identity1.getKey(id1Key1Name);
  BOOST_CHECK_EQUAL(addedKey.getName(), id1Key1Name);
  BOOST_TEST(addedKey.getPublicKey() == id1Key1, boost::test_tools::per_element());

  // new key becomes default key when there is no default key
  const auto& defaultKey0 = identity1.getDefaultKey();
  BOOST_CHECK_EQUAL(defaultKey0.getName(), id1Key1Name);
  BOOST_TEST(defaultKey0.getPublicKey() == id1Key1, boost::test_tools::per_element());

  // remove key
  identity1.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  BOOST_CHECK_THROW(identity1.getDefaultKey(), Pib::Error);

  // set default key directly
  BOOST_REQUIRE_NO_THROW(identity1.setDefaultKey(id1Key1, id1Key1Name));
  const auto& defaultKey1 = identity1.getDefaultKey();
  BOOST_CHECK_EQUAL(defaultKey1.getName(), id1Key1Name);
  BOOST_TEST(defaultKey1.getPublicKey() == id1Key1, boost::test_tools::per_element());

  // add another key
  identity1.addKey(id1Key2, id1Key2Name);
  BOOST_CHECK_EQUAL(identity1.getKeys().size(), 2);

  // set default key through name
  BOOST_REQUIRE_NO_THROW(identity1.setDefaultKey(id1Key2Name));
  const auto& defaultKey2 = identity1.getDefaultKey();
  BOOST_CHECK_EQUAL(defaultKey2.getName(), id1Key2Name);
  BOOST_TEST(defaultKey2.getPublicKey() == id1Key2, boost::test_tools::per_element());

  // remove key
  identity1.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  BOOST_CHECK_EQUAL(identity1.getKeys().size(), 1);

  // set default key directly again, change the default setting
  BOOST_REQUIRE_NO_THROW(identity1.setDefaultKey(id1Key1, id1Key1Name));
  const auto& defaultKey3 = identity1.getDefaultKey();
  BOOST_CHECK_EQUAL(defaultKey3.getName(), id1Key1Name);
  BOOST_TEST(defaultKey3.getPublicKey() == id1Key1, boost::test_tools::per_element());
  BOOST_CHECK_EQUAL(identity1.getKeys().size(), 2);

  // remove all keys
  identity1.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  BOOST_CHECK_EQUAL(identity1.getKeys().size(), 1);
  identity1.removeKey(id1Key2Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key2Name), Pib::Error);
  BOOST_CHECK_EQUAL(identity1.getKeys().size(), 0);
  BOOST_CHECK_THROW(identity1.getDefaultKey(), Pib::Error);
}

BOOST_AUTO_TEST_CASE(ReplaceKey)
{
  identity1.addKey(id1Key1, id1Key1Name);
  auto k1 = identity1.getKey(id1Key1Name);
  BOOST_TEST(k1.getPublicKey() == id1Key1, boost::test_tools::per_element());

  identity1.addKey(id1Key2, id1Key1Name); // overwrite key
  auto k2 = identity1.getKey(id1Key1Name);
  BOOST_TEST(k2.getPublicKey() == id1Key2, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(Errors)
{
  BOOST_CHECK_THROW(identity1.addKey(id2Key1, id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(identity1.removeKey(id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(identity1.getKey(id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(identity1.setDefaultKey(id2Key1, id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(identity1.setDefaultKey(id2Key1Name), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestIdentityImpl
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
