/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
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
{ BOOST_CHECK(true); }

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
