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

#include "delegation.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestDelegation)

BOOST_AUTO_TEST_CASE(Compare)
{
  BOOST_CHECK_EQUAL((Delegation{1, "/A"}), (Delegation{1, "/A"}));
  BOOST_CHECK_LE((Delegation{1, "/A"}), (Delegation{1, "/A"}));
  BOOST_CHECK_GE((Delegation{1, "/A"}), (Delegation{1, "/A"}));

  BOOST_CHECK_NE((Delegation{1, "/A"}), (Delegation{2, "/A"}));
  BOOST_CHECK_NE((Delegation{1, "/A"}), (Delegation{1, "/B"}));

  BOOST_CHECK_LT((Delegation{1, "/A"}), (Delegation{1, "/B"}));
  BOOST_CHECK_LE((Delegation{1, "/A"}), (Delegation{1, "/B"}));
  BOOST_CHECK_LT((Delegation{1, "/B"}), (Delegation{2, "/A"}));
  BOOST_CHECK_LE((Delegation{1, "/B"}), (Delegation{2, "/A"}));
  BOOST_CHECK_LT((Delegation{1, "/A"}), (Delegation{2, "/A"}));
  BOOST_CHECK_LE((Delegation{1, "/A"}), (Delegation{2, "/A"}));

  BOOST_CHECK_GT((Delegation{1, "/B"}), (Delegation{1, "/A"}));
  BOOST_CHECK_GE((Delegation{1, "/B"}), (Delegation{1, "/A"}));
  BOOST_CHECK_GT((Delegation{2, "/A"}), (Delegation{1, "/B"}));
  BOOST_CHECK_GE((Delegation{2, "/A"}), (Delegation{1, "/B"}));
  BOOST_CHECK_GT((Delegation{2, "/A"}), (Delegation{1, "/A"}));
  BOOST_CHECK_GE((Delegation{2, "/A"}), (Delegation{1, "/A"}));
}

BOOST_AUTO_TEST_CASE(Print)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(Delegation{1, "/B"}), "/B(1)");
}

BOOST_AUTO_TEST_SUITE_END() // TestDelegation

} // namespace tests
} // namespace ndn
