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

#ifndef NDN_UTIL_NOTIFICATION_SUBSCRIBER_HPP
#define NDN_UTIL_NOTIFICATION_SUBSCRIBER_HPP

#include "../face.hpp"
#include "signal.hpp"
#include "concepts.hpp"
#include "time.hpp"
#include "scheduler.hpp"
#include "scheduler-scoped-event-id.hpp"

namespace ndn {
namespace util {

class NotificationSubscriberBase : noncopyable
{
public:
  virtual
  ~NotificationSubscriberBase();

  /** \return InterestLifetime of Interests to retrieve notifications
   *
   *  This must be greater than FreshnessPeriod of Notification Data packets,
   *  to ensure correct operation of this subscriber implementation.
   */
  time::milliseconds
  getInterestLifetime() const
  {
    return m_interestLifetime;
  }

  bool
  isRunning() const
  {
    return m_isRunning;
  }

  /** \brief start or resume receiving notifications
   *  \note onNotification must have at least one listener,
   *        otherwise this operation has no effect.
   */
  void
  start();

  /** \brief stop receiving notifications
   */
  void
  stop();

protected:
  /** \brief construct a NotificationSubscriber
   *  \note The subscriber is not started after construction.
   *        User should add one or more handlers to onNotification, and invoke .start().
   */
  NotificationSubscriberBase(Face& face, const Name& prefix,
                             time::milliseconds interestLifetime);

private:
  void
  sendInitialInterest();

  void
  sendNextInterest();

  virtual bool
  hasSubscriber() const = 0;

  /** \brief Check if the subscriber is or should be stopped.
   *  \return true if the subscriber is stopped.
   */
  bool
  shouldStop();

  void
  afterReceiveData(const Data& data);

  /** \brief decode the Data as a notification, and deliver it to subscribers
   *  \return whether decode was successful
   */
  virtual bool
  decodeAndDeliver(const Data& data) = 0;

  void
  afterReceiveNack(const lp::Nack& nack);

  void
  afterTimeout();

  time::milliseconds
  exponentialBackoff(lp::Nack nack);

public:
  /** \brief fires when a NACK is received
   */
  signal::Signal<NotificationSubscriberBase, lp::Nack> onNack;

  /** \brief fires when no Notification is received within .getInterestLifetime period
   */
  signal::Signal<NotificationSubscriberBase> onTimeout;

  /** \brief fires when a Data packet in the Notification Stream cannot be decoded as Notification
   */
  signal::Signal<NotificationSubscriberBase, Data> onDecodeError;

private:
  Face& m_face;
  Name m_prefix;
  bool m_isRunning;
  uint64_t m_lastSequenceNo;
  uint64_t m_lastNackSequenceNo;
  uint64_t m_attempts;
  util::scheduler::Scheduler m_scheduler;
  util::scheduler::ScopedEventId m_nackEvent;
  const PendingInterestId* m_lastInterestId;
  time::milliseconds m_interestLifetime;
};

/** \brief provides a subscriber of Notification Stream
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/Notification
 *  \tparam Notification type of Notification item, appears in payload of Data packets
 */
template<typename Notification>
class NotificationSubscriber : public NotificationSubscriberBase
{
public:
  BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Notification>));
  BOOST_CONCEPT_ASSERT((WireDecodable<Notification>));

  /** \brief construct a NotificationSubscriber
   *  \note The subscriber is not started after construction.
   *        User should add one or more handlers to onNotification, and invoke .start().
   */
  NotificationSubscriber(Face& face, const Name& prefix,
                         time::milliseconds interestLifetime = 1_min)
    : NotificationSubscriberBase(face, prefix, interestLifetime)
  {
  }

public:
  /** \brief fires when a Notification is received
   *  \note Removing all handlers will cause the subscriber to stop.
   */
  signal::Signal<NotificationSubscriber, Notification> onNotification;

private:
  bool
  hasSubscriber() const override
  {
    return !onNotification.isEmpty();
  }

  bool
  decodeAndDeliver(const Data& data) override
  {
    Notification notification;
    try {
      notification.wireDecode(data.getContent().blockFromValue());
    }
    catch (const tlv::Error&) {
      return false;
    }

    onNotification(notification);
    return true;
  }
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NOTIFICATION_SUBSCRIBER_HPP
