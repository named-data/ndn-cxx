/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "security/pib/key-container.hpp"
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
BOOST_FIXTURE_TEST_SUITE(TestKeyContainer, PibDataFixture)

using pib::Pib;

BOOST_AUTO_TEST_CASE(Basic)
{
  auto pibImpl = make_shared<PibMemory>();

  // start with an empty container
  KeyContainer container(id1, pibImpl);
  BOOST_CHECK_EQUAL(container.size(), 0);
  BOOST_CHECK_EQUAL(container.getLoadedKeys().size(), 0);

  // add the first key
  Key key11 = container.add(id1Key1.data(), id1Key1.size(), id1Key1Name);
  BOOST_CHECK_EQUAL(key11.getName(), id1Key1Name);
  BOOST_CHECK(key11.getPublicKey() == id1Key1);
  BOOST_CHECK_EQUAL(container.size(), 1);
  BOOST_CHECK_EQUAL(container.getLoadedKeys().size(), 1);
  BOOST_CHECK(container.find(id1Key1Name) != container.end());

  // add the same key again
  Key key12 = container.add(id1Key1.data(), id1Key1.size(), id1Key1Name);
  BOOST_CHECK_EQUAL(key12.getName(), id1Key1Name);
  BOOST_CHECK(key12.getPublicKey() == id1Key1);
  BOOST_CHECK_EQUAL(container.size(), 1);
  BOOST_CHECK_EQUAL(container.getLoadedKeys().size(), 1);
  BOOST_CHECK(container.find(id1Key1Name) != container.end());

  // add the second key
  Key key21 = container.add(id1Key2.data(), id1Key2.size(), id1Key2Name);
  BOOST_CHECK_EQUAL(key21.getName(), id1Key2Name);
  BOOST_CHECK(key21.getPublicKey() == id1Key2);
  BOOST_CHECK_EQUAL(container.size(), 2);
  BOOST_CHECK_EQUAL(container.getLoadedKeys().size(), 2);
  BOOST_CHECK(container.find(id1Key1Name) != container.end());
  BOOST_CHECK(container.find(id1Key2Name) != container.end());

  // get keys
  BOOST_REQUIRE_NO_THROW(container.get(id1Key1Name));
  BOOST_REQUIRE_NO_THROW(container.get(id1Key2Name));
  Name id1Key3Name = v2::constructKeyName(id1, name::Component("non-existing-id"));
  BOOST_CHECK_THROW(container.get(id1Key3Name), Pib::Error);

  // check key
  Key key1 = container.get(id1Key1Name);
  Key key2 = container.get(id1Key2Name);
  BOOST_CHECK_EQUAL(key1.getName(), id1Key1Name);
  BOOST_CHECK(key1.getPublicKey() == id1Key1);
  BOOST_CHECK_EQUAL(key2.getName(), id1Key2Name);
  BOOST_CHECK(key2.getPublicKey() == id1Key2);

  // create another container from the same PibImpl
  // cache should be empty
  KeyContainer container2(id1, pibImpl);
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getLoadedKeys().size(), 0);

  // get key, cache should be filled
  BOOST_REQUIRE_NO_THROW(container2.get(id1Key1Name));
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getLoadedKeys().size(), 1);

  BOOST_REQUIRE_NO_THROW(container2.get(id1Key2Name));
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getLoadedKeys().size(), 2);

  // remove a key
  container2.remove(id1Key1Name);
  BOOST_CHECK_EQUAL(container2.size(), 1);
  BOOST_CHECK_EQUAL(container2.getLoadedKeys().size(), 1);
  BOOST_CHECK(container2.find(id1Key1Name) == container2.end());
  BOOST_CHECK(container2.find(id1Key2Name) != container2.end());

  // remove another key
  container2.remove(id1Key2Name);
  BOOST_CHECK_EQUAL(container2.size(), 0);
  BOOST_CHECK_EQUAL(container2.getLoadedKeys().size(), 0);
  BOOST_CHECK(container2.find(id1Key2Name) == container2.end());
}

BOOST_AUTO_TEST_CASE(Errors)
{
  auto pibImpl = make_shared<PibMemory>();

  KeyContainer container(id1, pibImpl);

  BOOST_CHECK_THROW(container.add(id2Key1.data(), id2Key1.size(), id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(container.remove(id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(container.get(id2Key1Name), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(Iterator)
{
  auto pibImpl = make_shared<PibMemory>();
  KeyContainer container(id1, pibImpl);

  container.add(id1Key1.data(), id1Key1.size(), id1Key1Name);
  container.add(id1Key2.data(), id1Key2.size(), id1Key2Name);

  std::set<Name> keyNames;
  keyNames.insert(id1Key1Name);
  keyNames.insert(id1Key2Name);

  KeyContainer::const_iterator it = container.begin();
  std::set<Name>::const_iterator testIt = keyNames.begin();
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  it++;
  testIt++;
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  ++it;
  testIt++;
  BOOST_CHECK(it == container.end());

  size_t count = 0;
  testIt = keyNames.begin();
  for (const auto& key : container) {
    BOOST_CHECK_EQUAL(key.getIdentity(), id1);
    BOOST_CHECK_EQUAL(key.getName(), *testIt);
    testIt++;
    count++;
  }
  BOOST_CHECK_EQUAL(count, 2);

  BOOST_CHECK(KeyContainer::const_iterator() == KeyContainer::const_iterator());
  BOOST_CHECK(KeyContainer::const_iterator() == container.end());
  BOOST_CHECK(container.end() == KeyContainer::const_iterator());
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyContainer
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
