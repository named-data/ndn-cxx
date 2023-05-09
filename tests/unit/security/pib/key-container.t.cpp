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

#include "ndn-cxx/security/pib/key-container.hpp"
#include "ndn-cxx/security/pib/impl/pib-memory.hpp"
#include "ndn-cxx/util/concepts.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn::tests {

using namespace ndn::security::pib;

NDN_CXX_ASSERT_FORWARD_ITERATOR(KeyContainer::const_iterator);

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestKeyContainer, PibDataFixture)

BOOST_AUTO_TEST_CASE(AddGetRemove)
{
  auto pibImpl = make_shared<PibMemory>();

  {
    // start with an empty container
    KeyContainer container(id1, pibImpl);
    BOOST_CHECK_EQUAL(container.size(), 0);
    BOOST_CHECK_EQUAL(container.m_keys.size(), 0);

    // add the first key
    Key key11 = container.add(id1Key1, id1Key1Name);
    BOOST_CHECK_EQUAL(key11.getName(), id1Key1Name);
    BOOST_TEST(key11.getPublicKey() == id1Key1, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(container.size(), 1);
    BOOST_CHECK_EQUAL(container.m_keys.size(), 1);
    BOOST_CHECK(container.find(id1Key1Name) != container.end());

    // add the same key again
    Key key12 = container.add(id1Key1, id1Key1Name);
    BOOST_CHECK_EQUAL(key12.getName(), id1Key1Name);
    BOOST_TEST(key12.getPublicKey() == id1Key1, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(container.size(), 1);
    BOOST_CHECK_EQUAL(container.m_keys.size(), 1);
    BOOST_CHECK(container.find(id1Key1Name) != container.end());

    // add the second key
    Key key21 = container.add(id1Key2, id1Key2Name);
    BOOST_CHECK_EQUAL(key21.getName(), id1Key2Name);
    BOOST_TEST(key21.getPublicKey() == id1Key2, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(container.size(), 2);
    BOOST_CHECK_EQUAL(container.m_keys.size(), 2);
    BOOST_CHECK(container.find(id1Key1Name) != container.end());
    BOOST_CHECK(container.find(id1Key2Name) != container.end());

    // check keys
    Key key1 = container.get(id1Key1Name);
    Key key2 = container.get(id1Key2Name);
    BOOST_CHECK_EQUAL(key1.getName(), id1Key1Name);
    BOOST_TEST(key1.getPublicKey() == id1Key1, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(key2.getName(), id1Key2Name);
    BOOST_TEST(key2.getPublicKey() == id1Key2, boost::test_tools::per_element());
    Name id1Key3Name = security::constructKeyName(id1, name::Component("non-existing-id"));
    BOOST_CHECK_THROW(container.get(id1Key3Name), Pib::Error);
  }

  {
    // create a container from an existing (non-empty) PibImpl
    // names are loaded immediately but the key cache should initially be empty
    KeyContainer container2(id1, pibImpl);
    BOOST_CHECK_EQUAL(container2.size(), 2);
    BOOST_CHECK_EQUAL(container2.m_keys.size(), 0);

    // fetching the keys should populate the cache
    BOOST_CHECK_EQUAL(container2.get(id1Key1Name).getName(), id1Key1Name);
    BOOST_CHECK_EQUAL(container2.size(), 2);
    BOOST_CHECK_EQUAL(container2.m_keys.size(), 1);

    BOOST_CHECK_EQUAL(container2.get(id1Key2Name).getName(), id1Key2Name);
    BOOST_CHECK_EQUAL(container2.size(), 2);
    BOOST_CHECK_EQUAL(container2.m_keys.size(), 2);

    // remove a key
    container2.remove(id1Key1Name);
    BOOST_CHECK_EQUAL(container2.size(), 1);
    BOOST_CHECK_EQUAL(container2.m_keys.size(), 1);
    BOOST_CHECK(container2.find(id1Key1Name) == container2.end());
    BOOST_CHECK(container2.find(id1Key2Name) != container2.end());

    // removing the same key again is a no-op
    container2.remove(id1Key1Name);
    BOOST_CHECK_EQUAL(container2.size(), 1);
    BOOST_CHECK_EQUAL(container2.m_keys.size(), 1);
    BOOST_CHECK(container2.find(id1Key1Name) == container2.end());
    BOOST_CHECK(container2.find(id1Key2Name) != container2.end());

    // remove another key
    container2.remove(id1Key2Name);
    BOOST_CHECK_EQUAL(container2.size(), 0);
    BOOST_CHECK_EQUAL(container2.m_keys.size(), 0);
    BOOST_CHECK(container2.find(id1Key2Name) == container2.end());
  }
}

BOOST_AUTO_TEST_CASE(Errors)
{
  auto pibImpl = make_shared<PibMemory>();
  KeyContainer container(id1, pibImpl);

  BOOST_CHECK_THROW(container.add(id2Key1, id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(container.remove(id2Key1Name), std::invalid_argument);
  BOOST_CHECK_THROW(container.get(id2Key1Name), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(Iterator)
{
  auto pibImpl = make_shared<PibMemory>();
  KeyContainer container(id1, pibImpl);
  container.add(id1Key1, id1Key1Name);
  container.add(id1Key2, id1Key2Name);

  const std::set<Name> keyNames{id1Key1Name, id1Key2Name};

  KeyContainer::const_iterator it = container.begin();
  auto testIt = keyNames.begin();
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  it++;
  testIt++;
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  ++it;
  testIt++;
  BOOST_CHECK(it == container.end());

  // test range-based for
  int count = 0;
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
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
