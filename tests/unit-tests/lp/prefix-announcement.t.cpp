/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
 *
 * @author Teng Liang <philoliang@email.arizona.edu>
 */

#include "lp/prefix-announcement.hpp"
#include "security/signature-sha256-with-rsa.hpp"

#include "boost-test.hpp"
#include "make-interest-data.hpp"

namespace ndn {
namespace lp {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Lp)
BOOST_AUTO_TEST_SUITE(TestPrefixAnnouncement)

BOOST_AUTO_TEST_CASE(SetData)
{
  Name name1("/ndn/pa");
  Name name2("/self-learning/ndn/pa");
  Name name3("/self-learning/ndn/pa");
  name3.appendVersion();

  PrefixAnnouncement pa;
  BOOST_CHECK_THROW(pa.setData(nullptr), PrefixAnnouncement::Error);
  BOOST_CHECK_THROW(pa.setData(makeData(name1)), PrefixAnnouncement::Error);
  BOOST_CHECK_THROW(pa.setData(makeData(name2)), PrefixAnnouncement::Error);
  BOOST_CHECK_NO_THROW(pa.setData(makeData(name3)));
  BOOST_CHECK_EQUAL(pa.getAnnouncedName(), "/ndn/pa");

  shared_ptr<Data> data1 = makeData(name3);
  shared_ptr<Data> data2 = makeData(name3);
  static const uint8_t someData[] = "someData";
  data1->setContent(someData, sizeof(someData));
  data2->setFreshnessPeriod(10_s);
  BOOST_CHECK_THROW(pa.setData(data1), PrefixAnnouncement::Error);
  BOOST_CHECK_THROW(pa.setData(data2), PrefixAnnouncement::Error);
}

BOOST_AUTO_TEST_CASE(GetAnnouncedName)
{
  PrefixAnnouncement pa1;
  BOOST_CHECK_THROW(pa1.getAnnouncedName(), PrefixAnnouncement::Error);

  Name name("/self-learning/edu/ua/news");
  name.appendVersion();

  PrefixAnnouncement pa2(makeData(name));

  BOOST_CHECK_EQUAL(pa2.getAnnouncedName(), "/edu/ua/news");
}

BOOST_AUTO_TEST_SUITE_END() // TestPrefixAnnouncement
BOOST_AUTO_TEST_SUITE_END() // Lp

} // namespace tests
} // namespace lp
} // namespace ndn
