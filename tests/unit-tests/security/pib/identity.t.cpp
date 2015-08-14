/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "security/pib/identity.hpp"
#include "security/pib/pib.hpp"
#include "security/pib/pib-memory.hpp"

#include "boost-test.hpp"
#include "pib-data-fixture.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace tests {

using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_FIXTURE_TEST_SUITE(TestIdentity, PibDataFixture)

using pib::Pib;

BOOST_AUTO_TEST_CASE(ValidityChecking)
{
  // identity
  Identity id;

  BOOST_CHECK_EQUAL(static_cast<bool>(id), false);
  BOOST_CHECK_EQUAL(!id, true);

  if (id)
    BOOST_CHECK(false);
  else
    BOOST_CHECK(true);

  id = Identity(id1, make_shared<PibMemory>(), true);

  BOOST_CHECK_EQUAL(static_cast<bool>(id), true);
  BOOST_CHECK_EQUAL(!id, false);

  if (id)
    BOOST_CHECK(true);
  else
    BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE(KeyOperations)
{
  Identity identity1(id1, make_shared<PibMemory>(), true);

  // Key does not exist, throw Error
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  // Key name does not match identity name, throw Error
  BOOST_CHECK_THROW(identity1.getKey(id2Key1Name), Pib::Error);

  // Add key
  Key key11 = identity1.addKey(id1Key1.buf(), id1Key1.size(), id1Key1Name);
  BOOST_CHECK_NO_THROW(identity1.getKey(id1Key1Name));
  // Key name does not match identity name, throw Error
  BOOST_CHECK_THROW(identity1.addKey(id2Key1.buf(), id2Key1.size(), id2Key1Name), Pib::Error);

  // Remove key
  identity1.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  // Key name does not match identity name, throw Error
  BOOST_CHECK_THROW(identity1.removeKey(id2Key1Name), Pib::Error);

  // Default key does not exist, throw Error
  BOOST_CHECK_THROW(identity1.getDefaultKey(), Pib::Error);

  // Set default key but the key does not exist, throw Error
  BOOST_CHECK_THROW(identity1.setDefaultKey(id1Key1Name), Pib::Error);
  // Set default key
  BOOST_REQUIRE_NO_THROW(identity1.setDefaultKey(id1Key1.buf(), id1Key1.size(), id1Key1Name));
  BOOST_CHECK_NO_THROW(identity1.getDefaultKey());
  BOOST_CHECK_EQUAL(identity1.getDefaultKey().getName(), id1Key1Name);

  // Remove the default key
  identity1.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);
  BOOST_CHECK_THROW(identity1.getDefaultKey(), Pib::Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestIdentity
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
