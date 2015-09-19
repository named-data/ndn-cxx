/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "util/backports.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestBackports)

BOOST_AUTO_TEST_CASE(MakeUnique)
{
  std::unique_ptr<int> v0 = make_unique<int>();
  std::unique_ptr<int> v1 = make_unique<int>(1728);
  BOOST_CHECK_EQUAL(*v0, 0);
  BOOST_CHECK_EQUAL(*v1, 1728);

  auto str = make_unique<std::string>("meow");
  BOOST_CHECK_EQUAL(*str, "meow");

  class Movable
  {
  public:
    Movable()
      : value(42)
    {
    }

    Movable(const Movable&) = delete;
    Movable(Movable&&) = default;

  public:
    int value;
  };

  class Container
  {
  public:
    explicit
    Container(Movable m)
      : m(std::move(m))
    {
    }

  public:
    Movable m;
  };

  Movable m;
  auto ptr = make_unique<Container>(std::move(m));
  BOOST_CHECK_EQUAL(ptr->m.value, 42);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
