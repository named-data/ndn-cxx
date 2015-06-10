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

#ifndef NDN_UTIL_NOTIFICATION_STREAM_HPP
#define NDN_UTIL_NOTIFICATION_STREAM_HPP

#include "../name.hpp"
#include "../face.hpp"
#include "../security/key-chain.hpp"

#include "concepts.hpp"

namespace ndn {

namespace util {

/** \brief provides a publisher of Notification Stream
 *  \sa http://redmine.named-data.net/projects/nfd/wiki/Notification
 */
template<typename Notification>
class NotificationStream : noncopyable
{
public:
  BOOST_CONCEPT_ASSERT((WireEncodable<Notification>));

  NotificationStream(Face& face, const Name& prefix, KeyChain& keyChain)
    : m_face(face)
    , m_prefix(prefix)
    , m_keyChain(keyChain)
    , m_sequenceNo(0)
  {
  }

  virtual
  ~NotificationStream()
  {
  }

  void
  postNotification(const Notification& notification)
  {
    Name dataName = m_prefix;
    dataName.appendSequenceNumber(m_sequenceNo);

    shared_ptr<Data> data = make_shared<Data>(dataName);
    data->setContent(notification.wireEncode());
    data->setFreshnessPeriod(time::seconds(1));

    m_keyChain.sign(*data);
    m_face.put(*data);

    ++m_sequenceNo;
  }

private:
  Face& m_face;
  const Name m_prefix;
  KeyChain& m_keyChain;
  uint64_t m_sequenceNo;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NOTIFICATION_STREAM_HPP
