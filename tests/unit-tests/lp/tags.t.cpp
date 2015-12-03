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

#include "lp/tags.hpp"
#include "encoding/nfd-constants.hpp"
#include "interest.hpp"
#include "data.hpp"
#include "lp/nack.hpp"

#include <boost/mpl/vector.hpp>
#include "boost-test.hpp"

#ifdef NDN_LP_KEEP_LOCAL_CONTROL_HEADER
#include "management/nfd-local-control-header.hpp"
#endif // NDN_LP_KEEP_LOCAL_CONTROL_HEADER

namespace ndn {
namespace lp {
namespace tests {

BOOST_AUTO_TEST_SUITE(Lp)
BOOST_AUTO_TEST_SUITE(TestTags)

#ifdef NDN_LP_KEEP_LOCAL_CONTROL_HEADER

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

BOOST_AUTO_TEST_SUITE(Facade)

typedef boost::mpl::vector<Interest, Data, Nack> TagHostTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(IncomingFaceId, T, TagHostTypes)
{
  T pkt;
  LocalControlHeaderFacade lch(pkt);

  BOOST_CHECK_EQUAL(lch.hasIncomingFaceId(), false);

  lch.setIncomingFaceId(303);
  shared_ptr<IncomingFaceIdTag> tag = static_cast<TagHost&>(pkt).getTag<IncomingFaceIdTag>();
  BOOST_REQUIRE(tag != nullptr);
  BOOST_CHECK_EQUAL(*tag, 303);

  lch.setIncomingFaceId(ndn::nfd::INVALID_FACE_ID);
  BOOST_CHECK(static_cast<TagHost&>(pkt).getTag<IncomingFaceIdTag>() == nullptr);

  static_cast<TagHost&>(pkt).setTag(make_shared<IncomingFaceIdTag>(104));
  BOOST_CHECK_EQUAL(lch.getIncomingFaceId(), 104);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(NextHopFaceId, T, TagHostTypes)
{
  T pkt;
  LocalControlHeaderFacade lch(pkt);

  BOOST_CHECK_EQUAL(lch.hasNextHopFaceId(), false);

  lch.setNextHopFaceId(303);
  shared_ptr<NextHopFaceIdTag> tag = static_cast<TagHost&>(pkt).getTag<NextHopFaceIdTag>();
  BOOST_REQUIRE(tag != nullptr);
  BOOST_CHECK_EQUAL(*tag, 303);

  lch.setNextHopFaceId(ndn::nfd::INVALID_FACE_ID);
  BOOST_CHECK(static_cast<TagHost&>(pkt).getTag<NextHopFaceIdTag>() == nullptr);

  static_cast<TagHost&>(pkt).setTag(make_shared<NextHopFaceIdTag>(104));
  BOOST_CHECK_EQUAL(lch.getNextHopFaceId(), 104);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CachePolicy, T, TagHostTypes)
{
  using lp::CachePolicy;

  T pkt;
  LocalControlHeaderFacade lch(pkt);

  BOOST_CHECK_EQUAL(lch.hasCachingPolicy(), false);

  lch.setCachingPolicy(LocalControlHeaderFacade::NO_CACHE);
  shared_ptr<CachePolicyTag> tag = static_cast<TagHost&>(pkt).getTag<CachePolicyTag>();
  BOOST_REQUIRE(tag != nullptr);
  BOOST_CHECK_EQUAL(tag->get().getPolicy(), CachePolicyType::NO_CACHE);

  lch.setCachingPolicy(LocalControlHeaderFacade::INVALID_POLICY);
  BOOST_CHECK(static_cast<TagHost&>(pkt).getTag<CachePolicyTag>() == nullptr);

  auto tag2 = make_shared<CachePolicyTag>(CachePolicy().setPolicy(CachePolicyType::NO_CACHE));
  static_cast<TagHost&>(pkt).setTag(tag2);
  BOOST_CHECK_EQUAL(lch.getCachingPolicy(), LocalControlHeaderFacade::NO_CACHE);
}

BOOST_AUTO_TEST_SUITE_END() // Facade

BOOST_AUTO_TEST_CASE(InterestGetters)
{
  Interest interest;

  interest.setTag(make_shared<IncomingFaceIdTag>(319));
  BOOST_CHECK_EQUAL(interest.getLocalControlHeader().getIncomingFaceId(), 319);
  BOOST_CHECK_EQUAL(interest.getIncomingFaceId(), 319);

  interest.setTag(make_shared<NextHopFaceIdTag>(213));
  BOOST_CHECK_EQUAL(interest.getLocalControlHeader().getNextHopFaceId(), 213);
  BOOST_CHECK_EQUAL(interest.getNextHopFaceId(), 213);
}

BOOST_AUTO_TEST_CASE(InterestSetters)
{
  Interest interest;

  interest.getLocalControlHeader().setIncomingFaceId(268);
  shared_ptr<IncomingFaceIdTag> incomingFaceIdTag = interest.getTag<IncomingFaceIdTag>();
  BOOST_REQUIRE(incomingFaceIdTag != nullptr);
  BOOST_CHECK_EQUAL(*incomingFaceIdTag, 268);

  interest.setIncomingFaceId(153);
  incomingFaceIdTag = interest.getTag<IncomingFaceIdTag>();
  BOOST_REQUIRE(incomingFaceIdTag != nullptr);
  BOOST_CHECK_EQUAL(*incomingFaceIdTag, 153);

  interest.getLocalControlHeader().setNextHopFaceId(307);
  shared_ptr<NextHopFaceIdTag> nextHopFaceIdTag = interest.getTag<NextHopFaceIdTag>();
  BOOST_REQUIRE(nextHopFaceIdTag != nullptr);
  BOOST_CHECK_EQUAL(*nextHopFaceIdTag, 307);

  interest.setNextHopFaceId(260);
  nextHopFaceIdTag = interest.getTag<NextHopFaceIdTag>();
  BOOST_REQUIRE(nextHopFaceIdTag != nullptr);
  BOOST_CHECK_EQUAL(*nextHopFaceIdTag, 260);
}

BOOST_AUTO_TEST_CASE(DataGetters)
{
  Data data;

  data.setTag(make_shared<IncomingFaceIdTag>(16));
  BOOST_CHECK_EQUAL(data.getLocalControlHeader().getIncomingFaceId(), 16);
  BOOST_CHECK_EQUAL(data.getIncomingFaceId(), 16);

  data.setTag(make_shared<CachePolicyTag>(CachePolicy().setPolicy(CachePolicyType::NO_CACHE)));
  BOOST_CHECK_EQUAL(data.getLocalControlHeader().getCachingPolicy(), nfd::LocalControlHeader::NO_CACHE);
  BOOST_CHECK_EQUAL(data.getCachingPolicy(), nfd::LocalControlHeader::NO_CACHE);
}

BOOST_AUTO_TEST_CASE(DataSetters)
{
  Data data;

  data.getLocalControlHeader().setIncomingFaceId(297);
  shared_ptr<IncomingFaceIdTag> incomingFaceIdTag = data.getTag<IncomingFaceIdTag>();
  BOOST_REQUIRE(incomingFaceIdTag != nullptr);
  BOOST_CHECK_EQUAL(*incomingFaceIdTag, 297);

  data.setIncomingFaceId(233);
  incomingFaceIdTag = data.getTag<IncomingFaceIdTag>();
  BOOST_REQUIRE(incomingFaceIdTag != nullptr);
  BOOST_CHECK_EQUAL(*incomingFaceIdTag, 233);

  data.getLocalControlHeader().setCachingPolicy(nfd::LocalControlHeader::NO_CACHE);
  shared_ptr<CachePolicyTag> cachePolicyTag = data.getTag<CachePolicyTag>();
  BOOST_REQUIRE(cachePolicyTag != nullptr);
  BOOST_CHECK_EQUAL(cachePolicyTag->get().getPolicy(), CachePolicyType::NO_CACHE);

  data.setCachingPolicy(nfd::LocalControlHeader::INVALID_POLICY);
  cachePolicyTag = data.getTag<CachePolicyTag>();
  BOOST_CHECK(cachePolicyTag == nullptr);
}

#pragma GCC diagnostic pop

#endif // NDN_LP_KEEP_LOCAL_CONTROL_HEADER

BOOST_AUTO_TEST_SUITE_END() // TestTags
BOOST_AUTO_TEST_SUITE_END() // Lp

} // namespace tests
} // namespace lp
} // namespace ndn
