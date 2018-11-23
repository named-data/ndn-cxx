/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2018 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#include "util/notification-subscriber.hpp"
#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"
#include "make-interest-data.hpp"
#include "simple-notification.hpp"
#include "../identity-management-time-fixture.hpp"

namespace ndn {
namespace util {
namespace tests {

using namespace ndn::tests;

class NotificationSubscriberFixture : public IdentityManagementTimeFixture
{
public:
  NotificationSubscriberFixture()
    : streamPrefix("ndn:/NotificationSubscriberTest")
    , subscriberFace(io, m_keyChain)
    , subscriber(subscriberFace, streamPrefix, 1_s)
    , nextSendNotificationNo(0)
  {
  }

  /** \brief deliver one notification to subscriber
   */
  void
  deliverNotification(const std::string& msg)
  {
    SimpleNotification notification(msg);

    Name dataName = streamPrefix;
    dataName.appendSequenceNumber(nextSendNotificationNo);
    Data data(dataName);
    data.setContent(notification.wireEncode());
    data.setFreshnessPeriod(1_s);
    m_keyChain.sign(data);

    lastDeliveredSeqNo = nextSendNotificationNo;
    lastNotification.setMessage("");
    ++nextSendNotificationNo;
    subscriberFace.receive(data);
  }

  /** \brief deliver a Nack to subscriber
   */
  void
  deliverNack(const Interest& interest, const lp::NackReason& reason)
  {
    lp::Nack nack = makeNack(interest, reason);
    subscriberFace.receive(nack);
  }

  void
  afterNotification(const SimpleNotification& notification)
  {
    lastNotification = notification;
  }

  void
  afterNack(const lp::Nack& nack)
  {
    lastNack = nack;
  }

  void
  afterTimeout()
  {
    hasTimeout = true;
  }

  void
  afterDecodeError(const Data& data)
  {
    lastDecodeErrorData = data;
  }

  void
  connectHandlers()
  {
    notificationConn = subscriber.onNotification.connect(
      bind(&NotificationSubscriberFixture::afterNotification, this, _1));
    nackConn = subscriber.onNack.connect(
      bind(&NotificationSubscriberFixture::afterNack, this, _1));
    subscriber.onTimeout.connect(
      bind(&NotificationSubscriberFixture::afterTimeout, this));
    subscriber.onDecodeError.connect(
      bind(&NotificationSubscriberFixture::afterDecodeError, this, _1));
  }

  void
  disconnectHandlers()
  {
    notificationConn.disconnect();
    nackConn.disconnect();
  }

  /** \return true if subscriberFace has an initial request (first sent Interest)
   */
  bool
  hasInitialRequest() const
  {
    if (subscriberFace.sentInterests.empty())
      return false;

    const Interest& interest = subscriberFace.sentInterests[0];
    return interest.getName() == streamPrefix &&
           interest.getChildSelector() == 1 &&
           interest.getMustBeFresh() &&
           interest.getInterestLifetime() == subscriber.getInterestLifetime();
  }

  /** \return sequence number of the continuation request sent from subscriberFace
   *          or 0 if there's no such request as sole sent Interest
   */
  uint64_t
  getRequestSeqNo() const
  {
    if (subscriberFace.sentInterests.size() != 1)
      return 0;

    const Interest& interest = subscriberFace.sentInterests[0];
    const Name& name = interest.getName();
    if (streamPrefix.isPrefixOf(name) &&
        name.size() == streamPrefix.size() + 1 &&
        interest.getInterestLifetime() == subscriber.getInterestLifetime())
      return name[-1].toSequenceNumber();
    else
      return 0;
  }

protected:
  Name streamPrefix;
  DummyClientFace subscriberFace;
  util::NotificationSubscriber<SimpleNotification> subscriber;
  util::signal::Connection notificationConn;
  util::signal::Connection nackConn;
  uint64_t nextSendNotificationNo;
  uint64_t lastDeliveredSeqNo;
  SimpleNotification lastNotification;
  lp::Nack lastNack;
  bool hasTimeout;
  Data lastDecodeErrorData;
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestNotificationSubscriber, NotificationSubscriberFixture)

BOOST_AUTO_TEST_CASE(StartStop)
{
  BOOST_REQUIRE_EQUAL(subscriber.isRunning(), false);

  // has no effect because onNotification has no handler
  subscriber.start();
  BOOST_REQUIRE_EQUAL(subscriber.isRunning(), false);

  this->connectHandlers();
  subscriber.start();
  BOOST_REQUIRE_EQUAL(subscriber.isRunning(), true);
  advanceClocks(1_ms);
  BOOST_CHECK(this->hasInitialRequest());

  subscriberFace.sentInterests.clear();
  this->disconnectHandlers();
  this->deliverNotification("n1");
  BOOST_REQUIRE_EQUAL(subscriberFace.sentInterests.size(), 0);
}

BOOST_AUTO_TEST_CASE(Notifications)
{
  this->connectHandlers();
  subscriber.start();
  advanceClocks(1_ms);

  // respond to initial request
  subscriberFace.sentInterests.clear();
  this->deliverNotification("n1");
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(lastNotification.getMessage(), "n1");
  BOOST_CHECK_EQUAL(this->getRequestSeqNo(), lastDeliveredSeqNo + 1);

  // respond to continuation request
  subscriberFace.sentInterests.clear();
  this->deliverNotification("n2");
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(lastNotification.getMessage(), "n2");
  BOOST_CHECK_EQUAL(this->getRequestSeqNo(), lastDeliveredSeqNo + 1);
}

BOOST_AUTO_TEST_CASE(Nack)
{
  this->connectHandlers();
  subscriber.start();
  advanceClocks(1_ms);

  // send the first Nack to initial request
  BOOST_REQUIRE_EQUAL(subscriberFace.sentInterests.size(), 1);
  Interest interest = subscriberFace.sentInterests[0];
  subscriberFace.sentInterests.clear();
  this->deliverNack(interest, lp::NackReason::CONGESTION);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(lastNack.getReason(), lp::NackReason::CONGESTION);
  BOOST_REQUIRE_EQUAL(this->hasInitialRequest(), false);
  advanceClocks(300_ms);
  BOOST_REQUIRE_EQUAL(this->hasInitialRequest(), true);

  // send the second Nack to initial request
  BOOST_REQUIRE_EQUAL(subscriberFace.sentInterests.size(), 1);
  interest = subscriberFace.sentInterests[0];
  subscriberFace.sentInterests.clear();
  this->deliverNack(interest, lp::NackReason::CONGESTION);
  advanceClocks(301_ms);
  BOOST_REQUIRE_EQUAL(this->hasInitialRequest(), false);
  advanceClocks(200_ms);
  BOOST_REQUIRE_EQUAL(this->hasInitialRequest(), true);

  // send a notification to initial request
  subscriberFace.sentInterests.clear();
  this->deliverNotification("n1");
  advanceClocks(1_ms);

  // send a Nack to subsequent request
  BOOST_REQUIRE_EQUAL(subscriberFace.sentInterests.size(), 1);
  interest = subscriberFace.sentInterests[0];
  subscriberFace.sentInterests.clear();
  this->deliverNack(interest, lp::NackReason::CONGESTION);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(lastNack.getReason(), lp::NackReason::CONGESTION);
  BOOST_REQUIRE_EQUAL(this->hasInitialRequest(), false);
  advanceClocks(300_ms);
  BOOST_REQUIRE_EQUAL(this->hasInitialRequest(), true);
}

BOOST_AUTO_TEST_CASE(Timeout)
{
  this->connectHandlers();
  subscriber.start();
  advanceClocks(1_ms);

  subscriberFace.sentInterests.clear();
  lastNotification.setMessage("");
  advanceClocks(subscriber.getInterestLifetime(), 2);
  BOOST_CHECK(lastNotification.getMessage().empty());
  BOOST_CHECK_EQUAL(hasTimeout, true);
  BOOST_CHECK(this->hasInitialRequest());

  subscriberFace.sentInterests.clear();
  this->deliverNotification("n1");
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(lastNotification.getMessage(), "n1");
}

BOOST_AUTO_TEST_CASE(SequenceError)
{
  this->connectHandlers();
  subscriber.start();
  advanceClocks(1_ms);

  Name wrongName = streamPrefix;
  wrongName.append("%07%07");
  Data wrongData(wrongName);
  m_keyChain.sign(wrongData);
  subscriberFace.receive(wrongData);
  subscriberFace.sentInterests.clear();
  lastNotification.setMessage("");
  advanceClocks(1_ms);
  BOOST_CHECK(lastNotification.getMessage().empty());
  BOOST_CHECK_EQUAL(lastDecodeErrorData.getName(), wrongName);
  BOOST_CHECK(this->hasInitialRequest());
}

BOOST_AUTO_TEST_CASE(PayloadError)
{
  this->connectHandlers();
  subscriber.start();
  advanceClocks(1_ms);

  subscriberFace.sentInterests.clear();
  lastNotification.setMessage("");
  this->deliverNotification("\x07n4");
  advanceClocks(1_ms);
  BOOST_CHECK(lastNotification.getMessage().empty());
  BOOST_CHECK(this->hasInitialRequest());
}

BOOST_AUTO_TEST_SUITE_END() // TestNotificationSubscriber
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
