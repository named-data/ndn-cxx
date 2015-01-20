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

/**
 * Original copyright notice from NFD:
 *
 * Copyright (c) 2014,  Regents of the University of California,
 *                      Arizona Board of Regents,
 *                      Colorado State University,
 *                      University Pierre & Marie Curie, Sorbonne University,
 *                      Washington University in St. Louis,
 *                      Beijing Institute of Technology,
 *                      The University of Memphis
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NDN_UTIL_NOTIFICATION_SUBSCRIBER_HPP
#define NDN_UTIL_NOTIFICATION_SUBSCRIBER_HPP

#include "../face.hpp"
#include "signal.hpp"
#include "concepts.hpp"
#include <boost/concept_check.hpp>

namespace ndn {
namespace util {

/** \brief provides a subscriber of Notification Stream
 *  \sa http://redmine.named-data.net/projects/nfd/wiki/Notification
 *  \tparam Notification type of Notification item, appears in payload of Data packets
 */
template<typename Notification>
class NotificationSubscriber : noncopyable
{
public:
  BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Notification>));
  BOOST_CONCEPT_ASSERT((WireDecodable<Notification>));

  /** \brief construct a NotificationSubscriber
   *  \note The subscriber is not started after construction.
   *        User should add one or more handlers to onNotification, and invoke .start().
   */
  NotificationSubscriber(Face& face, const Name& prefix,
                         const time::milliseconds& interestLifetime = time::milliseconds(60000))
    : m_face(face)
    , m_prefix(prefix)
    , m_isRunning(false)
    , m_lastSequenceNo(std::numeric_limits<uint64_t>::max())
    , m_lastInterestId(0)
    , m_interestLifetime(interestLifetime)
  {
  }

  virtual
  ~NotificationSubscriber()
  {
  }

  /** \return InterestLifetime of Interests to retrieve notifications
   *  \details This must be greater than FreshnessPeriod of Notification Data packets,
   *           to ensure correct operation of this subscriber implementation.
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
  start()
  {
    if (m_isRunning) // already running
      return;
    m_isRunning = true;

    this->sendInitialInterest();
  }

  /** \brief stop receiving notifications
   */
  void
  stop()
  {
    if (!m_isRunning) // not running
      return;
    m_isRunning = false;

    if (m_lastInterestId != 0)
      m_face.removePendingInterest(m_lastInterestId);
    m_lastInterestId = 0;
  }

public: // subscriptions
  /** \brief fires when a Notification is received
   *  \note Removing all handlers will cause the subscriber to stop.
   */
  signal::Signal<NotificationSubscriber, Notification> onNotification;

  /** \brief fires when no Notification is received within .getInterestLifetime period
   */
  signal::Signal<NotificationSubscriber> onTimeout;

  /** \brief fires when a Data packet in the Notification Stream cannot be decoded as Notification
   */
  signal::Signal<NotificationSubscriber, Data> onDecodeError;

private:
  void
  sendInitialInterest()
  {
    if (this->shouldStop())
      return;

    shared_ptr<Interest> interest = make_shared<Interest>(m_prefix);
    interest->setMustBeFresh(true);
    interest->setChildSelector(1);
    interest->setInterestLifetime(getInterestLifetime());

    m_lastInterestId = m_face.expressInterest(*interest,
                         bind(&NotificationSubscriber<Notification>::afterReceiveData, this, _2),
                         bind(&NotificationSubscriber<Notification>::afterTimeout, this));
  }

  void
  sendNextInterest()
  {
    if (this->shouldStop())
      return;

    BOOST_ASSERT(m_lastSequenceNo !=
                 std::numeric_limits<uint64_t>::max());// overflow or missing initial reply

    Name nextName = m_prefix;
    nextName.appendSequenceNumber(m_lastSequenceNo + 1);

    shared_ptr<Interest> interest = make_shared<Interest>(nextName);
    interest->setInterestLifetime(getInterestLifetime());

    m_lastInterestId = m_face.expressInterest(*interest,
                         bind(&NotificationSubscriber<Notification>::afterReceiveData, this, _2),
                         bind(&NotificationSubscriber<Notification>::afterTimeout, this));
  }

  /** \brief Check if the subscriber is or should be stopped.
   *  \return true if the subscriber is stopped.
   */
  bool
  shouldStop()
  {
    if (!m_isRunning)
      return true;
    if (onNotification.isEmpty()) {
      this->stop();
      return true;
    }
    return false;
  }

  void
  afterReceiveData(const Data& data)
  {
    if (this->shouldStop())
      return;

    Notification notification;
    try {
      m_lastSequenceNo = data.getName().get(-1).toSequenceNumber();
      notification.wireDecode(data.getContent().blockFromValue());
    }
    catch (tlv::Error&) {
      this->onDecodeError(data);
      this->sendInitialInterest();
      return;
    }

    this->onNotification(notification);

    this->sendNextInterest();
  }

  void
  afterTimeout()
  {
    if (this->shouldStop())
      return;

    this->onTimeout();

    this->sendInitialInterest();
  }

private:
  Face& m_face;
  Name m_prefix;
  bool m_isRunning;
  uint64_t m_lastSequenceNo;
  const PendingInterestId* m_lastInterestId;
  time::milliseconds m_interestLifetime;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NOTIFICATION_SUBSCRIBER_HPP
