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
 */

#include "interest-filter.hpp"
#include "data.hpp"
#include "encoding/buffer-stream.hpp"
#include "security/signature-sha256-with-rsa.hpp"
#include "security/digest-sha256.hpp"
#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestInterestFilter)

BOOST_AUTO_TEST_CASE(Matching)
{
  BOOST_CHECK_EQUAL(InterestFilter("/a").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b/c").doesMatch("/a/b"), false);

  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b>").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<b>").doesMatch("/a/b"), false);

  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<b>").doesMatch("/a/b/c/b"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<>*<b>").doesMatch("/a/b/c/b"), true);

  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b>").doesMatch("/a/b/c/d"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>*").doesMatch("/a/b/c/d"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>*").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>+").doesMatch("/a/b"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>+").doesMatch("/a/b/c"), true);
}

BOOST_AUTO_TEST_CASE(RegexConvertToName)
{
  util::DummyClientFace face;
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
    [] (const Name&, const Interest&) { BOOST_ERROR("unexpected Interest"); });
  face.processEvents(1_ms);
  BOOST_CHECK_THROW(face.receive(Interest("/Hello/World/a/b/c")), InterestFilter::Error);
}

BOOST_AUTO_TEST_CASE(AllowLoopback)
{
  InterestFilter filter("/A");
  BOOST_CHECK_EQUAL(filter.allowsLoopback(), true);
  BOOST_CHECK_EQUAL(&filter.allowLoopback(false), &filter);
  BOOST_CHECK_EQUAL(filter.allowsLoopback(), false);
}

BOOST_AUTO_TEST_SUITE_END() // TestInterestFilter

} // namespace tests
} // namespace ndn
