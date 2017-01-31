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

#include "security/v2/validator-config/name-relation.hpp"

#include "boost-test.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(ValidatorConfig)
BOOST_AUTO_TEST_SUITE(TestNameRelation)

BOOST_AUTO_TEST_CASE(ToString)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(NameRelation::EQUAL), "equal");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(NameRelation::IS_PREFIX_OF),
                    "is-prefix-of");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(NameRelation::IS_STRICT_PREFIX_OF),
                    "is-strict-prefix-of");
}

BOOST_AUTO_TEST_CASE(FromString)
{
  BOOST_CHECK_EQUAL(getNameRelationFromString("equal"), NameRelation::EQUAL);
  BOOST_CHECK_EQUAL(getNameRelationFromString("is-prefix-of"), NameRelation::IS_PREFIX_OF);
  BOOST_CHECK_EQUAL(getNameRelationFromString("is-strict-prefix-of"), NameRelation::IS_STRICT_PREFIX_OF);
  BOOST_CHECK_THROW(getNameRelationFromString("unknown"), validator_config::Error);
}

BOOST_AUTO_TEST_CASE(CheckRelation)
{
  BOOST_CHECK(checkNameRelation(NameRelation::EQUAL, "/prefix", "/prefix"));
  BOOST_CHECK(!checkNameRelation(NameRelation::EQUAL, "/prefix", "/prefix/other"));

  BOOST_CHECK(checkNameRelation(NameRelation::IS_PREFIX_OF, "/prefix", "/prefix"));
  BOOST_CHECK(checkNameRelation(NameRelation::IS_PREFIX_OF, "/prefix", "/prefix/other"));
  BOOST_CHECK(!checkNameRelation(NameRelation::IS_PREFIX_OF, "/prefix/other", "/prefix"));

  BOOST_CHECK(!checkNameRelation(NameRelation::IS_STRICT_PREFIX_OF, "/prefix", "/prefix"));
  BOOST_CHECK(checkNameRelation(NameRelation::IS_STRICT_PREFIX_OF, "/prefix", "/prefix/other"));
  BOOST_CHECK(!checkNameRelation(NameRelation::IS_STRICT_PREFIX_OF, "/prefix/other", "/prefix"));
}

BOOST_AUTO_TEST_SUITE_END() // TestNameRelation
BOOST_AUTO_TEST_SUITE_END() // ValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn
