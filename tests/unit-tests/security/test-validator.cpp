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

#include "security/validator-null.hpp"
#include "security/key-chain.hpp"
#include "util/time.hpp"

#include "boost-test.hpp"

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestValidator)

void
onValidated(const shared_ptr<const Data>& data)
{
  BOOST_CHECK(true);
}

void
onValidationFailed(const shared_ptr<const Data>& data, const string& failureInfo)
{
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE(Null)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestValidator/Null");
  identity.appendVersion();

  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));

  Name dataName = identity;
  dataName.append("1");
  shared_ptr<Data> data = make_shared<Data>(dataName);

  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data, identity));

  ValidatorNull validator;

  // data must be a shared pointer
  validator.validate(*data,
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  keyChain.deleteIdentity(identity);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
