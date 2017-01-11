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

#include "security/v2/validation-error.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(TestValidationError)

BOOST_AUTO_TEST_CASE(Basic)
{
  ValidationError e1{ValidationError::Code::INVALID_SIGNATURE};
  BOOST_CHECK_EQUAL(e1.getCode(), 1);
  BOOST_CHECK_EQUAL(e1.getInfo(), "");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(e1), "Invalid signature");

  ValidationError e2{ValidationError::Code::NO_SIGNATURE, "message"};
  BOOST_CHECK_EQUAL(e2.getCode(), 2);
  BOOST_CHECK_EQUAL(e2.getInfo(), "message");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(e2), "Missing signature (message)");

  ValidationError e3{65535, "other message"};
  BOOST_CHECK_EQUAL(e3.getCode(), 65535);
  BOOST_CHECK_EQUAL(e3.getInfo(), "other message");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(e3), "Custom error code 65535 (other message)");
}

BOOST_AUTO_TEST_SUITE_END() // TestValidationError
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
