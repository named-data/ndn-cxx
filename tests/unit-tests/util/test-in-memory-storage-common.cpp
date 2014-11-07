/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "util/in-memory-storage-persistent.hpp"
#include "util/in-memory-storage-fifo.hpp"
#include "util/in-memory-storage-lfu.hpp"
#include "util/in-memory-storage-lru.hpp"
#include "security/key-chain.hpp"

#include "boost-test.hpp"
#include "../test-make-interest-data.hpp"

#include <boost/mpl/list.hpp>

namespace ndn {
namespace util {

BOOST_AUTO_TEST_SUITE(UtilInMemoryStorage)

BOOST_AUTO_TEST_SUITE(Common)

typedef boost::mpl::list<InMemoryStoragePersistent, InMemoryStorageFifo, InMemoryStorageLfu,
                         InMemoryStorageLru> InMemoryStorages;

BOOST_AUTO_TEST_CASE_TEMPLATE(Insertion, T, InMemoryStorages)
{
  T ims;

  ims.insert(*makeData("/insertion"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Insertion2, T, InMemoryStorages)
{
  T ims;

  ims.insert(*makeData("/a"));
  ims.insert(*makeData("/b"));
  ims.insert(*makeData("/c"));
  ims.insert(*makeData("/d"));

  BOOST_CHECK_EQUAL(ims.size(), 4);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Insertion3, T, InMemoryStorages)
{
  T ims;

  Name name("/a");

  uint32_t content1 = 1;
  shared_ptr<Data> data1 = makeData(name);
  data1->setFreshnessPeriod(time::milliseconds(99999));
  data1->setContent(reinterpret_cast<const uint8_t*>(&content1), sizeof(content1));
  signData(data1);
  ims.insert(*data1);

  uint32_t content2 = 2;
  shared_ptr<Data> data2 = makeData(name);
  data2->setFreshnessPeriod(time::milliseconds(99999));
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

  BOOST_CHECK(static_cast<bool>(found));
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

  BOOST_CHECK(static_cast<bool>(found));
  BOOST_CHECK_EQUAL(data->getName(), found->getName());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndFindByFullName, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/and/find");

  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  shared_ptr<const Data> found = ims.find(data->getFullName());

  BOOST_CHECK(static_cast<bool>(found));
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

  BOOST_CHECK(!static_cast<bool>(found));
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

  BOOST_CHECK(!static_cast<bool>(found));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InsertAndEraseByName, T, InMemoryStorages)
{
  T ims;

  Name name("/insertandremovebyname");

  uint32_t content1 = 1;
  shared_ptr<Data> data1 = makeData(name);
  data1->setFreshnessPeriod(time::milliseconds(99999));
  data1->setContent(reinterpret_cast<const uint8_t*>(&content1), sizeof(content1));
  signData(data1);
  ims.insert(*data1);

  uint32_t content2 = 2;
  shared_ptr<Data> data2 = makeData(name);
  data2->setFreshnessPeriod(time::milliseconds(99999));
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
  BOOST_CHECK_EQUAL(ims.getCapacity(), 5);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DigestCalculation, T, InMemoryStorages)
{
  shared_ptr<Data> data = makeData("/digest/compute");

  ndn::ConstBufferPtr digest1 = ndn::crypto::sha256(data->wireEncode().wire(),
                                                    data->wireEncode().size());
  BOOST_CHECK_EQUAL(digest1->size(), 32);

  InMemoryStorageEntry* entry = new InMemoryStorageEntry();
  entry->setData(*data);

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->begin(), digest1->end(),
                                entry->getFullName()[-1].value_begin(),
                                entry->getFullName()[-1].value_end());
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
  for (;it != ims.end(); it++) {
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

  ndn::ConstBufferPtr digest1 = ndn::crypto::sha256(data->wireEncode().wire(),
                                                    data->wireEncode().size());

  Name name("/a");
  ims.erase(name);
  BOOST_CHECK_EQUAL(ims.size(), 6);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ImplicitDigestSelector, T, InMemoryStorages)
{
  T ims;

  Name name("/digest/works");
  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/a");
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/z/z/z");
  ims.insert(*data3);

  ndn::ConstBufferPtr digest1 = ndn::crypto::sha256(data->wireEncode().wire(),
                                                    data->wireEncode().size());

  shared_ptr<Interest> interest = makeInterest("");
  interest->setName(Name(name).appendImplicitSha256Digest(digest1->buf(), digest1->size()));
  interest->setMinSuffixComponents(0);
  interest->setMaxSuffixComponents(0);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_REQUIRE(static_cast<bool>(found));
  BOOST_CHECK_EQUAL(found->getName(), name);

  shared_ptr<Interest> interest2 = makeInterest("");
  uint8_t digest2[32] = {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
  interest2->setName(Name(name).appendImplicitSha256Digest(digest2, 32));
  interest2->setMinSuffixComponents(0);
  interest2->setMaxSuffixComponents(0);

  shared_ptr<const Data> notfound = ims.find(*interest2);
  BOOST_CHECK(static_cast<bool>(found));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ChildSelector, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/b");
  ims.insert(*data2);

  shared_ptr<Data> data4 = makeData("/d");
  ims.insert(*data4);

  shared_ptr<Data> data5 = makeData("/c/c");
  ims.insert(*data5);

  shared_ptr<Data> data6 = makeData("/c/f");
  ims.insert(*data6);

  shared_ptr<Data> data7 = makeData("/c/n");
  ims.insert(*data7);

  shared_ptr<Interest> interest = makeInterest("/c");
  interest->setChildSelector(1);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK_EQUAL(found->getName(), "/c/n");

  shared_ptr<Interest> interest2 = makeInterest("/c");
  interest2->setChildSelector(0);

  shared_ptr<const Data> found2 = ims.find(*interest2);
  BOOST_CHECK_EQUAL(found2->getName(), "/c/c");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ChildSelector2, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a/b/1");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/a/b/2");
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/a/z/1");
  ims.insert(*data3);

  shared_ptr<Data> data4 = makeData("/a/z/2");
  ims.insert(*data4);

  shared_ptr<Interest> interest = makeInterest("/a");
  interest->setChildSelector(1);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK_EQUAL(found->getName(), "/a/z/1");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(PublisherKeySelector, T, InMemoryStorages)
{
  T ims;

  Name name("/insert/withkey");
  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  shared_ptr<Interest> interest = makeInterest(name);
  Name keyName("/somewhere/key");

  ndn::KeyLocator locator(keyName);
  interest->setPublisherPublicKeyLocator(locator);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK(!static_cast<bool>(found));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(PublisherKeySelector2, T, InMemoryStorages)
{
  T ims;
  Name name("/insert/withkey");
  shared_ptr<Data> data = makeData(name);
  ims.insert(*data);

  Name name2("/insert/withkey2");
  shared_ptr<Data> data2 = make_shared<Data>(name2);

  Name keyName("/somewhere/key");
  const ndn::KeyLocator locator(keyName);

  ndn::SignatureSha256WithRsa fakeSignature;
  fakeSignature.setValue(ndn::dataBlock(tlv::SignatureValue,
                                        reinterpret_cast<const uint8_t*>(0), 0));

  fakeSignature.setKeyLocator(locator);
  data2->setSignature(fakeSignature);
  data2->wireEncode();

  ims.insert(*data2);

  shared_ptr<Interest> interest = makeInterest(name2);
  interest->setPublisherPublicKeyLocator(locator);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK(static_cast<bool>(found));
  BOOST_CHECK_EQUAL(found->getName(), data2->getName());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(MinMaxComponentsSelector, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/b");
  ims.insert(*data2);

  shared_ptr<Data> data4 = makeData("/d");
  ims.insert(*data4);

  shared_ptr<Data> data5 = makeData("/c/c/1/2/3/4/5/6");
  ims.insert(*data5);

  shared_ptr<Data> data6 = makeData("/c/c/6/7/8/9");
  ims.insert(*data6);

  shared_ptr<Data> data7 = makeData("/c/c/1/2/3");
  ims.insert(*data7);

  shared_ptr<Data> data8 = makeData("/c/c/1");
  ims.insert(*data8);

  shared_ptr<Interest> interest = makeInterest("/c/c");
  interest->setMinSuffixComponents(3);
  interest->setChildSelector(0);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK_EQUAL(found->getName(), "/c/c/1/2/3");

  shared_ptr<Interest> interest2 = makeInterest("/c/c");
  interest2->setMinSuffixComponents(4);
  interest2->setChildSelector(1);

  shared_ptr<const Data> found2 = ims.find(*interest2);
  BOOST_CHECK_EQUAL(found2->getName(), "/c/c/6/7/8/9");

  shared_ptr<Interest> interest3 = makeInterest("/c/c");
  interest3->setMaxSuffixComponents(2);
  interest3->setChildSelector(1);

  shared_ptr<const Data> found3 = ims.find(*interest3);
  BOOST_CHECK_EQUAL(found3->getName(), "/c/c/1");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExcludeSelector, T, InMemoryStorages)
{
  T ims;

  shared_ptr<Data> data = makeData("/a");
  ims.insert(*data);

  shared_ptr<Data> data2 = makeData("/b");
  ims.insert(*data2);

  shared_ptr<Data> data3 = makeData("/c/a");
  ims.insert(*data3);

  shared_ptr<Data> data4 = makeData("/d");
  ims.insert(*data4);

  shared_ptr<Data> data5 = makeData("/c/c");
  ims.insert(*data5);

  shared_ptr<Data> data6 = makeData("/c/f");
  ims.insert(*data6);

  shared_ptr<Data> data7 = makeData("/c/n");
  ims.insert(*data7);

  shared_ptr<Interest> interest = makeInterest("/c");
  interest->setChildSelector(1);
  Exclude e;
  e.excludeOne (Name::Component("n"));
  interest->setExclude(e);

  shared_ptr<const Data> found = ims.find(*interest);
  BOOST_CHECK_EQUAL(found->getName(), "/c/f");

  shared_ptr<Interest> interest2 = makeInterest("/c");
  interest2->setChildSelector(0);

  Exclude e2;
  e2.excludeOne (Name::Component("a"));
  interest2->setExclude(e2);

  shared_ptr<const Data> found2 = ims.find(*interest2);
  BOOST_CHECK_EQUAL(found2->getName(), "/c/c");

  shared_ptr<Interest> interest3 = makeInterest("/c");
  interest3->setChildSelector(0);

  Exclude e3;
  e3.excludeOne (Name::Component("c"));
  interest3->setExclude(e3);

  shared_ptr<const Data> found3 = ims.find(*interest3);
  BOOST_CHECK_EQUAL(found3->getName(), "/c/a");
}

typedef boost::mpl::list<InMemoryStorageFifo, InMemoryStorageLfu, InMemoryStorageLru>
                         InMemoryStoragesLimited;

BOOST_AUTO_TEST_CASE_TEMPLATE(setCapacity, T, InMemoryStoragesLimited)
{
  T ims;

  ims.setCapacity(3);
  ims.insert(*makeData("/1"));
  ims.insert(*makeData("/2"));
  ims.insert(*makeData("/3"));
  BOOST_CHECK_EQUAL(ims.size(), 3);

  ims.setCapacity(2);
  BOOST_CHECK_EQUAL(ims.size(), 2);
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

  for (int i = 0; i < 11; i++) {
    std::ostringstream convert;
    convert << i;
    Name name("/" + convert.str());
    shared_ptr<Data> data = makeData(name);
    data->setFreshnessPeriod(time::milliseconds(5000));
    signData(data);
    ims.insert(*data);
  }

  BOOST_CHECK_EQUAL(ims.size(), 11);

  BOOST_CHECK_EQUAL(ims.getCapacity(), 20);
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
  BOOST_CHECK(!static_cast<bool>(found));
}

///as Find function is implemented at the base case, therefore testing for one derived class is
///sufficient for all
class FindFixture
{
protected:
  void
  insert(uint32_t id, const Name& name)
  {
    shared_ptr<Data> data = makeData(name);
    data->setFreshnessPeriod(time::milliseconds(99999));
    data->setContent(reinterpret_cast<const uint8_t*>(&id), sizeof(id));
    signData(data);

    m_ims.insert(*data);
  }

  Interest&
  startInterest(const Name& name)
  {
    m_interest = makeInterest(name);
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
    return *reinterpret_cast<const uint32_t*>(content.value());
  }

protected:
  InMemoryStoragePersistent m_ims;
  shared_ptr<Interest> m_interest;
};

BOOST_FIXTURE_TEST_SUITE(Find, FindFixture)

BOOST_AUTO_TEST_CASE(EmptyDataName)
{
  insert(1, "ndn:/");

  startInterest("ndn:/");
  BOOST_CHECK_EQUAL(find(), 1);
}

BOOST_AUTO_TEST_CASE(EmptyInterestName)
{
  insert(1, "ndn:/A");

  startInterest("ndn:/");
  BOOST_CHECK_EQUAL(find(), 1);
}

BOOST_AUTO_TEST_CASE(Leftmost)
{
  insert(1, "ndn:/A");
  insert(2, "ndn:/B/p/1");
  insert(3, "ndn:/B/p/2");
  insert(4, "ndn:/B/q/1");
  insert(5, "ndn:/B/q/2");
  insert(6, "ndn:/C");

  startInterest("ndn:/B");
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(Rightmost)
{
  insert(1, "ndn:/A");
  insert(2, "ndn:/B/p/1");
  insert(3, "ndn:/B/p/2");
  insert(4, "ndn:/B/q/1");
  insert(5, "ndn:/B/q/2");
  insert(6, "ndn:/C");

  startInterest("ndn:/B")
    .setChildSelector(1);
  BOOST_CHECK_EQUAL(find(), 4);
}

/// @todo Expected failures, needs to be fixed as part of Issue #2118
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(Leftmost_ExactName1, 1)
BOOST_AUTO_TEST_CASE(Leftmost_ExactName1)
{
  insert(1, "ndn:/");
  insert(2, "ndn:/A/B");
  insert(3, "ndn:/A/C");
  insert(4, "ndn:/A");
  insert(5, "ndn:/D");

  // Intuitively you would think Data 4 should be between Data 1 and 2,
  // but Data 4 has full Name ndn:/A/<32-octet hash>.
  startInterest("ndn:/A");
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(Leftmost_ExactName33)
{
  insert(1, "ndn:/");
  insert(2, "ndn:/A");
  insert(3, "ndn:/A/BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"); // 33 'B's
  insert(4, "ndn:/A/CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"); // 33 'C's
  insert(5, "ndn:/D");

  // Data 2 is returned, because <32-octet hash> is less than Data 3.
  startInterest("ndn:/A");
  BOOST_CHECK_EQUAL(find(), 2);
}

/// @todo Expected failures, needs to be fixed as part of Issue #2118
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MinSuffixComponents, 2)
BOOST_AUTO_TEST_CASE(MinSuffixComponents)
{
  insert(1, "ndn:/A/1/2/3/4");
  insert(2, "ndn:/B/1/2/3");
  insert(3, "ndn:/C/1/2");
  insert(4, "ndn:/D/1");
  insert(5, "ndn:/E");
  insert(6, "ndn:/");

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(0);
  BOOST_CHECK_EQUAL(find(), 6);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(find(), 6);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(2);
  BOOST_CHECK_EQUAL(find(), 5);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(3);
  BOOST_CHECK_EQUAL(find(), 4);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(4);
  BOOST_CHECK_EQUAL(find(), 3);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(5);
  BOOST_CHECK_EQUAL(find(), 2);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(6);
  BOOST_CHECK_EQUAL(find(), 1);

  startInterest("ndn:/")
    .setChildSelector(1)
    .setMinSuffixComponents(7);
  BOOST_CHECK_EQUAL(find(), 0);
}

/// @todo Expected failures, needs to be fixed as part of Issue #2118
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MaxSuffixComponents, 5)
BOOST_AUTO_TEST_CASE(MaxSuffixComponents)
{
  insert(1, "ndn:/");
  insert(2, "ndn:/A");
  insert(3, "ndn:/A/B");
  insert(4, "ndn:/A/B/C");
  insert(5, "ndn:/A/B/C/D");
  insert(6, "ndn:/A/B/C/D/E");
  // Order is 6,5,4,3,2,1, because <32-octet hash> is greater than a 1-octet component.

  startInterest("ndn:/")
    .setMaxSuffixComponents(0);
  BOOST_CHECK_EQUAL(find(), 0);

  startInterest("ndn:/")
    .setMaxSuffixComponents(1);
  BOOST_CHECK_EQUAL(find(), 1);

  startInterest("ndn:/")
    .setMaxSuffixComponents(2);
  BOOST_CHECK_EQUAL(find(), 2);

  startInterest("ndn:/")
    .setMaxSuffixComponents(3);
  BOOST_CHECK_EQUAL(find(), 3);

  startInterest("ndn:/")
    .setMaxSuffixComponents(4);
  BOOST_CHECK_EQUAL(find(), 4);

  startInterest("ndn:/")
    .setMaxSuffixComponents(5);
  BOOST_CHECK_EQUAL(find(), 5);

  startInterest("ndn:/")
    .setMaxSuffixComponents(6);
  BOOST_CHECK_EQUAL(find(), 6);
}

BOOST_AUTO_TEST_CASE(DigestOrder)
{
  insert(1, "ndn:/A");
  insert(2, "ndn:/A");
  // We don't know which comes first, but there must be some order

  startInterest("ndn:/A")
    .setChildSelector(0);
  uint32_t leftmost = find();

  startInterest("ndn:/A")
    .setChildSelector(1);
  uint32_t rightmost = find();

  BOOST_CHECK_NE(leftmost, rightmost);
}

/// @todo Expected failures, needs to be fixed as part of Issue #2118
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(DigestExclude, 1)
BOOST_AUTO_TEST_CASE(DigestExclude)
{
  insert(1, "ndn:/A/B");
  insert(2, "ndn:/A");
  insert(3, "ndn:/A/CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"); // 33 'C's

  startInterest("ndn:/A")
    .setExclude(Exclude().excludeBefore(name::Component(reinterpret_cast<const uint8_t*>(
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
        "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"), 31))); // 31 0xFF's
  BOOST_CHECK_EQUAL(find(), 2);

  startInterest("ndn:/A")
    .setChildSelector(1)
    .setExclude(Exclude().excludeAfter(name::Component(reinterpret_cast<const uint8_t*>(
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00"), 33))); // 33 0x00's
  BOOST_CHECK_EQUAL(find(), 2);
}

BOOST_AUTO_TEST_CASE(ExactName32)
{
  insert(1, "ndn:/A/BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"); // 32 'B's
  insert(2, "ndn:/A/CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"); // 32 'C's

  startInterest("ndn:/A/BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
  BOOST_CHECK_EQUAL(find(), 1);
}

/// @todo Expected failures, needs to be fixed as part of Issue #2118
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MinSuffixComponents32, 2)
BOOST_AUTO_TEST_CASE(MinSuffixComponents32)
{
  insert(1, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/A/1/2/3/4"); // 32 'x's
  insert(2, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/B/1/2/3");
  insert(3, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/C/1/2");
  insert(4, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/D/1");
  insert(5, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/E");
  insert(6, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(0);
  BOOST_CHECK_EQUAL(find(), 6);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(find(), 6);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(2);
  BOOST_CHECK_EQUAL(find(), 5);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(3);
  BOOST_CHECK_EQUAL(find(), 4);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(4);
  BOOST_CHECK_EQUAL(find(), 3);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(5);
  BOOST_CHECK_EQUAL(find(), 2);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(6);
  BOOST_CHECK_EQUAL(find(), 1);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setChildSelector(1)
    .setMinSuffixComponents(7);
  BOOST_CHECK_EQUAL(find(), 0);
}

/// @todo Expected failures, needs to be fixed as part of Issue #2118
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MaxSuffixComponents32, 5)
BOOST_AUTO_TEST_CASE(MaxSuffixComponents32)
{
  insert(1, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/"); // 32 'x's
  insert(2, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/A");
  insert(3, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/A/B");
  insert(4, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/A/B/C");
  insert(5, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/A/B/C/D");
  insert(6, "ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/A/B/C/D/E");
  // Order is 6,5,4,3,2,1, because <32-octet hash> is greater than a 1-octet component.

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(0);
  BOOST_CHECK_EQUAL(find(), 0);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(1);
  BOOST_CHECK_EQUAL(find(), 1);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(2);
  BOOST_CHECK_EQUAL(find(), 2);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(3);
  BOOST_CHECK_EQUAL(find(), 3);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(4);
  BOOST_CHECK_EQUAL(find(), 4);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(5);
  BOOST_CHECK_EQUAL(find(), 5);

  startInterest("ndn:/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .setMaxSuffixComponents(6);
  BOOST_CHECK_EQUAL(find(), 6);
}

BOOST_AUTO_TEST_SUITE_END() // Find
BOOST_AUTO_TEST_SUITE_END() // Common
BOOST_AUTO_TEST_SUITE_END() // UtilInMemoryStorage

} // namespace util
} // namespace ndn
