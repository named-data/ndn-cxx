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

#include "ndn-cxx/ims/in-memory-storage.hpp"
#include "ndn-cxx/ims/in-memory-storage-fifo.hpp"
#include "ndn-cxx/ims/in-memory-storage-lfu.hpp"
#include "ndn-cxx/ims/in-memory-storage-lru.hpp"
#include "ndn-cxx/ims/in-memory-storage-persistent.hpp"
#include "ndn-cxx/security/signature-sha256-with-rsa.hpp"
#include "ndn-cxx/util/sha256.hpp"

#include "tests/boost-test.hpp"
#include "tests/make-interest-data.hpp"
#include "tests/unit/unit-test-time-fixture.hpp"

#include <boost/mpl/vector.hpp>

namespace ndn {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Ims)
BOOST_AUTO_TEST_SUITE(TestInMemoryStorage)

using InMemoryStorages = boost::mpl::vector<InMemoryStoragePersistent,
                                            InMemoryStorageFifo,
                                            InMemoryStorageLfu,
                                            InMemoryStorageLru>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Insertion, T, InMemoryStorages)
{
  T ims;

  ims.insert(*makeData("/a"));
  ims.insert(*makeData("/b"));
  ims.insert(*makeData("/c"));
  ims.insert(*makeData("/d"));

  BOOST_CHECK_EQUAL(ims.size(), 4);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Insertion2, T, InMemoryStorages)
{
  T ims;

  Name name("/a");

  uint32_t content1 = 1;
  shared_ptr<Data> data1 = makeData(name);
  data1->setFreshnessPeriod(99999_ms);
  data1->setContent(reinterpret_cast<const uint8_t*>(&content1), sizeof(content1));
  signData(data1);
  ims.insert(*data1);

  uint32_t content2 = 2;
  shared_ptr<Data> data2 = makeData(name);
  data2->setFreshnessPeriod(99999_ms);
  data2->setContent(reinterpret_cast<const uint8_t*>(&content2), sizeof(content2));
  signData(data2);
  ims.insert(*data2);

  BOOST_CHECK_EQUAL(ims.size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DuplicateInsertion, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data0 = makeData("/insert/smth");
  ims.insert(*data0);

  shared_ptr<Data> data = makeData("/insert/duplicate");
  ims.insert(*data);

  ims.insert(*data);
  BOOST_CHECK_EQUAL(ims.size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DuplicateInsertion2, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/insert/duplicate");
  ims.insert(*data);

  ims.insert(*data);
  BOOST_CHECK_EQUAL(ims.size(), 1);

  shared_ptr<Data> data2 = makeData("/insert/original");
  ims.insert(*data2);
  BOOST_CHECK_EQUAL(ims.size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndFind, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/and/find");

  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  shared_ptr<Interest> interest = makeInterest(name);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK(found != nullptr);
  BOOST_CHECK_EQUAL(data->getName(), found->getName());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndNotFind, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/and/find");
  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  Name name2("/not/find");
  shared_ptr<Interest> interest = makeInterest(name2);

  shared_ptr<const Data> found = ims.find(*interest);

  BOOST_CHECK_EQUAL(found.get(), static_cast<const Data*>(0));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndFindByName, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/and/find");

  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  shared_ptr<const Data> found = ims.find(name);
  BOOST_CHECK(found != nullptr);
  BOOST_CHECK_EQUAL(data->getName(), found->getName());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndFindByFullName, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/and/find");

  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  shared_ptr<const Data> found = ims.find(data->getFullName());
  BOOST_CHECK(found != nullptr);
  BOOST_CHECK_EQUAL(data->getFullName(), found->getFullName());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndNotFindByName, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/and/find");
  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  Name name2("/not/find");

  shared_ptr<const Data> found = ims.find(name2);
  BOOST_CHECK(found == nullptr);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndNotFindByFullName, T, InMemoryStorages)
{
  T ims;

  Name name("/a");
  uint32_t content1 = 1;
  shared_ptr<Data> data1 = makeData(name);
  data1->setContent(reinterpret_cast<const uint8_t*>(&content1), sizeof(content1));
  signData(data1);
  ims.insert(*data1);

  uint32_t content2 = 2;
  shared_ptr<Data> data2 = makeData(name);
  data2->setContent(reinterpret_cast<const uint8_t*>(&content2), sizeof(content2));
  signData(data2);

  shared_ptr<const Data> found = ims.find(data2->getFullName());
  BOOST_CHECK(found == nullptr);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndEraseByName, T, InMemoryStorages)
{
  T ims;

  Name name("/insertandremovebyname");

  uint32_t content1 = 1;
  shared_ptr<Data> data1 = makeData(name);
  data1->setFreshnessPeriod(99999_ms);
  data1->setContent(reinterpret_cast<const uint8_t*>(&content1), sizeof(content1));
  signData(data1);
  ims.insert(*data1);

  uint32_t content2 = 2;
  shared_ptr<Data> data2 = makeData(name);
  data2->setFreshnessPeriod(99999_ms);
  data2->setContent(reinterpret_cast<const uint8_t*>(&content2), sizeof(content2));
  signData(data2);
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/insertandremovebyname/1");
  ims.insert(*data3);

  shared_ptr<Data> data4 = makeData("/insertandremovebyname/2");
  ims.insert(*data4);

  BOOST_CHECK_EQUAL(ims.size(), 4);

  ims.erase(data1->getFullName(), false);
  BOOST_CHECK_EQUAL(ims.size(), 3);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndEraseByPrefix, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/b");
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/c");
  ims.insert(*data3);

  shared_ptr<Data> data4 = makeData("/d");
  ims.insert(*data4);

  shared_ptr<Data> data5 = makeData("/c/c/1/2/3/4/5/6");
  ims.insert(*data5);

  shared_ptr<Data> data6 = makeData("/c/c/1/2/3");
  ims.insert(*data6);

  shared_ptr<Data> data7 = makeData("/c/c/1");
  ims.insert(*data7);

  BOOST_CHECK_EQUAL(ims.size(), 7);

  Name name("/c");
  ims.erase(name);
  BOOST_CHECK_EQUAL(ims.size(), 3);
  BOOST_CHECK_EQUAL(ims.getCapacity(), 16);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DigestCalculation, T, InMemoryStorages)
{
  shared_ptr<Data> data = makeData("/digest/compute");

  ConstBufferPtr digest1 = util::Sha256::computeDigest(data->wireEncode().wire(), data->wireEncode().size());
  BOOST_CHECK_EQUAL(digest1->size(), 32);

  InMemoryStorageEntry entry;
  entry.setData(*data);

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->begin(), digest1->end(),
                                entry.getFullName()[-1].value_begin(),
                                entry.getFullName()[-1].value_end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Iterator, T, InMemoryStorages)
{
  T ims;

  BOOST_CONCEPT_ASSERT((boost::InputIterator<InMemoryStorage::const_iterator>));

  for (int i = 0; i < 10; i++) {
    std::ostringstream convert;
    convert << i;
    Name name("/" + convert.str());
    shared_ptr<Data> data = makeData(name);
    ims.insert(*data);
  }

  InMemoryStorage::const_iterator it = ims.begin();
  InMemoryStorage::const_iterator tmp1 = it;
  BOOST_REQUIRE(tmp1 == it);
  InMemoryStorage::const_iterator tmp2 = tmp1++;
  BOOST_REQUIRE(tmp2 != tmp1);
  tmp2 = ++tmp1;
  BOOST_REQUIRE(tmp2 == tmp1);

  int i = 0;
  for (; it != ims.end(); it++) {
    std::ostringstream convert;
    convert << i;
    Name name("/" + convert.str());
    BOOST_CHECK_EQUAL(it->getName(), name);
    BOOST_CHECK_EQUAL((*it).getName(), name);
    i++;
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertCanonical, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/b");
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/c");
  ims.insert(*data3);

  shared_ptr<Data> data4 = makeData("/d");
  ims.insert(*data4);

  shared_ptr<Data> data5 = makeData("/c/c/1/2/3/4/5/6");
  ims.insert(*data5);

  shared_ptr<Data> data6 = makeData("/c/c/1/2/3");
  ims.insert(*data6);

  shared_ptr<Data> data7 = makeData("/c/c/1");
  ims.insert(*data7);

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EraseCanonical, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/b");
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/c");
  ims.insert(*data3);

  shared_ptr<Data> data4 = makeData("/d");
  ims.insert(*data4);

  shared_ptr<Data> data5 = makeData("/c/c/1/2/3/4/5/6");
  ims.insert(*data5);

  shared_ptr<Data> data6 = makeData("/c/c/1/2/3");
  ims.insert(*data6);

  shared_ptr<Data> data7 = makeData("/c/c/1");
  ims.insert(*data7);

  ConstBufferPtr digest1 = util::Sha256::computeDigest(data->wireEncode().wire(), data->wireEncode().size());

  Name name("/a");
  ims.erase(name);
  BOOST_CHECK_EQUAL(ims.size(), 6);
}

using InMemoryStoragesLimited = boost::mpl::vector<InMemoryStorageFifo,
                                                   InMemoryStorageLfu,
                                                   InMemoryStorageLru>;

BOOST_AUTO_TEST_CASE_TEMPLATE(SetCapacity, T, InMemoryStoragesLimited)
{
  T ims;

  ims.setCapacity(18);
  for (int i = 1; i < 19; ++i) {
    ims.insert(*makeData(to_string(i)));
  }
  BOOST_CHECK_EQUAL(ims.size(), 18);

  ims.setCapacity(16);
  BOOST_CHECK_EQUAL(ims.size(), 16);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(GetLimit, T, InMemoryStoragesLimited)
{
  T ims(10000);
  BOOST_CHECK_EQUAL(ims.getLimit(), 10000);

  T ims2(4);
  BOOST_CHECK_EQUAL(ims2.getLimit(), 4);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndDouble, T, InMemoryStoragesLimited)
{
  T ims(40);
  size_t initialCapacity = ims.getCapacity();

  for (size_t i = 0; i < initialCapacity + 1; i++) {
    shared_ptr<Data> data = makeData(to_string(i));
    data->setFreshnessPeriod(5000_ms);
    signData(data);
    ims.insert(*data);
  }

  BOOST_CHECK_EQUAL(ims.size(), initialCapacity + 1);
  BOOST_CHECK_EQUAL(ims.getCapacity(), initialCapacity * 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndEvict, T, InMemoryStoragesLimited)
{
  T ims(2);

  Name name("/insert/1");
  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  Name name2("/insert/2");
  shared_ptr<Data> data2 = makeData(name2);
  ims.insert(*data2);

  Name name3("/insert/3");
  shared_ptr<Data> data3 = makeData(name3);
  ims.insert(*data3);

  BOOST_CHECK_EQUAL(ims.size(), 2);

  shared_ptr<Interest> interest = makeInterest(name);
  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK(found == nullptr);
}

// Find function is implemented at the base case, so it's sufficient to test for one derived class.
class FindFixture : public tests::UnitTestTimeFixture
{
protected:
  FindFixture()
    : m_ims(io)
  {
  }

  Name
  insert(uint32_t id, const Name& name,
         const std::function<void(Data&)>& modifyData = nullptr,
         const time::milliseconds& freshWindow = InMemoryStorage::INFINITE_WINDOW)
  {
    auto data = makeData(name);
    data->setContent(reinterpret_cast<const uint8_t*>(&id), sizeof(id));

    if (modifyData != nullptr) {
      modifyData(*data);
    }

    data->wireEncode();
    m_ims.insert(*data, freshWindow);

    return data->getFullName();
  }

  Interest&
  startInterest(const Name& name)
  {
    m_interest = makeInterest(name, false);
    return *m_interest;
  }

  uint32_t
  find()
  {
    shared_ptr<const Data> found = m_ims.find(*m_interest);
    if (found == 0) {
      return 0;
    }
    const Block& content = found->getContent();
    if (content.value_size() != sizeof(uint32_t)) {
      return 0;
    }
    uint32_t id = 0;
    std::memcpy(&id, content.value(), sizeof(id));
    return id;
  }

protected:
  InMemoryStoragePersistent m_ims;
  shared_ptr<Interest> m_interest;
};

BOOST_FIXTURE_TEST_SUITE(Find, FindFixture)

BOOST_AUTO_TEST_CASE(ExactName)
{
  insert(1, "/");
  insert(2, "/A");
  insert(3, "/A/B");
  insert(4, "/A/C");
  insert(5, "/D");

  startInterest("/A");
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(ExactName_CanBePrefix)
{
  insert(1, "/");
  insert(2, "/A");
  insert(3, "/A/B");
  insert(4, "/A/C");
  insert(5, "/D");

  startInterest("/A")
    .setCanBePrefix(true);
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(FullName)
{
  Name n1 = insert(1, "/A");
  Name n2 = insert(2, "/A");

  startInterest(n1);
  BOOST_CHECK_EQUAL(find(), 1);

  startInterest(n2);
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(FullName_EmptyDataName)
{
  Name n1 = insert(1, "/");
  Name n2 = insert(2, "/");

  startInterest(n1);
  BOOST_CHECK_EQUAL(find(), 1);

  startInterest(n2);
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(PrefixName)
{
  insert(1, "/A");
  insert(2, "/B/p/1");
  insert(3, "/B/p/2");
  insert(4, "/B/q/1");
  insert(5, "/B/q/2");
  insert(6, "/C");

  startInterest("/B")
    .setCanBePrefix(true);
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(PrefixName_NoCanBePrefix)
{
  insert(1, "/B/p/1");

  startInterest("/B");
  BOOST_CHECK_EQUAL(find(), 0);
}

BOOST_AUTO_TEST_CASE(MustBeFresh)
{
  insert(1, "/A/1"); // omitted FreshnessPeriod means FreshnessPeriod = 0 ms
  insert(2, "/A/2", [] (Data& data) { data.setFreshnessPeriod(0_s); });
  insert(3, "/A/3", [] (Data& data) { data.setFreshnessPeriod(1_s); }, 1_s);
  insert(4, "/A/4", [] (Data& data) { data.setFreshnessPeriod(1_h); }, 1_h);

  // lookup at exact same moment as insertion is not tested because this won't happen in reality

  advanceClocks(500_ms); // @500ms
  startInterest("/A")
    .setCanBePrefix(true)
    .setMustBeFresh(true);
  BOOST_CHECK_EQUAL(find(), 3);

  advanceClocks(1500_ms); // @2s
  startInterest("/A")
    .setCanBePrefix(true)
    .setMustBeFresh(true);
  BOOST_CHECK_EQUAL(find(), 4);

  advanceClocks(3500_s); // @3502s
  startInterest("/A")
    .setCanBePrefix(true)
    .setMustBeFresh(true);
  BOOST_CHECK_EQUAL(find(), 4);

  advanceClocks(3500_s); // @7002s
  startInterest("/A")
    .setCanBePrefix(true)
    .setMustBeFresh(true);
  BOOST_CHECK_EQUAL(find(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // Find
BOOST_AUTO_TEST_SUITE_END() // TestInMemoryStorage
BOOST_AUTO_TEST_SUITE_END() // Ims

} // namespace tests
} // namespace ndn
