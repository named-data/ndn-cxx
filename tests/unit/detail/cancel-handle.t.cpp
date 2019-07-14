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

#include "ndn-cxx/detail/cancel-handle.hpp"

#include "tests/boost-test.hpp"

namespace ndn {
namespace detail {
namespace tests {

BOOST_AUTO_TEST_SUITE(Detail)
BOOST_AUTO_TEST_SUITE(TestCancelHandle)

static CancelHandle
makeDummyCancelHandle(int& nCancels)
{
  return CancelHandle([&] { ++nCancels; });
}

BOOST_AUTO_TEST_SUITE(PlainHandle)

BOOST_AUTO_TEST_CASE(ManualCancel)
{
  int nCancels = 0;
  auto hdl = makeDummyCancelHandle(nCancels);
  BOOST_CHECK_EQUAL(nCancels, 0);

  hdl.cancel();
  BOOST_CHECK_EQUAL(nCancels, 1);

  hdl = CancelHandle();
  BOOST_CHECK_EQUAL(nCancels, 1);
}

BOOST_AUTO_TEST_SUITE_END() // PlainHandle

BOOST_AUTO_TEST_SUITE(ScopedHandle)

using ScopedTestHandle = ScopedCancelHandle<CancelHandle>;

BOOST_AUTO_TEST_CASE(ManualCancel)
{
  int nCancels = 0;
  {
    ScopedTestHandle hdl = makeDummyCancelHandle(nCancels);
    BOOST_CHECK_EQUAL(nCancels, 0);

    hdl.cancel();
    BOOST_CHECK_EQUAL(nCancels, 1);
  } // hdl goes out of scope
  BOOST_CHECK_EQUAL(nCancels, 1);
}

BOOST_AUTO_TEST_CASE(Destruct)
{
  int nCancels = 0;
  {
    ScopedTestHandle hdl = makeDummyCancelHandle(nCancels);
    BOOST_CHECK_EQUAL(nCancels, 0);
  } // hdl goes out of scope
  BOOST_CHECK_EQUAL(nCancels, 1);
}

BOOST_AUTO_TEST_CASE(Assign)
{
  int nCancels1 = 0, nCancels2 = 0;
  {
    ScopedTestHandle hdl = makeDummyCancelHandle(nCancels1);
    hdl = makeDummyCancelHandle(nCancels2);
    BOOST_CHECK_EQUAL(nCancels1, 1);
    BOOST_CHECK_EQUAL(nCancels2, 0);
  } // hdl goes out of scope
  BOOST_CHECK_EQUAL(nCancels2, 1);
}

BOOST_AUTO_TEST_CASE(Release)
{
  int nCancels = 0;
  {
    ScopedTestHandle hdl = makeDummyCancelHandle(nCancels);
    hdl.release();
    hdl.cancel(); // no effect
  } // hdl goes out of scope
  BOOST_CHECK_EQUAL(nCancels, 0);
}

BOOST_AUTO_TEST_CASE(MoveConstruct)
{
  int nCancels = 0;
  unique_ptr<ScopedTestHandle> hdl1;
  {
    ScopedTestHandle hdl2 = makeDummyCancelHandle(nCancels);
    hdl1 = make_unique<ScopedTestHandle>(std::move(hdl2));
  } // hdl2 goes out of scope
  BOOST_CHECK_EQUAL(nCancels, 0);
  hdl1.reset();
  BOOST_CHECK_EQUAL(nCancels, 1);
}

BOOST_AUTO_TEST_CASE(MoveAssign)
{
  int nCancels = 0;
  {
    ScopedTestHandle hdl1;
    {
      ScopedTestHandle hdl2 = makeDummyCancelHandle(nCancels);
      hdl1 = std::move(hdl2);
    } // hdl2 goes out of scope
    BOOST_CHECK_EQUAL(nCancels, 0);
  } // hdl1 goes out of scope
  BOOST_CHECK_EQUAL(nCancels, 1);
}

BOOST_AUTO_TEST_SUITE_END() // ScopedHandle
BOOST_AUTO_TEST_SUITE_END() // TestCancelHandle
BOOST_AUTO_TEST_SUITE_END() // Detail

} // namespace tests
} // namespace detail
} // namespace ndn
