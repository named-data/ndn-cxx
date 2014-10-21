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

#ifndef NDN_TESTS_UNIT_TESTS_DUMMY_CLIENT_FACE_HPP
#define NDN_TESTS_UNIT_TESTS_DUMMY_CLIENT_FACE_HPP

#include "face.hpp"
#include "transport/transport.hpp"
#include "management/nfd-controller.hpp"
#include "management/nfd-control-response.hpp"
#include "util/event-emitter.hpp"

namespace ndn {
namespace tests {

class DummyClientTransport : public ndn::Transport
{
public:
  void
  receive(const Block& block)
  {
    if (static_cast<bool>(m_receiveCallback))
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
    if (wire.type() == tlv::Interest) {
      shared_ptr<Interest> interest = make_shared<Interest>(wire);
      (*m_onInterest)(*interest, this);
    }
    else if (wire.type() == tlv::Data) {
      shared_ptr<Data> data = make_shared<Data>(wire);
      (*m_onData)(*data, this);
    }
  }

  virtual void
  send(const Block& header, const Block& payload)
  {
    this->send(payload);
  }

  boost::asio::io_service&
  getIoService()
  {
    return *m_ioService;
  }

private:
  friend class DummyClientFace;
  util::EventEmitter<Interest, DummyClientTransport*>* m_onInterest;
  util::EventEmitter<Data, DummyClientTransport*>* m_onData;
};


/** \brief Callback to connect
 */
inline void
replyNfdRibCommands(const Interest& interest, DummyClientTransport* transport)
{
  static const Name localhostRegistration("/localhost/nfd/rib");
  if (localhostRegistration.isPrefixOf(interest.getName())) {
    shared_ptr<Data> okResponse = make_shared<Data>(interest.getName());
    nfd::ControlParameters params(interest.getName().get(-5).blockFromValue());
    params.setFaceId(1);
    params.setOrigin(0);
    if (interest.getName().get(3) == name::Component("register")) {
      params.setCost(0);
    }
    nfd::ControlResponse resp;
    resp.setCode(200);
    resp.setBody(params.wireEncode());
    okResponse->setContent(resp.wireEncode());
    KeyChain keyChain;
    keyChain.signWithSha256(*okResponse);

    transport->getIoService().post(bind(&DummyClientTransport::receive, transport,
                                        okResponse->wireEncode()));
  }
}

/** \brief a client-side face for unit testing
 */
class DummyClientFace : public ndn::Face
{
public:
  explicit
  DummyClientFace(shared_ptr<DummyClientTransport> transport)
    : Face(transport)
    , m_transport(transport)
  {
    m_transport->m_onInterest = &onInterest;
    m_transport->m_onData     = &onData;

    enablePacketLogging();
  }

  DummyClientFace(shared_ptr<DummyClientTransport> transport, boost::asio::io_service& ioService)
    : Face(transport, ioService)
    , m_transport(transport)
  {
    m_transport->m_onInterest = &onInterest;
    m_transport->m_onData     = &onData;

    enablePacketLogging();
  }

  /** \brief cause the Face to receive a packet
   */
  template<typename Packet>
  void
  receive(const Packet& packet)
  {
    m_transport->receive(packet.wireEncode());
  }

  void
  enablePacketLogging()
  {
    // @todo Replace with C++11 lambdas

    onInterest += bind(static_cast<void(std::vector<Interest>::*)(const Interest&)>(
                         &std::vector<Interest>::push_back),
                       &m_sentInterests, _1);

    onData += bind(static_cast<void(std::vector<Data>::*)(const Data&)>(
                     &std::vector<Data>::push_back),
                   &m_sentDatas, _1);
  }

  void
  enableRegistrationReply()
  {
    onInterest += &replyNfdRibCommands;
  }

public:
  /** \brief sent Interests
   *  \note After .expressInterest, .processEvents must be called before
   *        the Interest would show up here.
   */
  std::vector<Interest> m_sentInterests;
  /** \brief sent Datas
   *  \note After .put, .processEvents must be called before
   *        the Interest would show up here.
   */
  std::vector<Data>     m_sentDatas;

public:
  /** \brief Event to be called whenever an Interest is received
   *  \note After .expressInterest, .processEvents must be called before
   *        the Interest would show up here.
   */
  util::EventEmitter<Interest, DummyClientTransport*> onInterest;

  /** \brief Event to be called whenever a Data packet is received
   *  \note After .put, .processEvents must be called before
   *        the Interest would show up here.
   */
  util::EventEmitter<Data, DummyClientTransport*> onData;

private:
  shared_ptr<DummyClientTransport> m_transport;
};

inline shared_ptr<DummyClientFace>
makeDummyClientFace()
{
  return make_shared<DummyClientFace>(make_shared<DummyClientTransport>());
}

inline shared_ptr<DummyClientFace>
makeDummyClientFace(boost::asio::io_service& ioService)
{
  return make_shared<DummyClientFace>(make_shared<DummyClientTransport>(), ref(ioService));
}


} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_UNIT_TESTS_DUMMY_CLIENT_FACE_HPP
