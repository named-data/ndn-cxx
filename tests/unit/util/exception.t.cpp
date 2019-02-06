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

#include "ndn-cxx/util/exception.hpp"

#include "tests/boost-test.hpp"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/get_error_info.hpp>

namespace ndn {
namespace exception {
namespace test {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestException)

BOOST_AUTO_TEST_CASE(Throw)
{
  auto f = [] { NDN_THROW(std::invalid_argument("test")); };

  BOOST_CHECK_THROW(f(), boost::exception);
  BOOST_CHECK_THROW(f(), std::exception);
  BOOST_CHECK_THROW(f(), std::invalid_argument);

  try {
    f();
  }
  catch (const boost::exception& ex) {
    BOOST_CHECK(boost::get_error_info<boost::throw_file>(ex) != nullptr);
    BOOST_CHECK(boost::get_error_info<boost::throw_line>(ex) != nullptr);
    BOOST_CHECK(boost::get_error_info<boost::throw_function>(ex) != nullptr);

#ifdef NDN_CXX_HAVE_STACKTRACE
    auto stack = boost::get_error_info<errinfo_stacktrace>(ex);
    BOOST_REQUIRE(stack != nullptr);
    auto info = boost::diagnostic_information(ex);
    BOOST_TEST_MESSAGE(info);
    BOOST_CHECK(stack->empty() || info.find("===== Stacktrace =====") != std::string::npos);
#endif
  }
}

BOOST_AUTO_TEST_CASE(ThrowErrno)
{
  auto f = [] {
    errno = ERANGE;
    NDN_THROW_ERRNO(std::out_of_range("test"));
  };

  BOOST_CHECK_THROW(f(), boost::exception);
  BOOST_CHECK_THROW(f(), std::exception);
  BOOST_CHECK_THROW(f(), std::out_of_range);

  try {
    f();
  }
  catch (const boost::exception& ex) {
    auto errPtr = boost::get_error_info<boost::errinfo_errno>(ex);
    BOOST_REQUIRE(errPtr != nullptr);
    BOOST_CHECK_EQUAL(*errPtr, ERANGE);
  }
}

BOOST_AUTO_TEST_CASE(ThrowNested)
{
  auto f = [] {
    try {
      NDN_THROW(std::overflow_error("inner"));
    }
    catch (...) {
      NDN_THROW_NESTED(std::domain_error("outer"));
    }
  };

  BOOST_CHECK_THROW(f(), boost::exception);
  BOOST_CHECK_THROW(f(), std::exception);
  BOOST_CHECK_THROW(f(), std::domain_error);

  try {
    f();
  }
  catch (const boost::exception& ex) {
    BOOST_CHECK(boost::get_error_info<boost::errinfo_nested_exception>(ex) != nullptr);
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestException
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace test
} // namespace exception
} // namespace ndn
