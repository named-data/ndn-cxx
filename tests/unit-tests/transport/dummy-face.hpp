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

#ifndef NDN_TESTS_UNIT_TESTS_TRANSPORT_DUMMY_FACE_HPP
#define NDN_TESTS_UNIT_TESTS_TRANSPORT_DUMMY_FACE_HPP

#include "face.hpp"
#include "transport/transport.hpp"

namespace ndn {

class DummyTransport : public Transport
{
public:
  void
  receive(const Block& block)
  {
    m_receiveCallback(block);
  }

  virtual void
  close()
  {
  }

  virtual void
  pause()
  {
  }

  virtual void
  resume()
  {
  }

  virtual void
  send(const Block& wire)
  {
    if (wire.type() == Tlv::Interest) {
      m_sentInterests->push_back(Interest(wire));
    }
    else if (wire.type() == Tlv::Data) {
      m_sentDatas->push_back(Data(wire));
    }
  }

  virtual void
  send(const Block& header, const Block& payload)
  {
    this->send(payload);
  }

public:
  std::vector<Interest>* m_sentInterests;
  std::vector<Data>*     m_sentDatas;
};


/** \brief a Face for unit testing
 */
class DummyFace : public Face
{
public:
  explicit
  DummyFace(shared_ptr<DummyTransport> transport)
    : Face(transport)
    , m_transport(transport)
  {
    m_transport->m_sentInterests = &m_sentInterests;
    m_transport->m_sentDatas     = &m_sentDatas;
  }

  /** \brief cause the Face to receive a packet
   */
  template<typename Packet>
  void
  receive(const Packet& packet)
  {
    m_transport->receive(packet.wireEncode());
  }

public:
  std::vector<Interest> m_sentInterests;
  std::vector<Data>     m_sentDatas;

private:
  shared_ptr<DummyTransport> m_transport;
};

inline shared_ptr<DummyFace>
makeDummyFace()
{
  return make_shared<DummyFace>(make_shared<DummyTransport>());
}

} // namespace ndn

#endif // NDN_TESTS_UNIT_TESTS_TRANSPORT_DUMMY_FACE_HPP
