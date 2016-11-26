/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "tag-host.hpp"

#include "boost-test.hpp"
#include "interest.hpp"
#include "data.hpp"

#include <boost/mpl/vector.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestTagHost)

class TestTag : public Tag
{
public:
  static constexpr size_t
  getTypeId()
  {
    return 1;
  }
};

class TestTag2 : public Tag
{
public:
  static constexpr size_t
  getTypeId()
  {
    return 2;
  }
};

typedef boost::mpl::vector<TagHost, Interest, Data> Fixtures;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Basic, T, Fixtures, T)
{
  BOOST_CHECK(this->template getTag<TestTag>() == nullptr);
  BOOST_CHECK(this->template getTag<TestTag2>() == nullptr);

  this->setTag(make_shared<TestTag>());

  BOOST_CHECK(this->template getTag<TestTag>() != nullptr);
  BOOST_CHECK(this->template getTag<TestTag2>() == nullptr);

  this->setTag(make_shared<TestTag2>());

  BOOST_CHECK(this->template getTag<TestTag>() != nullptr);
  BOOST_CHECK(this->template getTag<TestTag2>() != nullptr);

  this->template removeTag<TestTag2>();

  BOOST_CHECK(this->template getTag<TestTag>() != nullptr);
  BOOST_CHECK(this->template getTag<TestTag2>() == nullptr);

  this->template removeTag<TestTag>();

  BOOST_CHECK(this->template getTag<TestTag>() == nullptr);
  BOOST_CHECK(this->template getTag<TestTag2>() == nullptr);
}

BOOST_AUTO_TEST_SUITE_END() // TestTagHost

} // namespace tests
} // namespace ndn
